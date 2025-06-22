/**
 ****************************************************************************************************
 * @file        wirelessplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-����ͨ�Ų��� ����
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
 * V1.1 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "wirelessplay.h"
#include "paint.h"
#include "./BSP/NRF24L01/nrf24l01.h"
#include "./BSP/SPI/spi.h"



/* �ػ���ť������ */
uint8_t *const wireless_cbtn_caption_tbl[GUI_LANGUAGE_NUM] =
{
    "�ػ�", "�خ�", "CLEAR",
};

/* �������� */
uint8_t *const wireless_remind_msg_tbl[GUI_LANGUAGE_NUM] =
{
    "δ��⵽NRF24L01,����!", "δ�z�y��NRF24L01,Ո�z��!", "No NRF24L01,Please Check...",
};

/* ģʽѡ�� */
uint8_t *const wireless_mode_tbl[GUI_LANGUAGE_NUM][2] =
{
    {"����ģʽ", "����ģʽ",},
    {"�l��ģʽ", "����ģʽ",},
    {"TX Mode", "RX Mode",},
};

/**
 * @brief       ���ߴ���
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t wireless_play(void)
{
    _btn_obj *cbtn = 0;     /* �����ť�ؼ� */
    uint8_t res;
    uint8_t *caption;       /* ���� */
    uint8_t mode = 0;       /* 0,����ģʽ;1,����ģʽ */
    uint8_t tmp_buf[5];     /* buf[0~3]:����ֵ;buf[4]:0,������ͼ;1,����;2,�˳� */
    uint16_t x = 0, y = 0;
    uint16_t lastx = 0XFFFF, lasty = 0XFFFF;
    uint8_t rval = 0;

    /* ��ѡ��ģʽ */
    res = app_items_sel((lcddev.width - 180) / 2, (lcddev.height - 152) / 2, 180, 72 + 40 * 2, (uint8_t **)wireless_mode_tbl[gui_phy.language], 2, (uint8_t *)&mode, 0X90, (uint8_t *)APP_MODESEL_CAPTION_TBL[gui_phy.language]); /* 2��ѡ�� */

    if (res == 0) /* ȷ�ϼ�������,ͬʱ������������ */
    {
        caption = (uint8_t *)APP_MFUNS_CAPTION_TBL[10][gui_phy.language];   /* ���ߴ��� */
        gui_fill_rectangle(0, 0, lcddev.width, lcddev.height, LGRAY);       /* ��䱳��ɫ */
        nrf24l01_init();            /* ��ʼ��NRF24L01 */
        res = 0;

        while (nrf24l01_check())    /* ��ⲻ��24L01,�������5�� */
        {
            if (res == 0)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 70) / 2 - 15, 200, 70, (uint8_t *)wireless_remind_msg_tbl[gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);

            delay_ms(500);
            res++;

            if (res > 5)return 0; /* ��ʱδ��⵽,�˳� */
        }

        gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight, LGRAY); /* ��䱳��ɫ */

        if (mode == 0)nrf24l01_tx_mode();   /* ����ģʽ */
        else nrf24l01_rx_mode();

        caption = (uint8_t *)wireless_mode_tbl[gui_phy.language][mode]; /* ���� */
        app_filebrower(caption, 0X07);      /* ��ʾ���� */
        cbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbheight + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */

        if (cbtn == NULL)rval = 1;          /* û���㹻�ڴ湻���� */
        else
        {
            cbtn->caption = (uint8_t *)wireless_cbtn_caption_tbl[gui_phy.language]; /* �ػ� */
            cbtn->font = gui_phy.tbfsize;   /* �����µ������С */
            cbtn->bcfdcolor = WHITE;        /* ����ʱ����ɫ */
            cbtn->bcfucolor = WHITE;        /* �ɿ�ʱ����ɫ */

            if (mode == 0)btn_draw(cbtn);   /* ����ģʽ����Ҫ�ػ���ť */
        }

        tmp_buf[4] = tp_dev.touchtype & 0X80;   /**
                                                 * �õ�����������,tmp_buf[4]:
                                                 *  
                                                 *  b3~0:0,�����κδ���
                                                 *       1,����
                                                 *       2,����
                                                 *       3,�˳�
                                                 *  b4~6:����
                                                 *  b7:0,������;1,������
                                                 */
        
        while (rval == 0)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);	/* �õ�������ֵ */
            tmp_buf[4] &= 0X80; /* ���ԭ�������� */

            if (system_task_return) /* TPAD���� */
            {
                tmp_buf[4] |= 0X03; /* ����3,�˳� */

                if (mode == 0)      /* ����ģʽ,��Ҫ�����˳�ָ�� */
                {
                    nrf24l01_tx_packet(tmp_buf); /* ����ģʽ,�����˳�ָ�� */
                }
                else break; /* ����ģʽ,ֱ���˳� */
            }

            if (mode == 0) /* ����ģʽ */
            {
                res = btn_check(cbtn, &in_obj); /* ����ػ���ť */

                if (res) /* �ػ���ť����Ч���� */
                {
                    if (((cbtn->sta & 0X80) == 0))      /* ��ť״̬�ı��� */
                    {
                        tmp_buf[4] |= 0X02;             /* ����2,���� */
                        nrf24l01_tx_packet(tmp_buf);    /* �������ָ�� */
                    }
                }

                if (tp_dev.sta & TP_PRES_DOWN)          /* ������������ */
                {
                    if (tp_dev.y[0] < (lcddev.height - gui_phy.tbheight) && tp_dev.y[0] > (gui_phy.tbheight + 1))   /* �ڻ�ͼ������ */
                    {
                        x = tp_dev.x[0];
                        y = tp_dev.y[0];
                        tmp_buf[0] = tp_dev.x[0] >> 8;
                        tmp_buf[1] = tp_dev.x[0] & 0xFF;
                        tmp_buf[2] = tp_dev.y[0] >> 8;
                        tmp_buf[3] = tp_dev.y[0] & 0xFF;
                        tmp_buf[4] |= 0X01;             /* ����Ϊ1,������ͼ */
                        nrf24l01_tx_packet(tmp_buf);    /* �������� */
                    }
                }
            }
            else    /* ����ģʽ */
            {
                if (nrf24l01_rx_packet(tmp_buf) == 0)   /* һ�����յ���Ϣ,����ʾ���� */
                {
                    x = tmp_buf[0];
                    x = (x << 8) + tmp_buf[1];
                    y = tmp_buf[2];
                    y = (y << 8) + tmp_buf[3];
                }
            }

            if (tmp_buf[4] & 0X7F)  /* ��Ҫ���� */
            {
                switch (tmp_buf[4] & 0X7F)
                {
                    case 1:         /* �������� */
                        if (tmp_buf[4] & 0X80)      /* ������ */
                        {
                            if (lastx == 0XFFFF)    /* ��һ�� */
                            {
                                lastx = x;
                                lasty = y;
                            }

                            gui_draw_bline(lastx, lasty, x, y, 2, RED); /* ���� */
                            lastx = x;
                            lasty = y;
                        }
                        else paint_draw_point(x, y, RED, 2);            /* ��ͼ,�뾶Ϊ2 */

                        break;

                    case 2:/* ��� */
                        gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - 2 * gui_phy.tbheight, LGRAY); /* �����Ļ */
                        break;

                    case 3:/* �˳� */
                        rval = 1; /* ��־�˳� */
                        break;
                }
            }
            else lastx = 0XFFFF;

            delay_ms(5);/* ���� ��ʱһ��ʱ�ӽ��� */
        }
    }

    spi2_init();                    /* ��ʼ��SPI */
    spi2_set_speed(SPI_SPEED_4);    /* ���õ�����ģʽ */
    btn_delete(cbtn);               /* ɾ����ť */
    return 0;
}






















