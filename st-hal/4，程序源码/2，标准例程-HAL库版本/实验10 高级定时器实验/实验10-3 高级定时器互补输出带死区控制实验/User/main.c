/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-21
 * @brief       高级定时器互补输出带死区控制 实验
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
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/atim.h"


int main(void)
{
    uint8_t t = 0;

    HAL_Init();                                /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);        /* 设置时钟, 72Mhz */
    delay_init(72);                            /* 延时初始化 */
    usart_init(115200);                        /* 串口初始化为115200 */
    led_init();                                /* 初始化LED */
    atim_timx_cplm_pwm_init(1000 - 1, 72 - 1); /* 1Mhz的计数频率 1Khz的周期. */
    atim_timx_cplm_pwm_set(300, 100);          /* 占空比:7:3, 死区时间 100 * tDTS */

    while (1)
    {
        delay_ms(10);
        t++;

        if (t >= 20)
        {
            LED0_TOGGLE(); /* LED0(RED)闪烁 */
            t = 0;
        }
    }
}





