/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-13
 * @brief       USB������ ʵ��
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
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/NORFLASH/norflash.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"


USBD_HandleTypeDef USBD_Device;             /* USB Device����ṹ�� */
extern volatile uint8_t g_usb_state_reg;    /* USB״̬ */
extern volatile uint8_t g_device_state;     /* USB���� ��� */

int main(void)
{
    uint8_t offline_cnt = 0;
    uint8_t tct = 0;
    uint8_t usb_sta;
    uint8_t device_sta;
    uint16_t id;

    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */
    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                 /* ��ʼ��LED */
    lcd_init();                 /* ��ʼ��LCD */
    key_init();                 /* ��ʼ������ */
    norflash_init();            /* ��ʼ��NOR FLASH */

    my_mem_init(SRAMIN);        /* ��ʼ���ڲ�SRAM�ڴ�� */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB Card Reader TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    if (sd_init())  /* ��ʼ��SD�� */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);   /* ���SD������ */
    }
    else     /* SD ������ */
    {
        lcd_show_string(30, 110, 200, 16, 16, "SD Card Size:     MB", RED);
        lcd_show_num(134, 110, g_sd_card_info.CardCapacity >> 20, 5, 16, RED);  /* ��ʾSD������ */
    }

    id = norflash_read_id();
    if ((id == 0) || (id == 0XFFFF))
    {
        lcd_show_string(30, 110, 200, 16, 16, "NorFlash Error!", RED);  /* ���NorFlash���� */
    }
    else   /* SPI FLASH ���� */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SPI FLASH Size:7.25MB", RED);
    }

    usbd_port_config(0);    /* USB�ȶϿ� */
    delay_ms(500);
    usbd_port_config(1);    /* USB�ٴ����� */
    delay_ms(500);

    lcd_show_string(30, 170, 200, 16, 16, "USB Connecting...", RED);    /* ��ʾ���ڽ������� */
    USBD_Init(&USBD_Device, &MSC_Desc, 0);                      /* ��ʼ��USB */
    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);           /* ����� */
    USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);    /* ΪMSC����ӻص����� */
    USBD_Start(&USBD_Device);                                   /* ����USB */
    delay_ms(1800);

    while (1)
    {
        delay_ms(1);

        if (usb_sta != g_usb_state_reg)   /* ״̬�ı��� */
        {
            lcd_fill(30, 190, 240, 210 + 16, WHITE); /* �����ʾ */

            if (g_usb_state_reg & 0x01)   /* ����д */
            {
                LED1(0);
                lcd_show_string(30, 190, 200, 16, 16, "USB Writing...", RED); /* ��ʾUSB����д������ */
            }

            if (g_usb_state_reg & 0x02)   /* ���ڶ� */
            {
                LED1(0);
                lcd_show_string(30, 190, 200, 16, 16, "USB Reading...", RED); /* ��ʾUSB���ڶ������� */
            }

            if (g_usb_state_reg & 0x04)
            {
                lcd_show_string(30, 210, 200, 16, 16, "USB Write Err ", RED); /* ��ʾд����� */
            }
            else
            {
                lcd_fill(30, 210, 240, 230 + 16, WHITE); /* �����ʾ */
            }
            
            if (g_usb_state_reg & 0x08)
            {
                lcd_show_string(30, 230, 200, 16, 16, "USB Read  Err ", RED); /* ��ʾ�������� */
            }
            else
            {
                lcd_fill(30, 230, 240, 250 + 16, WHITE); /* �����ʾ */
            }
            
            usb_sta = g_usb_state_reg; /* ��¼����״̬ */
        }

        if (device_sta != g_device_state)
        {
            if (g_device_state == 1)
            {
                lcd_show_string(30, 170, 200, 16, 16, "USB Connected    ", RED);    /* ��ʾUSB�����Ѿ����� */
            }
            else
            {
                lcd_show_string(30, 170, 200, 16, 16, "USB DisConnected ", RED);    /* ��ʾUSB���γ��� */
            }
            
            device_sta = g_device_state;
        }

        tct++;

        if (tct == 200)
        {
            tct = 0;
            LED1(1);        /* �ر� LED1 */
            LED0_TOGGLE();  /* LED0 ��˸ */

            if (g_usb_state_reg & 0x10)
            {
                offline_cnt = 0;    /* USB������,�����offline������ */
                g_device_state = 1;
            }
            else    /* û�еõ���ѯ */
            {
                offline_cnt++;

                if (offline_cnt > 100)
                {
                    g_device_state = 0;/* 20s��û�յ����߱��,����USB���γ��� */
                }
            }

            g_usb_state_reg = 0;
        }
    }
}




