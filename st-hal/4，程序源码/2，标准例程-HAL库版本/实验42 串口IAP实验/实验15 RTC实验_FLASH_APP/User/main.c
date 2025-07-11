/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-22
 * @brief       RTC 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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
    uint8_t t = 0;
    /* 设置中断向量表偏移量为0X10000 */
    sys_nvic_set_vector_table(FLASH_BASE,0x10000);

    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* 设置时钟, 72Mhz */
    delay_init(72);                         /* 延时初始化 */
    usart_init(115200);                     /* 串口初始化为115200 */
    usmart_dev.init(72);                    /* 初始化USMART */
    led_init();                             /* 初始化LED */
    lcd_init();                             /* 初始化LCD */
    rtc_init();                             /* 初始化RTC */

    rtc_set_alarm(2020, 4, 26, 9, 23, 45); /*设置一次闹钟*/

    lcd_show_string(30, 50, 200, 16, 16, "STM32F103", RED);
    lcd_show_string(30, 70, 200, 16, 16, "RTC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "2020/4/22", RED);

    while (1)
    {
        t++;

        if ((t % 10) == 0)                  /* 每100ms更新一次显示数据 */
        {
            rtc_get_time();
            sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
            lcd_show_string(30, 140, 210, 16, 16, (char *)tbuf, RED);
            sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
            lcd_show_string(30, 160, 210, 16, 16, (char *)tbuf, RED);
            sprintf((char *)tbuf, "Week:%d", calendar.week);
            lcd_show_string(30, 180, 210, 16, 16, (char *)tbuf, RED);
        }

        if ((t % 20) == 0)
        {
            LED0_TOGGLE();              /* 每200ms,翻转一次LED0 */
        }

        delay_ms(10);
    }
}
