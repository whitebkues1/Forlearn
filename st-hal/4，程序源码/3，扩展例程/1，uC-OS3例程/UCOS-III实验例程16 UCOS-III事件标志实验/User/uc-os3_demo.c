/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-13
 * @brief       uC/OS-III �¼���־ʵ��
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
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
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
#define TASK1_PRIO      (OS_CFG_PRIO_MAX - 4)           /* �������ȼ� */
#define TASK1_STK_SIZE  1024                            /* ����ջ��С */
OS_TCB                  Task1Task_TCB;                  /* ������ƿ� */
CPU_STK                 *Task1Task_STK;                 /* ����ջ */
void task1(void *p_arg);                                /* ������ */

/* TASK2 ���� ����
 * ����: �������ȼ� ����ջ��С ������ƿ� ����ջ ������
 */
#define TASK2_PRIO      (OS_CFG_PRIO_MAX - 4)           /* �������ȼ� */
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

OS_FLAG_GRP flag;

#define FLAGBIT_0   (1 << 0)
#define FLAGBIT_1   (1 << 1)
#define FLAGBIT_ALL (FLAGBIT_0 | FLAGBIT_1)

/******************************************************************************************************/

/* LCDˢ��ʱʹ�õ���ɫ */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/**
 * @brief       uC/OS-III������ں���
 * @param       ��
 * @retval      ��
 */
void uc_os3_demo(void)
{
    OS_ERR err;
    
    lcd_show_string(10, 10, 220, 32, 32, "STM32", RED);
    lcd_show_string(10, 47, 220, 24, 24, "uC/OS-III Flag", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_draw_rectangle(5, 130, 234, 314, BLACK);         /* ������ */
    lcd_show_string(59, 110, 220, 16, 16, "Flags Value: 0", BLUE);
    
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
    
    /* �����¼���־ */
    OSFlagCreate(   (OS_FLAG_GRP   *)&flag,
                    (CPU_CHAR      *)"flag",
                    (OS_FLAGS       )0,
                    (OS_ERR        *)&err);
    
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
    uint8_t key;
    OS_ERR err;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                OSFlagPost( (OS_FLAG_GRP   *)&flag,
                            (OS_FLAGS       )FLAGBIT_0,
                            (OS_OPT         )OS_OPT_POST_FLAG_SET,
                            (OS_ERR        *)&err);
                break;
            }
            case KEY1_PRES:
            {
                OSFlagPost( (OS_FLAG_GRP   *)&flag,
                            (OS_FLAGS       )FLAGBIT_1,
                            (OS_OPT         )OS_OPT_POST_FLAG_SET,
                            (OS_ERR        *)&err);
                break;
            }
            default:
            {
                break;
            }
        }
        
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
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
    CPU_SR_ALLOC();
    
    while (1)
    {
        OSFlagPend( (OS_FLAG_GRP   *)&flag,
                    (OS_FLAGS       )FLAGBIT_ALL,
                    (OS_TICK        )0,
                    (OS_OPT         )OS_OPT_PEND_FLAG_SET_ALL |
                                     OS_OPT_PEND_FLAG_CONSUME |
                                     OS_OPT_PEND_BLOCKING,
                    (CPU_TS        *)0,
                    (OS_ERR        *)&err);
        
        CPU_CRITICAL_ENTER();
        lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);
        CPU_CRITICAL_EXIT();
    }
}

/**
 * @brief       task3
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *p_arg)
{
    OS_FLAGS flags;
    OS_ERR err;
    CPU_SR_ALLOC();
    
    while (1)
    {
        flags = flag.Flags;
        
        CPU_CRITICAL_ENTER();
        lcd_show_xnum(163, 110, flags, 1, 16, 0, BLUE);
        CPU_CRITICAL_EXIT();
        
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}
