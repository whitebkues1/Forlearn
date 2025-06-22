/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-19
 * @brief       �������� ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"


int main(void)
{
    uint8_t key;

    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��, 72Mhz */
    delay_init(72);                         /* ��ʱ��ʼ�� */
    led_init();                             /* ��ʼ��LED */
    beep_init();                            /* ��ʼ�������� */
    key_init();                             /* ��ʼ������ */
    LED0(0);                                /* �ȵ���LED0 */
    
    while(1)
    {
        key = key_scan(0);                  /* �õ���ֵ */

        if (key)
        {
            switch (key)
            {
                case WKUP_PRES:             /* ���Ʒ����� */
                    BEEP_TOGGLE();          /* BEEP״̬ȡ�� */
                    break;

                case KEY1_PRES:             /* ����LED1(GREEN)��ת */
                    LED1_TOGGLE();          /* LED1״̬ȡ�� */
                    break;

                case KEY0_PRES:             /* ͬʱ����LED0, LED1��ת */
                    LED0_TOGGLE();          /* LED0״̬ȡ�� */
                    LED1_TOGGLE();          /* LED1״̬ȡ�� */
                    break;
            } 
        }
        else
        {
            delay_ms(10);
        }
    }
}

