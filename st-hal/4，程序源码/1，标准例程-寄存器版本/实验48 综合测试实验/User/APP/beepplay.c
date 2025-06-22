/**
 ****************************************************************************************************
 * @file        beepplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-11-01
 * @brief       APP-���������� ����
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
 * V1.0 20221101
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "beepplay.h"
#include "gradienter.h"
#include "./BSP/BEEP/beep.h"


/****************************************************************************************************/
/* ��gradienter.c���������Ĵ��� */

/**
 * @brief       ��������
 * @param       ��
 * @retval      0, OK;  ����, �������;
 */
uint8_t grad_load_font(void)
{
    uint8_t rval = 0;
    uint8_t res;
    uint32_t br;
    FIL *f_grad = 0;
    f_grad = (FIL *)gui_memin_malloc(sizeof(FIL));  /* ����FIL�ֽڵ��ڴ����� */

    if (f_grad == NULL)rval = 1;                    /* ����ʧ�� */
    else
    {
        if (lcddev.width <= 272)
        {
            res = f_open(f_grad, (const TCHAR *)APP_ASCII_S7236, FA_READ); /* ���ļ� */

            if (res == FR_OK)
            {
                asc2_7236 = (uint8_t *)gui_memex_malloc(f_grad->obj.objsize);   /* Ϊ�����忪�ٻ����ַ */

                if (asc2_7236 == 0)rval = 1;
                else res = f_read(f_grad, asc2_7236, f_grad->obj.objsize, (UINT *)&br); /* һ�ζ�ȡ�����ļ� */
            }
        }
        else if (lcddev.width >= 320)   /* 480*320��800*480�� ��Ļ ������� */
        {
            res = f_open(f_grad, (const TCHAR *)APP_ASCII_S8844, FA_READ);      /* ���ļ� */

            if (res == FR_OK)
            {
                asc2_8844 = (uint8_t *)gui_memex_malloc(f_grad->obj.objsize);   /* Ϊ�����忪�ٻ����ַ */

                if (asc2_8844 == 0)rval = 1;
                else res = f_read(f_grad, asc2_8844, f_grad->obj.objsize, (UINT *)&br); /* һ�ζ�ȡ�����ļ� */
            }
        }
//        else if (lcddev.width >= 480) /* ��Ӣ�� ��144*72���ļ�, �ڴ治��!! */
//        {
//            res = f_open(f_grad, (const TCHAR *)APP_ASCII_S14472, FA_READ); /* ���ļ� */

//            if (res == FR_OK)
//            {
//                asc2_14472 = (uint8_t *)gui_memex_malloc(f_grad->obj.objsize);  /* Ϊ�����忪�ٻ����ַ */

//                if (asc2_14472 == 0)rval = 1;
//                else res = f_read(f_grad, asc2_14472, f_grad->obj.objsize, (UINT *)&br);    /* һ�ζ�ȡ�����ļ� */
//            }
//        }

        if (res)rval = res;
    }

    gui_memin_free(f_grad);/* �ͷ��ڴ� */
    return rval;
}

/**
 * @brief       ɾ������
 * @param       ��
 * @retval      ��
 */
void grad_delete_font(void)
{
    if (lcddev.width == 240)
    {
        gui_memex_free(asc2_7236);
        asc2_7236 = 0;
    }
    else if (lcddev.width >= 320)
    {
        gui_memex_free(asc2_8844);
        asc2_8844 = 0;
    }
//    else if (lcddev.width >= 480)
//    {
//        gui_memex_free(asc2_14472);
//        asc2_14472 = 0;
//    }
}

/****************************************************************************************************/


/* BEEP��ť���� */
uint8_t *const beep_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    {"��", "���_", "ON",},
    {"�ر�", "�P�]", "OFF",},
};

/**
 * @brief       ����������
 * @param       caption         : ��������
 * @retval      δ�õ�
 */
uint8_t beep_play(uint8_t *caption)
{

    uint8_t res, rval = 0;
    uint8_t *buf;
    uint8_t beepsta = 0;

    _btn_obj *beepbtn = 0;  /* ���ư�ť */
    uint16_t btnx, btny, btnw, btnh;    /* ����������� */
    uint16_t cx, cy, cr;    /* Բ������� */
    uint8_t btnfsize = 24;  /* �����С */
    uint8_t fsize = 0;      /* ON/OFF�����С */

//    if (lcddev.width > 320)btnfsize = 32; /* ս���ȿ���������������24���� */
//    else btnfsize = 24;

    btnw = lcddev.width * 2 / 3;
    btnh = btnw / 4;
    btnx = (lcddev.width - btnw) / 2;
    btny = lcddev.height - 2 * btnh;

    if (lcddev.width <= 272)
    {
        fsize = 72;
    }
    else if (lcddev.width == 320)
    {
        fsize = 88;
    }
    else if (lcddev.width >= 480)
    {
        fsize = 88; /* 144����̫����,�����ڴ� */
    }

    cx = lcddev.width / 2;
    cy = cx + gui_phy.tbheight;
    cr = lcddev.width / 3;
    beepbtn = btn_creat(btnx, btny, btnw, btnh, 0, 0);
    buf = gui_memin_malloc(32); /* �����ڴ� */

    if (buf && beepbtn)rval = grad_load_font(); /* �������� */
    else rval = 1;

    if (rval == 0)
    {
        lcd_clear(LGRAY);
        app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);  /* �·ֽ��� */
        gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, caption); /* ��ʾ���� */

        beepbtn->caption = beep_btncaption_tbl[0][gui_phy.language];
        beepbtn->font = btnfsize;

        btn_draw(beepbtn);  /* ����ť */
        beepbtn->caption = beep_btncaption_tbl[1][gui_phy.language];

        gui_fill_circle(cx, cy, cr, RED);
        BEEP(0);            /* �رշ����� */
        sprintf((char *)buf, "OFF");
        gui_show_strmid(0, cy - (fsize / 2), lcddev.width, fsize, WHITE, fsize, buf);   /* ��ʾ������ */
        system_task_return = 0;

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            res = btn_check(beepbtn, &in_obj);

            if (res && ((beepbtn->sta & (1 << 7)) == 0) && (beepbtn->sta & (1 << 6)))   /* ������,�а����������ɿ�,����TP�ɿ��� */
            {
                beepbtn->caption = beep_btncaption_tbl[beepsta][gui_phy.language];
                beepsta = !beepsta;

                if (beepsta)
                {
                    sprintf((char *)buf, "ON");
                    BEEP(1);/* �򿪷����� */
                }
                else
                {
                    sprintf((char *)buf, "OFF");
                    BEEP(0);/* �رշ����� */
                }

                gui_fill_circle(cx, cy, cr, RED);
                gui_show_strmid(0, cy - (fsize / 2), lcddev.width, fsize, WHITE, fsize, buf);	/* ��ʾ������ */
            }

            if (system_task_return)break;   /* TPAD���� */

            delay_ms(3);
        }
    }

    BEEP(0);            /* �رշ����� */
    btn_delete(beepbtn);/* ɾ����ť */
    grad_delete_font(); /* ɾ������ */
    gui_memin_free(buf);/* �ͷ��ڴ� */
    return rval;
}







































