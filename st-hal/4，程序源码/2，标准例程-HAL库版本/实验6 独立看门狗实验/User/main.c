/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-22
 * @brief       �������Ź� ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/WDG/wdg.h"
#include "./BSP/KEY/key.h"


int main(void)
{
    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��, 72Mhz */
    delay_init(72);                         /* ��ʱ��ʼ�� */
    usart_init(115200);                     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                             /* ��ʼ��LED */
    key_init();                             /* ��ʼ������ */
    delay_ms(100);                          /* ��ʱ100ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�" */
    iwdg_init(IWDG_PRESCALER_64, 625);      /* Ԥ��Ƶ��Ϊ64,����ֵΪ625,���ʱ��ԼΪ1s */
    LED0(0);                                /* ����LED0(���) */

    while (1)
    {
        if (key_scan(1) == WKUP_PRES)       /* ���WK_UP����,��ι�� */
        {
            iwdg_feed();                    /* ι�� */
        }

        delay_ms(10);
    }
}

