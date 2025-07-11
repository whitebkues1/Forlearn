/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       单通道ADC采集 实验
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
#include "./BSP/ADC/adc.h"


int main(void)
{
    uint16_t adcx;
    float temp;

    sys_stm32_clock_init(9);    /* 设置时钟, 72Mhz */
    delay_init(72);             /* 延时初始化 */
    usart_init(72, 115200);     /* 串口初始化为115200 */
    led_init();                 /* 初始化LED */
    lcd_init();                 /* 初始化LCD */
    adc_init();                 /* 初始化ADC */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "ADC TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
    lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE); /* 先在固定位置显示小数点 */

    while (1)
    {
        adcx = adc_get_result_average(ADC_ADCX_CHY, 10);/* 获取通道5的转换值，10次取平均 */
        lcd_show_xnum(134, 110, adcx, 5, 16, 0, BLUE);  /* 显示ADCC采样后的原始值 */
 
        temp = (float)adcx * (3.3 / 4096);              /* 获取计算后的带小数的实际电压值，比如3.1111 */
        adcx = temp;                                    /* 赋值整数部分给adcx变量，因为adcx为u16整形 */
        lcd_show_xnum(134, 130, adcx, 1, 16, 0, BLUE);  /* 显示电压值的整数部分，3.1111的话，这里就是显示3 */

        temp -= adcx;                                   /* 把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111 */
        temp *= 1000;                                   /* 小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。 */
        lcd_show_xnum(150, 130, temp, 3, 16, 0X80, BLUE);/* 显示小数部分（前面转换为了整形显示），这里显示的就是111. */

        LED0_TOGGLE();
        delay_ms(100);
    }
}


























