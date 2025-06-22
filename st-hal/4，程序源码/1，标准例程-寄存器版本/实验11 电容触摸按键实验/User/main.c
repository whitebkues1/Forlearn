/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-21
 * @brief       ���ݴ������� ʵ��
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
#include "./BSP/TPAD/tpad.h"


int main(void)
{
    uint8_t t = 0;

    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */
    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                 /* ��ʼ��LED */
    tpad_init(6);               /* ��ʼ���������� */

    while (1)
    {
        if (tpad_scan(0))       /* �ɹ�������һ��������(�˺���ִ��ʱ������15ms) */
        {
            LED1_TOGGLE();      /* LED1ȡ�� */
        }

        t++;

        if (t == 15)
        {
            t = 0;
            LED0_TOGGLE();      /* LED0ȡ�� */
        }

        delay_ms(10);
    }
}
















