/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ���ڿ��Ź� ʵ��
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


int main(void)
{
    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */
    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                 /* ��ʼ��LED */
    LED0(0);                    /* ����LED0 ��� */
    delay_ms(300);              /* ��ʱ300ms�ٳ�ʼ�����Ź�,LEDR�ı仯"�ɼ�" */
    wwdg_init(0X7F, 0X5F, 3);   /* ������ֵΪ7f,���ڼĴ���Ϊ5f,��Ƶ��Ϊ8 */

    while (1)
    {
        LED0(1);                /* �رպ�� */
    }
}















