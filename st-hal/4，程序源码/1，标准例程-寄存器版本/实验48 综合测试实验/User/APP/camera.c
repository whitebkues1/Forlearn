/**
 ****************************************************************************************************
 * @file        camera.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-31
 * @brief       APP-����� ����
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

#include "camera.h"
#include "calendar.h"
#include "common.h"
#include "./BSP/OV7725/ov7725.h"
#include "./BSP/EXTI/exti.h"


extern volatile uint8_t framecnt;       /* ��timer.c�� �涨�� */
extern volatile uint8_t g_ov7725_vsta;  /* ��exit.c�� �涨�� */

/* 
 * ������OV7725ģ���FIFO���������޷�����һ֡640*480�Ļ�����������QVGA��VGAģʽ������
 * 320*240�ķֱ���,VGAģʽ��֧�����ţ����Կ���ʹ�ø�С�ķֱ�������ʾ�����߿��Ը�����Ҫ�����޸�
 */
uint16_t g_ov7725_wwidth  = 320;    /* Ĭ�ϴ��ڿ��Ϊ320 */
uint16_t g_ov7725_wheight = 240;    /* Ĭ�ϴ��ڸ߶�Ϊ240 */


/* ����ͷ��ʾ */
uint8_t *const camera_remind_tbl[3][GUI_LANGUAGE_NUM] =
{
    {"��ʼ��OV7725,���Ժ�...", "��ʼ��OV7725,Ո����...", "OV7725 Init,Please wait...",},
    {"δ��⵽OV7725,����...", "δ�z�y��OV7725,Ո�z��...", "No OV7725 find,Please check...",},
    {"����Ϊ:", "�����:", "SAVE AS:",},
};
/* ���ճɹ���ʾ����� */
uint8_t *const camera_saveok_caption[GUI_LANGUAGE_NUM] =
{
    "���ճɹ���", "���ճɹ���", "Take Photo OK��",
};

/* 00������ѡ������ */
uint8_t *const camera_l00fun_caption[GUI_LANGUAGE_NUM] =
{
    "�������", "���C�O��", "Camera Set",
};

/* 10������ѡ��� */
uint8_t *const camera_l00fun_table[GUI_LANGUAGE_NUM][5] =
{
    {"��������", "��Ч����", "��������", "ɫ������", "�Աȶ�����",},
    {"�����O��", "��Ч�O��", "�����O��", "ɫ���O��", "���ȶ��O��",},
    {"Scene", "Effects", "Brightness", "Saturation", "Contrast"},
};

/* 11������ѡ���, �ƹ�ģʽ */
uint8_t *const camera_l11fun_table[GUI_LANGUAGE_NUM][6] =
{
    {"�Զ�", "����", "����", "�칫��", "��ͥ", "ҹ��",},
    {"�Ԅ�", "����", "���", "�k����", "��ͥ", "ҹ��",},
    {"Auto", "Sunny", "Cloudy", "Office", "Home", "Night"},
};

/* 12������ѡ���, ��Ч���� */
uint8_t *const camera_l12fun_table[GUI_LANGUAGE_NUM][7] =
{
    {"��ͨ", "��Ƭ", "�ڰ�", "ƫ��ɫ", "ƫ��ɫ", "ƫ��ɫ", "����"},
    {"��ͨ", "ؓƬ", "�ڰ�", "ƫ�tɫ", "ƫ�Gɫ", "ƫ�{ɫ", "�͹�"},
    {"Normal", "Negative", "B&W", "Redish", "Greenish", "Bluish", "Antique",},
};

/* 13~15������ѡ���, ����-3~3��7��ֵ */
uint8_t *const camera_l135fun_table[GUI_LANGUAGE_NUM][7] =
{
    {"-3", "-2", "-1", "0", "+1", "+2", "+3"},
    {"-3", "-2", "-1", "0", "+1", "+2", "+3"},
    {"-3", "-2", "-1", "0", "+1", "+2", "+3"},
};

/**
 * @brief       ����LCD��ʾ
 * @param       ��
 * @retval      ��
 */
