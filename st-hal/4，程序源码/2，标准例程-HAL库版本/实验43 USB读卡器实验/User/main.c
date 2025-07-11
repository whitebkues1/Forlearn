/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-13
 * @brief       USB读卡器 实验
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
#include "./MALLOC/malloc.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/NORFLASH/norflash.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"


USBD_HandleTypeDef USBD_Device;             /* USB Device处理结构体 */
extern volatile uint8_t g_usb_state_reg;    /* USB状态 */
extern volatile uint8_t g_device_state;     /* USB连接 情况 */

int main(void)
{
    uint8_t offline_cnt = 0;
    uint8_t tct = 0;
    uint8_t usb_sta;
    uint8_t device_sta;
    uint16_t id;
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟, 72Mhz */
    delay_init(72);                             /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    led_init();                 /* 初始化LED */
    lcd_init();                 /* 初始化LCD */
    key_init();                 /* 初始化按键 */
    norflash_init();            /* 初始化NOR FLASH */
    my_mem_init(SRAMIN);        /* 初始化内部SRAM内存池 */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB Card Reader TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    if (sd_init())  /* 初始化SD卡 */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);   /* 检测SD卡错误 */
    }
    else     /* SD 卡正常 */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Size:     MB", RED);
        lcd_show_num(134, 110, SD_TOTAL_SIZE_MB(&g_sdcard_handler), 5, 16, RED);  /* 显示SD卡容量 */
    }

    id = norflash_read_id();
    if ((id == 0) || (id == 0XFFFF))
    {
        lcd_show_string(30, 110, 200, 16, 16, "NorFlash Error!", RED);  /* 检测NorFlash错误 */
    }
    else   /* SPI FLASH 正常 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SPI FLASH Size:7.25MB", RED);
    }

    usbd_port_config(0);    /* USB先断开 */
    delay_ms(500);
    usbd_port_config(1);    /* USB再次连接 */
    delay_ms(500);

    lcd_show_string(30, 170, 200, 16, 16, "USB Connecting...", RED);    /* 提示正在建立连接 */
    USBD_Init(&USBD_Device, &MSC_Desc, 0);                              /* 初始化USB */
    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);                   /* 添加类 */
    USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);            /* 为MSC类添加回调函数 */
    USBD_Start(&USBD_Device);                                           /* 开启USB */
    delay_ms(1800);

    while (1)
    {
        delay_ms(1);

        if (usb_sta != g_usb_state_reg)   /* 状态改变了 */
        {
            lcd_fill(30, 190, 240, 210 + 16, WHITE); /* 清除显示 */

            if (g_usb_state_reg & 0x01)   /* 正在写 */
            {
                LED1(0);
                lcd_show_string(30, 190, 200, 16, 16, "USB Writing...", RED); /* 提示USB正在写入数据 */
            }

            if (g_usb_state_reg & 0x02)   /* 正在读 */
            {
                LED1(0);
                lcd_show_string(30, 190, 200, 16, 16, "USB Reading...", RED); /* 提示USB正在读出数据 */
            }

            if (g_usb_state_reg & 0x04)
            {
                lcd_show_string(30, 210, 200, 16, 16, "USB Write Err ", RED); /* 提示写入错误 */
            }
            else
            {
                lcd_fill(30, 210, 240, 230 + 16, WHITE); /* 清除显示 */
            }
            
            if (g_usb_state_reg & 0x08)
            {
                lcd_show_string(30, 230, 200, 16, 16, "USB Read  Err ", RED); /* 提示读出错误 */
            }
            else
            {
                lcd_fill(30, 230, 240, 250 + 16, WHITE); /* 清除显示 */
            }
            
            usb_sta = g_usb_state_reg; /* 记录最后的状态 */
        }

        if (device_sta != g_device_state)
        {
            if (g_device_state == 1)
            {
                lcd_show_string(30, 170, 200, 16, 16, "USB Connected    ", RED);    /* 提示USB连接已经建立 */
            }
            else
            {
                lcd_show_string(30, 170, 200, 16, 16, "USB DisConnected ", RED);    /* 提示USB被拔出了 */
            }
            
            device_sta = g_device_state;
        }

        tct++;

        if (tct == 200)
        {
            tct = 0;
            LED1(1);        /* 关闭 LED1 */
            LED0_TOGGLE();  /* LED0 闪烁 */

            if (g_usb_state_reg & 0x10)
            {
                offline_cnt = 0;    /* USB连接了,则清除offline计数器 */
                g_device_state = 1;
            }
            else    /* 没有得到轮询 */
            {
                offline_cnt++;

                if (offline_cnt > 100)
                {
                    g_device_state = 0;/* 20s内没收到在线标记,代表USB被拔出了 */
                }
            }

            g_usb_state_reg = 0;
        }
    }
}
















