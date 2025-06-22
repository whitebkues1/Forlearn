/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ����ͨ�� ʵ��
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

#include "./stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"


int main(void)
{
    uint8_t len;
    uint16_t times = 0;

    HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��Ϊ72Mhz */
    delay_init(72);                         /* ��ʱ��ʼ�� */
    usart_init(115200);                     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                             /* ��ʼ��LED */
    while (1)
    {
        if (g_usart_rx_sta & 0x8000)        /* ���յ�������? */
        {
            len = g_usart_rx_sta & 0x3fff;  /* �õ��˴ν��յ������ݳ��� */
            printf("\r\n�����͵���ϢΪ:\r\n");

            HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_usart_rx_buf, len, 1000);    /* ���ͽ��յ������� */
            while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);           /* �ȴ����ͽ��� */
            printf("\r\n\r\n");             /* ���뻻�� */
            g_usart_rx_sta = 0;
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\r\n����ԭ�� STM32������ ����ʵ��\r\n");
                printf("����ԭ��@ALIENTEK\r\n\r\n\r\n");
            }

            if (times % 200 == 0) printf("����������,�Իس�������\r\n");

            if (times % 30  == 0) LED0_TOGGLE(); /* ��˸LED,��ʾϵͳ��������. */

            delay_ms(10);
        }
    }
}