void camera_refresh(void)
{
    uint32_t i, j;
    uint16_t color;
 
    if (g_ov7725_vsta)                  /* ��֡�жϸ��� */
    {
        lcd_scan_dir(U2D_L2R);          /* ���ϵ���, ������ */
        lcd_set_window((lcddev.width - g_ov7725_wwidth) / 2, (lcddev.height - g_ov7725_wheight) / 2,
                        g_ov7725_wwidth, g_ov7725_wheight);     /* ����ʾ�������õ���Ļ���� */

        lcd_write_ram_prepare();        /* ��ʼд��GRAM */

        OV7725_RRST(0);                 /* ��ʼ��λ��ָ�� */
        OV7725_RCLK(0);
        OV7725_RCLK(1);
        OV7725_RCLK(0);
        OV7725_RRST(1);                 /* ��λ��ָ����� */
        OV7725_RCLK(1);

        for (i = 0; i < g_ov7725_wheight; i++)
        {
            for (j = 0; j < g_ov7725_wwidth; j++)
            {
                OV7725_RCLK(0);
                color = OV7725_DATA;    /* ������ */
                OV7725_RCLK(1);
                color <<= 8;
                OV7725_RCLK(0);
                color |= OV7725_DATA;   /* ������ */
                OV7725_RCLK(1);
                LCD->LCD_RAM = color;
            }
        }

        g_ov7725_vsta = 0;              /* ����֡�жϱ�� */
        framecnt++;
        lcd_scan_dir(DFT_SCAN_DIR);     /* �ָ�Ĭ��ɨ�跽�� */
    }
}

/**
 * @brief       ͨ��ʱ���ȡ�ļ���
 *  @note       ��ϳ�:����"0:PHOTO/PIC20120321210633.bmp"/"2:PHOTO/PIC20120321210633.bmp"���ļ���
 * @param       pname           : ��·��������
 * @retval      ��
 */
