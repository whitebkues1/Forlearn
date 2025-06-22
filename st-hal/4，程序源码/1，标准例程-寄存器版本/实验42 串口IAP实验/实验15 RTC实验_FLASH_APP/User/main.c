/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-12
 * @brief       RTC ʵ��-FLASH APP�汾
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
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/RTC/rtc.h"


int main(void)
{
    uint8_t tbuf[40];
    uint8_t t=0;  

    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */

    /* �����ж�������ƫ����Ϊ0X9000 */
    sys_nvic_set_vector_table(FLASH_BASE,0x9000);

    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    usmart_dev.init(72);        /* ��ʼ��USMART */
    led_init();                 /* ��ʼ��LED */
    lcd_init();                 /* ��ʼ��LCD */
    rtc_init();                 /* ��ʼ��RTC */

    lcd_show_string(30, 50, 200, 16, 16, "STM32F103", RED);
    lcd_show_string(30, 70, 200, 16, 16, "RTC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "2020/5/12", RED);

    while (1)
    {
        t++;

        if ((t % 10) == 0)  /* ÿ100ms����һ����ʾ���� */
        {
            rtc_get_time();
            sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
            lcd_show_string(30, 140, 210, 16, 16, (char*)tbuf, RED);
            sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
            lcd_show_string(30, 160, 210, 16, 16, (char*)tbuf, RED);
            sprintf((char *)tbuf, "Week:%d", calendar.week);
            lcd_show_string(30, 180, 210, 16, 16, (char*)tbuf, RED);
        }

        if ((t % 20) == 0)
        {
            LED0_TOGGLE();  /* ÿ200ms,��תһ��LED0 */
        }
        
        delay_ms(10);
    }
}


















