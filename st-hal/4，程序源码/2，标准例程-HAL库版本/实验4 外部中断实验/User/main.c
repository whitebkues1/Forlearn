/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       �ⲿ�ж� ʵ��
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
#include "./BSP/EXTI/exti.h"


int main(void)
{
    HAL_Init();                            /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);    /* ����ʱ��, 72Mhz */
    delay_init(72);                        /* ��ʱ��ʼ�� */
    usart_init(115200);                    /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                            /* ��ʼ��LED */
    beep_init();                           /* ��ʼ�������� */
    extix_init();                          /* ��ʼ���ⲿ�ж����� */
    LED0(0);                               /* �ȵ������ */

    while (1)
    {
        printf("OK\r\n");
        delay_ms(1000);
    }
}



