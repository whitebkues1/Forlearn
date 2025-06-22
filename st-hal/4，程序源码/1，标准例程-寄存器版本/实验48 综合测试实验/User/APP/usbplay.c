/**
 ****************************************************************************************************
 * @file        usbplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-31
 * @brief       APP-USB���� ����
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.1 20221031
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "usbplay.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"
#include "./BSP/SDIO/sdio_sdcard.h"



USBD_HandleTypeDef * USBD_Device;           /* USB Device����ṹ�� */

extern volatile uint8_t g_usb_state_reg;    /* USB״̬ */ 


/**
 * @brief       USB����
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t usb_play(void)
{
    uint8_t rval = 0;           //����ֵ
    uint8_t offline_cnt = 0;
    uint8_t tct = 0;
    
    uint8_t USB_STA = 0;
    uint8_t busycnt = 0;    /* USBæ������ */
    uint8_t errcnt = 0;     /* USB��������� */
    
    
    g_back_color = LGRAY;
    lcd_clear(g_back_color);

    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[8][gui_phy.language], 0X05); //��ʾ����
    
    lcd_show_string(60 + (lcddev.width - 240) / 2, 130 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB DisConnected ", BLUE); /* ������ʾUSB������ */

    usbd_port_config(0);    /* USB�ȶϿ� */
    delay_ms(500);
    usbd_port_config(1);    /* USB�ٴ����� */
    delay_ms(500);

    USBD_Device = gui_memin_malloc(sizeof(USBD_HandleTypeDef)); /* �����ڴ� */
    
    if(USBD_Device == NULL)
    {
        return USBD_FAIL;
    }
    
    gui_memset(USBD_Device, 0, sizeof(USBD_HandleTypeDef));     /* �������� */

    USBD_Init(USBD_Device, &MSC_Desc, 0);                       /* ��ʼ��USB */
    USBD_RegisterClass(USBD_Device, USBD_MSC_CLASS);            /* ����� */
    USBD_MSC_RegisterStorage(USBD_Device, &USBD_DISK_fops);     /* ΪMSC����ӻص����� */
    USBD_Start(USBD_Device);                                    /* ����USB */

    while (1)
    {
        delay_ms(1000 / OS_TICKS_PER_SEC);  /* ��ʱһ��ʱ�ӽ��� */

        if (system_task_return)
        {
            if (busycnt) /* USB���ڶ�д */
            {
                lcd_show_string(60 + (lcddev.width - 240) / 2, 110 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB BUSY!!!", RED); /* ��ʾUSB��æ */
                system_task_return = 0;     /* ȡ�� */
            }
            else break; /* USB����,���˳�USB */
        }

        tct++;

        if (tct == 40) /* ÿ40ms����һ�� */
        {
            tct = 0;

            if (busycnt)busycnt--;
            else gui_fill_rectangle(60 + (lcddev.width - 240) / 2, 110 + (lcddev.height - 320) / 2, 100, 16, g_back_color); /* �����ʾ */

            if (errcnt)errcnt--;
            else gui_fill_rectangle(60 + (lcddev.width - 240) / 2, 170 + (lcddev.height - 320) / 2, 128, 16, g_back_color); /* �����ʾ */

            if (g_usb_state_reg & 0x10)         /* ����ѯ���� */
            {
                offline_cnt = 0;                /* USB������,�����offline������ */
                g_usb_state_reg |= 0X80;        /* ���USB�������� */
                g_usb_state_reg &= ~(1 << 4);   /* �����ѯ��־λ */
            }
            else
            {
                offline_cnt++;

                if (offline_cnt > 50)g_usb_state_reg = 0; /* 2s��û�յ����߱��,����USB���γ��� */
            }
        }

        if (USB_STA != g_usb_state_reg) /* ״̬�ı��� */
        {
            gui_fill_rectangle(60 + (lcddev.width - 240) / 2, 150 + (lcddev.height - 320) / 2, 120, 16, g_back_color); /* �����ʾ */

            if (g_usb_state_reg & 0x01) /* ����д */
            {
                if (busycnt < 5)busycnt++;

                lcd_show_string(60 + (lcddev.width - 240) / 2, 150 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB Writing...", BLUE); /* ��ʾUSB����д������ */
            }

            if (g_usb_state_reg & 0x02) /* ���ڶ� */
            {
                if (busycnt < 5)busycnt++;

                lcd_show_string(60 + (lcddev.width - 240) / 2, 150 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB Reading...", BLUE); /* ��ʾUSB���ڶ������� */
            }

            if (g_usb_state_reg & 0x04)
            {
                if (errcnt < 5)errcnt++;

                lcd_show_string(60 + (lcddev.width - 240) / 2, 170 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB Write Error", RED); /* ��ʾд����� */
            }

            if (g_usb_state_reg & 0x08)
            {
                if (errcnt < 5)errcnt++;

                lcd_show_string(60 + (lcddev.width - 240) / 2, 170 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB Read  Error", RED); /* ��ʾ�������� */
            }
            
            if (g_usb_state_reg & 0X80)lcd_show_string(60 + (lcddev.width - 240) / 2, 130 + (lcddev.height - 320) / 2, lcddev.width, lcddev.height, 16, "USB Connected   ", BLUE);    /* ��ʾUSB�����Ѿ����� */
            else lcd_show_string(60 + (lcddev.width - 240) / 2, 130 + (lcddev.height - 320) / 2, lcddev.width, 320, 16, "USB DisConnected", BLUE);  /* ��ʾUSB���γ��� */

            g_usb_state_reg &= 0X90;      /* ���������״̬&��ѯ��־�����������λ */
            USB_STA = g_usb_state_reg;    /* ��¼����״̬ */
        }
    }

    usbd_port_config(0);        /* �ر� USB */
    
    USBD_Stop(USBD_Device);     /* ֹͣUSB */
    
    gui_memin_free(USBD_Device);
    
    return rval;
}







