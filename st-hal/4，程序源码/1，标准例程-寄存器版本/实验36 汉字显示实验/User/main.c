/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       汉字显示 实验
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
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/NORFLASH/norflash.h"


int main(void)
{
    uint32_t fontcnt;
    uint8_t i, j;
    uint8_t fontx[2];           /* GBK码 */
    uint8_t key, t;

    sys_stm32_clock_init(9);    /* 设置时钟, 72Mhz */
    delay_init(72);             /* 延时初始化 */
    usart_init(72, 115200);     /* 串口初始化为115200 */
    usmart_dev.init(72);        /* 初始化USMART */
    led_init();                 /* 初始化LED */
    lcd_init();                 /* 初始化LCD */
    key_init();                 /* 初始化按键 */
    norflash_init();            /* 初始化NORFLASH */
    my_mem_init(SRAMIN);        /* 初始化内部SRAM内存池 */

    exfuns_init();              /* 为fatfs相关变量申请内存 */
    f_mount(fs[0], "0:", 1);    /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);    /* 挂载FLASH */

    while (fonts_init())        /* 检查字库 */
    {
UPD:
        lcd_clear(WHITE);       /* 清屏 */
        lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);

        while (sd_init())       /* 检测SD卡 */
        {
            lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
            delay_ms(200);
            lcd_fill(30, 50, 200 + 30, 50 + 16, WHITE);
            delay_ms(200);
        }

        lcd_show_string(30, 50, 200, 16, 16, "SD Card OK", RED);
        lcd_show_string(30, 70, 200, 16, 16, "Font Updating...", RED);
        key = fonts_update_font(20, 90, 16, (uint8_t *)"0:", RED);  /* 更新字库 */

        while (key)             /* 更新失败 */
        {
            lcd_show_string(30, 90, 200, 16, 16, "Font Update Failed!", RED);
            delay_ms(200);
            lcd_fill(20, 90, 200 + 20, 90 + 16, WHITE);
            delay_ms(200);
        }

        lcd_show_string(30, 90, 200, 16, 16, "Font Update Success!   ", RED);
        delay_ms(1500);
        lcd_clear(WHITE);/* 清屏 */
    }
 
    text_show_string(30, 30, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    text_show_string(30, 50, 200, 16, "GBK字库测试程序", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "按KEY0,更新字库", 16, 0, RED);
    
    text_show_string(30, 110, 200, 16, "内码高字节:", 16, 0, BLUE);
    text_show_string(30, 130, 200, 16, "内码低字节:", 16, 0, BLUE);
    text_show_string(30, 150, 200, 16, "汉字计数器:", 16, 0, BLUE);
    
    text_show_string(30, 180, 200, 24, "对应汉字为:", 24, 0, BLUE);
    text_show_string(30, 204, 200, 16, "对应汉字(16*16)为:", 16, 0, BLUE);
    text_show_string(30, 220, 200, 16, "对应汉字(12*12)为:", 12, 0, BLUE);

    while (1)
    {
        fontcnt = 0;

        for (i = 0x81; i < 0xff; i++)       /* GBK内码高字节范围为0X81~0XFE */
        {
            fontx[0] = i;
            lcd_show_num(118, 130, i, 3, 16, BLUE);             /* 显示内码高字节 */

            for (j = 0x40; j < 0xfe; j++)   /* GBK内码低字节范围为 0X40~0X7E, 0X80~0XFE) */
            {
                if (j == 0x7f)continue;

                fontcnt++;
                lcd_show_num(118, 130, j, 3, 16, BLUE);         /* 显示内码低字节 */
                lcd_show_num(118, 150, fontcnt, 5, 16, BLUE);   /* 汉字计数显示 */
                fontx[1] = j;
                text_show_font(30 + 132, 180, fontx, 24, 0, BLUE);
                text_show_font(30 + 144, 204, fontx, 16, 0, BLUE);
                text_show_font(30 + 108, 220, fontx, 12, 0, BLUE);
                t = 200;

                while (t--)   /* 延时,同时扫描按键 */
                {
                    delay_ms(1);
                    key = key_scan(0);

                    if (key == KEY0_PRES)
                    {
                        goto UPD;   /* 跳转到UPD位置(强制更新字库) */
                    }
                }

                LED0_TOGGLE();
            }
        }
    }
}