void camera_new_pathname(uint8_t *pname)
{
    calendar_get_time(&calendar);
    calendar_get_date(&calendar);

    if (gui_phy.memdevflag & (1 << 0))sprintf((char *)pname, "0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);       /* ��ѡ������SD�� */
    else if (gui_phy.memdevflag & (1 << 2))sprintf((char *)pname, "2:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);  /* SD��������,�򱣴���U�� */
}

/**
 * @brief       ����� ����
 *  @note       ������Ƭ�ļ�,��������SD��PHOTO�ļ�����.
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t camera_play(void)
{
    uint8_t rval = 0;
    uint8_t res;
    uint8_t *caption = 0;
    uint8_t *pname;
    uint8_t selx = 0;
    uint8_t l00sel = 0, l11sel = 0, l12sel = 0; /* Ĭ��ѡ���� */
    uint8_t l345sel[3];
    uint8_t *psn;

    /* ��ʾ��ʼ���OV7725 */
    window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_remind_tbl[0][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);

    if (gui_phy.memdevflag & (1 << 0))f_mkdir("0:PHOTO"); /* ǿ�ƴ����ļ���,��������� */

    if (ov7725_init())  /* ��ʼ��OV7725 */
    {
        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, (uint8_t *)camera_remind_tbl[1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);
        rval = 1;
    }
    
    delay_ms(1500);

    pname = gui_memin_malloc(40);   /* ����40���ֽ��ڴ�,���ڴ���ļ��� */
    psn = gui_memin_malloc(50);     /* ����50���ֽ��ڴ�,���ڴ�����ƣ�������Ϊ:0:PHOTO/PIC20120321210633.bmp"������ʾ�� */

    if (pname == NULL)rval = 1;     /* ����ʧ�� */

    if (rval == 0)  /* OV7725����\�����ڴ�ɹ� */
    {
        l345sel[0] = 2;             /* ����Ĭ��Ϊ2,ʵ��ֵ0 */
        l345sel[1] = 2;             /* ɫ��Ĭ��Ϊ2,ʵ��ֵ0 */
        l345sel[2] = 2;             /* �Աȶ�Ĭ��Ϊ2,ʵ��ֵ0 */

        ov7725_window_set(g_ov7725_wwidth, g_ov7725_wheight, 0);    /* QVGAģʽ��� */

        OV7725_OE(0);               /* ʹ��OV7725 FIFO������� */

        lcd_clear(BLACK);
        exti_ov7725_vsync_init();   /* ʹ��OV7725 VSYNC�ⲿ�ж�, ����֡�ж� */
        
        tim3_int_init(10000 - 1, 7200 - 1); /* ����TIM3 ,1s�ж�һ�� */

        while (1)
        {
            tp_dev.scan(0);

            if (tp_dev.sta & TP_PRES_DOWN)
            {
                caption = (uint8_t *)camera_l00fun_caption[gui_phy.language];
                res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 5) / 2, 160, 72 + 32 * 5, (uint8_t **)camera_l00fun_table[gui_phy.language], 5, (uint8_t *)&l00sel, 0X90, caption); /* ��ѡ */

                if (res == 0)
                {
                    caption = (uint8_t *)camera_l00fun_table[gui_phy.language][l00sel];
                    camera_refresh();   /* ˢ��һ�� */

                    switch (l00sel)
                    {
                        case 0:/* �ƹ�ģʽ���� */
                            res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 6) / 2, 160, 72 + 32 * 6, (uint8_t **)camera_l11fun_table[gui_phy.language], 6, (uint8_t *)&l11sel, 0X90, caption); /* ��ѡ */

                            if (res == 0)
                            {
                                ov7725_light_mode(l11sel);  /* �ƹ�ģʽ���� */
                            }

                            break;

                        case 1:/* ��Ч���� */
                            res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 7) / 2, 160, 72 + 32 * 7, (uint8_t **)camera_l12fun_table[gui_phy.language], 7, (uint8_t *)&l12sel, 0X90, caption); /* ��ѡ */

                            if (res == 0)
                            {
                                ov7725_special_effects(l12sel); /* ��Ч���� */
                            }

                            break;

                        case 2:/* ����,ɫ��,�Աȶ����� */
                        case 3:/* ����,ɫ��,�Աȶ����� */
                        case 4:/* ����,ɫ��,�Աȶ����� */
                            selx = l345sel[l00sel - 2]; /* �õ�֮ǰ��ѡ�� */
                            res = app_items_sel((lcddev.width - 160) / 2, (lcddev.height - 72 - 32 * 7) / 2, 160, 72 + 32 * 7, (uint8_t **)camera_l135fun_table[gui_phy.language], 7, (uint8_t *)&selx, 0X90, caption); /* ��ѡ */

                            if (res == 0)
                            {
                                l345sel[l00sel - 2] = selx; /* ��¼��ֵ */

                                if (l00sel == 2)ov7725_brightness(selx);        /* �������� */

                                if (l00sel == 3)ov7725_color_saturation(selx);  /* ɫ������ */

                                if (l00sel == 4)ov7725_contrast(selx);          /* �Աȶ����� */
                            }

                            break;
                    }
                }

                lcd_clear(BLACK);
            }

            if (system_task_return)break; /* TPAD���� */

            camera_refresh();

            if (key_scan(0) == WKUP_PRES) /* KEY_UP������һ��,����һ�� */
            {
                LED1(0);            /* DS1��,��ʾ���ڱ��� */
                pname[0] = '\0';    /* ��ӽ����� */
                
                camera_new_pathname(pname);

                if (pname != NULL)
                {
                    if (bmp_encode(pname, (lcddev.width - 240) / 2, (lcddev.height - 320) / 2, 240, 320, 0)) /* �������� */
                    {
                        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)APP_CREAT_ERR_MSG_TBL[gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* ��ʾSD���Ƿ���� */
                        delay_ms(2000);/* �ȴ�2���� */
                    }
                    else
                    {
                        strcpy((char *)psn, (const char *)camera_remind_tbl[2][gui_phy.language]);
                        strcat((char *)psn, (const char *)pname);
                        window_msg_box((lcddev.width - 180) / 2, (lcddev.height - 80) / 2, 180, 80, psn, (uint8_t *)camera_saveok_caption[gui_phy.language], 12, 0, 0, 0);
                        BEEP(1);            /* �������̽У���ʾ������� */
                        delay_ms(100);
                    }

                    system_task_return = 0; /* ���TPAD */
                }

                LED1(1);    /* DS1��,��ʾ������� */
                BEEP(0);    /* �������̽� */
                delay_ms(1800);
            }
        }
    }

    gui_memin_free(pname);
    gui_memin_free(psn);
    
    TIM3->CR1 &= ~(1 << 0); /* �رն�ʱ��3 */
    EXTI->IMR &= ~(1 << 8); /* �ر�line8�ж� */
    
    return 0;
}















