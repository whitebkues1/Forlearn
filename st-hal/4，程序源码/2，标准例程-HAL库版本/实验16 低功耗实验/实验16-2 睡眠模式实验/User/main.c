/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       睡眠模式 实验
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
#include "./BSP/KEY/key.h"
#include "./BSP/PWR/pwr.h"


int main(void)
{
    uint8_t t = 0;
    uint8_t key = 0;

    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    pwr_wkup_key_init();                /* 唤醒按键初始化 */

    lcd_show_string(30, 50, 200, 16, 16,  "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16,  "SLEEP TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16,  "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Enter SLEEP MODE", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:Exit SLEEP MODE", RED);

    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)
        {
            LED1(0);                    /* 点亮绿灯,提示进入睡眠模式 */

            pwr_enter_sleep();          /* 进入睡眠模式 */

            LED1(1);                    /* 关闭绿灯,提示退出睡眠模式 */
        }

        if ((t % 20) == 0)
        {
            LED0_TOGGLE();              /* 每200ms,翻转一次LED0 */
        }

        delay_ms(10);
        t++;
    }
}




