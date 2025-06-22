/**
 ****************************************************************************************************
 * @file        uc-os3_demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-13
 * @brief       uC/OS-III �ڴ����ʵ��
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

OS_MEM mem;
#define MEM_BLOCK_CNT   5
#define MEM_BLOCK_SIZE  100
uint8_t memory[MEM_BLOCK_CNT][MEM_BLOCK_SIZE];

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
    lcd_show_string(10, 47, 220, 24, 24, "Mem Manage", RED);
    lcd_show_string(10, 76, 220, 16, 16, "ATOM@ALIENTEK", RED);
    
    lcd_show_string(30, 118, 200, 16, 16, "Total Block Count:", RED);
    lcd_show_string(30, 139, 200, 16, 16, "Free  Block Count:", RED);
    lcd_show_string(30, 160, 200, 16, 16, "Get Block Addr:", RED);
    
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
    
    /* ������uC/OS-III������ڴ� */
    OSMemCreate((OS_MEM *)&mem,
                (CPU_CHAR *)"mem",
                (void *)memory,
                (OS_MEM_QTY)MEM_BLOCK_CNT,
                (OS_MEM_SIZE)MEM_BLOCK_SIZE,
                (OS_ERR *)&err);
    
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
    uint8_t *buf = NULL;
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                buf = OSMemGet( (OS_MEM *)&mem,
                                (OS_ERR *)&err);
                if (buf != NULL)
                {
                    sprintf((char *)buf, "0x%p", buf);
                    lcd_show_string(158, 160, 200, 16, 16, (char *)buf, BLUE);
                }
                else
                {
                    lcd_show_string(158, 160, 200, 16, 16, "          ", RED);
                    lcd_show_string(158, 160, 200, 16, 16, "Failed!", RED);
                }
                break;
            }
            case KEY1_PRES:
            {
                if (buf != NULL)
                {
                    OSMemPut(   (OS_MEM *)&mem,
                                (void *)buf,
                                (OS_ERR *)&err);
                    buf = NULL;
                }
                break;
            }
            default:
            {
                break;
            }
        }
        
        lcd_show_xnum(182, 118, mem.NbrMax, 1, 16, 0, BLUE);
        lcd_show_xnum(182, 139, mem.NbrFree, 1, 16, 0, BLUE);
        
        OSTimeDly(10, OS_OPT_TIME_DLY, &err);
    }
}
