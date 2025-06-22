/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-13
 * @brief       uC/OS-III �����ź����Ż����ȼ���תʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ��ӢF103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "uc-os3_demo.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"
/*uC/OS-III*********************************************************************************************/
#include "os.h"
#include "cpu.h"

/******************************************************************************************************/
/*uC/OS-III����*/

/* START_TASK ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define START_TASK_PRIO 2                               /* �������ȼ� */
#define START_STK_SIZE  512                             /* ����ջ��С */
OS_TCB                  StartTask_TCB;                  /* ������ƿ� */
CPU_STK                 StartTask_STK[START_STK_SIZE];  /* ����ջ */
void start_task(void *p_arg);                           /* ������ */

/* TASK1 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK1_PRIO      (OS_CFG_PRIO_MAX - 6)           /* �������ȼ� */
#define TASK1_STK_SIZE  1024                            /* ����ջ��С */
OS_TCB                  Task1Task_TCB;                  /* ������ƿ� */
CPU_STK                 *Task1Task_STK;                 /* ����ջ */
void task1(void *p_arg);                                /* ������ */

/* TASK2 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK2_PRIO      (OS_CFG_PRIO_MAX - 5)           /* �������ȼ� */
#define TASK2_STK_SIZE  1024                            /* ����ջ��С */
OS_TCB                  Task2Task_TCB;                  /* ������ƿ� */
CPU_STK                 *Task2Task_STK;                 /* ����ջ */
void task2(void *p_arg);                                /* ������ */

/* TASK3 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK3_PRIO      (OS_CFG_PRIO_MAX - 4)           /* �������ȼ� */
#define TASK3_STK_SIZE  1024                            /* ����ջ��С */
OS_TCB                  Task3Task_TCB;                  /* ������ƿ� */
CPU_STK                 *Task3Task_STK;                 /* ����ջ */
void task3(void *p_arg);                                /* ������ */

OS_MUTEX mutex;

/******************************************************************************************************/

/**
 * @brief       uC/OS-III������ں���
 * @param       ��
 * @retval      ��
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "Mutex Pri Overturn", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* ��ʼ��uC/OS-III */
    OSInit(&err);
    
    /* ����Start Task */
    OSTaskCreate(   (OS_TCB        *)&StartTask_TCB,
                    (CPU_CHAR      *)"start_task",
                    (OS_TASK_PTR    )start_task,
                    (void          *)0,
                    (OS_PRIO        )START_TASK_PRIO,
                    (CPU_STK       *)StartTask_STK,
                    (CPU_STK_SIZE   )START_STK_SIZE / 10,
                    (CPU_STK_SIZE   )START_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* ��ʼ������� */
    OSStart(&err);
    
    for (;;)
    {
        /* ����������� */
    }
}

/**
 * @brief       start_task
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_INT32U cnts;
    
    /* ��ʼ��CPU�� */
    CPU_Init();
    
    /* �������õĽ���Ƶ������SysTick */
    cnts = (CPU_INT32U)(HAL_RCC_GetSysClockFreq() / OSCfg_TickRate_Hz);
    OS_CPU_SysTickInit(cnts);
    
    /* ����ʱ��Ƭ���ȣ�ʱ��Ƭ��ΪĬ��ֵ */
    OSSchedRoundRobinCfg(OS_TRUE, 0, &err);
    
    /* ���������ź��� */
    OSMutexCreate(  (OS_MUTEX  *)&mutex,
                    (CPU_CHAR  *)"mutex",
                    (OS_ERR    *)&err);
    
    /* ����Task1 */
    Task1Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK1_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate(   (OS_TCB        *)&Task1Task_TCB,
                    (CPU_CHAR      *)"task1",
                    (OS_TASK_PTR    )task1,
                    (void          *)0,
                    (OS_PRIO        )TASK1_PRIO,
                    (CPU_STK       *)Task1Task_STK,
                    (CPU_STK_SIZE   )TASK1_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK1_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* ����Task2 */
    Task2Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK2_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate(   (OS_TCB        *)&Task2Task_TCB,
                    (CPU_CHAR      *)"task2",
                    (OS_TASK_PTR    )task2,
                    (void          *)0,
                    (OS_PRIO        )TASK2_PRIO,
                    (CPU_STK       *)Task2Task_STK,
                    (CPU_STK_SIZE   )TASK2_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK2_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* ����Task3 */
    Task3Task_STK = (CPU_STK *)mymalloc(SRAMIN, TASK3_STK_SIZE * sizeof(CPU_STK));
    OSTaskCreate(   (OS_TCB        *)&Task3Task_TCB,
                    (CPU_CHAR      *)"task3",
                    (OS_TASK_PTR    )task3,
                    (void          *)0,
                    (OS_PRIO        )TASK3_PRIO,
                    (CPU_STK       *)Task3Task_STK,
                    (CPU_STK_SIZE   )TASK3_STK_SIZE / 10,
                    (CPU_STK_SIZE   )TASK3_STK_SIZE,
                    (OS_MSG_QTY     )0,
                    (OS_TICK        )0,
                    (void          *)0,
                    (OS_OPT         )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR        *)&err);
    
    /* ɾ��Start Task */
    OSTaskDel((OS_TCB *)0, &err);
}

/**
 * @brief       task1
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task1(void *p_arg)
{
    OS_ERR err;
    
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);
    while (1)
    {
        printf("task1 ready to pend mutex\r\n");
        OSMutexPend((OS_MUTEX  *)&mutex,
                    (OS_TICK    )0,
                    (OS_OPT     )OS_OPT_PEND_BLOCKING,
                    (CPU_TS    *)0,
                    (OS_ERR    *)&err);
        printf("task1 has pended mutex\r\n");
        printf("task1 runing\r\n");
        printf("task1 post mutex\r\n");
        OSMutexPost((OS_MUTEX  *)&mutex,
                    (OS_OPT     )OS_OPT_POST_NONE,
                    (OS_ERR    *)&err);
        OSTimeDly(100, OS_OPT_TIME_DLY, &err);
    }
}

/**
 * @brief       task2
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task2(void *p_arg)
{
    uint32_t task2_num = 0;
    OS_ERR err;
    
    OSTimeDly(200, OS_OPT_TIME_DLY, &err);
    while (1)
    {
        for (task2_num=0; task2_num<5; task2_num++)
        {
            printf("task2 runing\r\n");
            delay_ms(100);
        }
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}

/**
 * @brief       task3
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *p_arg)
{
    uint32_t task3_num = 0;
    OS_ERR err;
    
    while (1)
    {
        printf("task3 ready to pend mutex\r\n");
        OSMutexPend((OS_MUTEX  *)&mutex,
                    (OS_TICK    )0,
                    (OS_OPT     )OS_OPT_PEND_BLOCKING,
                    (CPU_TS    *)0,
                    (OS_ERR    *)&err);
        printf("task3 has pend mutex\r\n");
        for (task3_num=0; task3_num<5; task3_num++)
        {
            printf("task3 runing\r\n");
            delay_ms(100);
        }
        printf("task3 post mutex\r\n");
        OSMutexPost((OS_MUTEX  *)&mutex,
                    (OS_OPT     )OS_OPT_POST_NONE,
                    (OS_ERR    *)&err);
        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}
