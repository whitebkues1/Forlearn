/**
 ****************************************************************************************************
 * @file        phoneplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-07
 * @brief       APP-������ ����
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
 * V1.1 20220607
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "phoneplay.h"
#include "spb.h"
#include "camera.h"
//#include "sai.h"
//#include "audioplay.h"
#include "./BSP/USART3/usart3.h"


/******************************************************************************************/
/* ������������ */

#define PHONE_INCALL_TASK_PRIO      4       /* �����������ȼ� */
#define PHONE_INCALL_STK_SIZE       512     /* ���������ջ��С */

OS_STK *PHONE_INCALL_TASK_STK = 0;          /* �����ջ�������ڴ����ķ�ʽ�������� */

void phone_incall_task(void *pdata);        /* ������ */

/******************************************************************************************/

/* ��绰ͼƬ */
uint8_t *const PHONE_CALL_PIC[3] =
{
    "1:/SYSTEM/APP/PHONE/call_28.bmp",
    "1:/SYSTEM/APP/PHONE/call_36.bmp",
    "1:/SYSTEM/APP/PHONE/call_54.bmp",
};

/* �ҵ绰ͼƬ */
uint8_t *const PHONE_HANGUP_PIC[3] =
{
    "1:/SYSTEM/APP/PHONE/hangup_28.bmp",
    "1:/SYSTEM/APP/PHONE/hangup_36.bmp",
    "1:/SYSTEM/APP/PHONE/hangup_54.bmp",
};

/* �˸�ͼƬ */
uint8_t *const PHONE_DEL_PIC[3] =
{
    "1:/SYSTEM/APP/PHONE/del_28.bmp",
    "1:/SYSTEM/APP/PHONE/del_36.bmp",
    "1:/SYSTEM/APP/PHONE/del_54.bmp",
};

/* �绰��ʾ��Ϣ */
uint8_t *const phone_remind_tbl[4][GUI_LANGUAGE_NUM] =
{
    "������...", "��̖��...", "Dialing...",
    "����...", "����...", "Calling...",
    "δ�ҵ�GSMģ��,����!", "δ�ҵ�GSMģ�K,Ո�z��!", "GSM Module Error!Pleas Check!",
    "SIM���쳣,����!", "SIM������,Ո�z��!", "SIM Card Error!Pleas Check!",
};

/* �绰����/�ܾ���ť���� */
uint8_t *const phone_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    "����", "�� ", "ANSWER",
    "�ܾ�", "�ܽ^", "REFUSE",
};

/* �������Ʊ� */
uint8_t *const call_btnstr_tbl[14] =
{
    "1", "2", "3",
    "4", "5", "6",
    "7", "8", "9",
    "*", "0", "#",
    "call", "del",
};

/**
 * @brief       ��ʾ��������
 * @param       pdis            : ��ʾ�ṹ��
 * @retval      ��
 */
void phone_show_clear(_phonedis_obj *pdis)
{
    gui_fill_rectangle(pdis->xoff, pdis->yoff, pdis->width, pdis->height, BLACK); /* ���ԭ������ʾ */
}

/**
 * @brief       ��ʾͨ��ʱ��
 * @param       pdis            : ��ʾ�ṹ��
 * @retval      ��
 */
void phone_show_time(_phonedis_obj *pdis)
{
    uint8_t *pbuf;
    uint16_t yoff;
    uint16_t xoff;
    pbuf = gui_memin_malloc(100);

    if (pbuf == 0)return ;

    sprintf((char *)pbuf, "%03d:%02d", pdis->time / 60, pdis->time % 60);
    gui_phy.back_color = BLACK;
    xoff = pdis->xoff + pdis->width / 2 - 5 * pdis->tsize / 2;
    yoff = pdis->yoff + pdis->height / 2 - pdis->tsize / 2;
    gui_fill_rectangle(xoff, yoff, 5 * pdis->tsize, pdis->tsize, BLACK); /* ���ԭ������ʾ */
    gui_show_strmid(xoff, yoff, 5 * pdis->tsize, pdis->tsize, WHITE, pdis->tsize, pbuf);
    gui_memin_free(pbuf);/* �ͷ��ڴ� */
}

/**
 * @brief       ��ʾ������
 * @param       pdis            : ��ʾ�ṹ��
 * @param       gsmx            : GSM״̬�ṹ��
 * @retval      ��
 */
void phone_show_calling(_phonedis_obj *pdis, __gsmdev *gsmx)
{
    uint16_t yoff;
    uint16_t xoff;
    gui_phy.back_color = BLACK;
    xoff = pdis->xoff + pdis->width / 2 - 5 * pdis->tsize / 2;
    yoff = pdis->yoff + pdis->height / 2 - pdis->tsize / 2;
    gui_fill_rectangle(xoff, yoff, 5 * pdis->tsize, pdis->tsize, BLACK); /* ���ԭ������ʾ */

    if (gsmx->mode == 3)gui_show_strmid(xoff, yoff, 5 * pdis->tsize, pdis->tsize, WHITE, pdis->tsize, phone_remind_tbl[1][gui_phy.language]);
    else gui_show_strmid(xoff, yoff, 5 * pdis->tsize, pdis->tsize, WHITE, pdis->tsize, phone_remind_tbl[0][gui_phy.language]);
}

/**
 * @brief       ��ʾ�绰����
 * @param       pdis            : ��ʾ�ṹ��
 * @param       gsmx            : GSM״̬�ṹ��
 * @retval      ��
 */
void phone_show_phone(_phonedis_obj *pdis, __gsmdev *gsmx)
{
    uint16_t yoff;
    uint16_t xoff;
    uint8_t maxnum;
    maxnum = pdis->width / (pdis->psize / 2);

    if (pdis->plen > maxnum)maxnum = pdis->plen - maxnum;
    else maxnum = 0;

    if (gsmx->mode == 0) /* ��������ģʽ,�������ڵ绰������ʾ */
    {
        xoff = pdis->xoff;
        yoff = pdis->yoff + pdis->height / 2 - pdis->psize / 2;
        gui_fill_rectangle(xoff, yoff, pdis->width, pdis->psize, BLACK); /* ���ԭ������ʾ */
        gui_show_strmid(xoff, yoff, pdis->width, pdis->psize, WHITE, pdis->psize, pdis->phonebuf + maxnum); /* ��ʾ�绰���� */
    }
    else
    {
        xoff = pdis->xoff;
        yoff = pdis->yoff + pdis->height / 2 - pdis->tsize / 2 - pdis->psize;
        gui_fill_rectangle(xoff, yoff, pdis->width, pdis->psize, BLACK); /* ���ԭ������ʾ */
        gui_show_strmid(xoff, yoff, pdis->width, pdis->psize, WHITE, pdis->psize, pdis->phonebuf + maxnum); /* ��ʾ�绰���� */
    }
}

/**
 * @brief       ��ʾ�����ַ���
 * @param       pdis            : ��ʾ�ṹ��
 * @param       gsmx            : GSM״̬�ṹ��
 * @retval      ��
 */
void phone_show_input(_phonedis_obj *pdis, __gsmdev *gsmx)
{
    uint16_t yoff;
    uint16_t xoff;
    uint8_t maxnum;

    if (gsmx->mode == 2) /* ���ڵ绰��ͨ����Ч */
    {
        maxnum = pdis->width / (pdis->psize / 2);

        if (pdis->inlen > maxnum)maxnum = pdis->inlen - maxnum;
        else maxnum = 0;

        xoff = pdis->xoff;
        yoff = pdis->yoff + pdis->height / 2 + pdis->tsize;
        gui_fill_rectangle(xoff, yoff, pdis->width, pdis->psize, BLACK); /* ���ԭ������ʾ */
        gui_show_strmid(xoff, yoff, pdis->width, pdis->psize, WHITE, pdis->psize, pdis->inbuf + maxnum); /* ��ʾ�������� */
    }
}

/**
 * @brief       �绰��ʾ��
 * @param       ��
 * @retval      ��
 */
void phone_ring(void)
{
    uint8_t i;

    for (i = 0; i < 2; i++)
    {
        BEEP(1);
        delay_ms(100);
        BEEP(0);
        delay_ms(50);
    }
}

/**
 * @brief       ����״̬��ʾʱ��
 * @param       x,y,width,height: ����ʹ�С
 * @param       c               : ������ɫ 
 * @param       pdis            : ��ʾ�ṹ��
 * @retval      ��
 */
void phone_incall_show_time(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t c, _phonedis_obj *pdis)
{
    uint8_t *pbuf;
    uint16_t yoff;
    uint16_t xoff;
    pbuf = gui_memin_malloc(100);

    if (pbuf == 0)return ;

    sprintf((char *)pbuf, "%03d:%02d", pdis->time / 60, pdis->time % 60);
    gui_phy.back_color = c;
    xoff = (width - 3 * pdis->tsize) / 2;
    yoff = (height - pdis->tsize) / 2;
    gui_show_ptstr(x + xoff, y + yoff, x + width, y + height, 0, WHITE, pdis->tsize, pbuf, 0);
    gui_memin_free(pbuf);/* �ͷ��ڴ� */
}

/**
 * @brief       ��������,ר�Ŵ���绰����.ͨ���������򴴽�
 * @param       pdata           : δ�õ����������
 * @retval      ��
 */
void phone_incall_task(void *pdata)
{
    uint8_t rval = 0, res;
    _window_obj *twin = 0;  /* ���� */
    _btn_obj *rbtn = 0;     /* ȡ����ť */
    _btn_obj *okbtn = 0;    /* ȷ����ť */
    uint16_t calltime = 0;
    uint32_t curtime = 0;
    uint8_t *pbuf;
    uint8_t camflag = 0;    /* ����ͷ������־ */
    OSTaskSuspend(6);       /* ���������� */

    if (EXTI->IMR & (1 << 8))   /* ����ͷ���ڹ���? */
    {
        camflag = 1;
        EXTI->IMR &= ~(1 << 8); /* �ر�line8�ж� */
        lcd_scan_dir(DFT_SCAN_DIR); /* �ָ�Ĭ��ɨ�跽�� */
    }

    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* ����Ϊȫ������ */
    twin = window_creat((lcddev.width - 150) / 2, (lcddev.height - 60) / 2, 150, 60, 0, 1 | 1 << 5 | 1 << 6, 16);   /* �������� */
    okbtn = btn_creat((lcddev.width - 150) / 2 + 15, (lcddev.height - 60) / 2 + 30, 50, 24, 0, 0x02);               /* ������ť */
    rbtn = btn_creat((lcddev.width - 150) / 2 + 15 + 50 + 20, (lcddev.height - 60) / 2 + 30, 50, 24, 0, 0x02);      /* ������ť */
    pbuf = gui_memin_malloc(100);

    if (!twin || !rbtn || !okbtn || !pbuf || rval)rval = 1;
    else
    {
        /* ���ڵ����ֺͱ���ɫ */
        strcpy((char *)pbuf, (char *)gsmdev.incallnum); /* �������뵽pbuf���� */
        strcat((char *)pbuf, (char *)phone_remind_tbl[1][gui_phy.language]);
        twin->caption = pbuf;
        twin->font = 12;            /* ����12������ */
        twin->captionheight = 24;   /* �޸�caption�߶�Ϊ24 */
        twin->windowbkc = APP_WIN_BACK_COLOR;
        
        /* ���ذ�ť����ɫ */
        rbtn->bkctbl[0] = 0XFA69;   /* �߿���ɫ */
        rbtn->bkctbl[1] = 0XFA69;   /* ��һ�е���ɫ */
        rbtn->bkctbl[2] = 0XFA69;   /* �ϰ벿����ɫ */
        rbtn->bkctbl[3] = 0XFBEF;   /* �°벿����ɫ */

        okbtn->bkctbl[0] = 0X3DEA;  /* �߿���ɫ */
        okbtn->bkctbl[1] = 0X3DEA;  /* ��һ�е���ɫ */
        okbtn->bkctbl[2] = 0X3DEA;  /* �ϰ벿����ɫ */
        okbtn->bkctbl[3] = 0X76B2;  /* �°벿����ɫ */

        rbtn->caption = (uint8_t *)phone_btncaption_tbl[1][gui_phy.language];   /* ����Ϊ�ܾ� */
        okbtn->caption = (uint8_t *)phone_btncaption_tbl[0][gui_phy.language];  /* ����Ϊ���� */
        window_draw(twin);          /* �������� */
        btn_draw(rbtn);             /* ����ť */
        btn_draw(okbtn);            /* ����ť */

        while (gsmdev.mode != 0)    /* �ǲ���ģʽ */
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

            if (system_task_return)break;           /* TPAD����,���ܾ����� */

            if (g_usart3_rx_sta & 0X8000)           /* ���յ����� */
            {
                if (gsm_check_cmd("NO CARRIER"))gsmdev.mode = 0;    /* ����ʧ�� */

                if (gsm_check_cmd("NO ANSWER"))gsmdev.mode = 0;     /* ����ʧ�� */

                if (gsm_check_cmd("ERROR"))gsmdev.mode = 0;         /* ����ʧ�� */

                g_usart3_rx_sta = 0;
            }

            if (gsmdev.mode == 2) /* ͨ���� */
            {
                if ((OSTime - curtime) >= OS_TICKS_PER_SEC)         /* ����1������ */
                {
                    curtime = OSTime;
                    calltime++;
                    sprintf((char *)pbuf, "%02d:%02d", calltime / 60, calltime % 60);
                    gui_fill_rectangle((lcddev.width - 150) / 2 + 15, (lcddev.height - 60) / 2 + 30, 50, 24, twin->windowbkc);
                    gui_show_strmid((lcddev.width - 150) / 2 + 15, (lcddev.height - 60) / 2 + 30, 50, 24, BLUE, 16, pbuf);
                }
            }

            res = btn_check(rbtn, &in_obj);         /* �ܾ���ť��� */

            if (res && ((rbtn->sta & 0X80) == 0))   /* ����Ч���� */
            {
                gsmdev.mode = 0;    /* ���벦��ģʽ */
                break;
            }

            if (gsmdev.mode != 2)
            {
                res = btn_check(okbtn, &in_obj);    /* ������ť��� */

                if (res && (okbtn->sta & 0X80) == 0)    /* ������... */
                {
                    gsmdev.mode = 2;    /* ͨ���� */
                    gsm_send_cmd("ATA", "OK", 200);     /* �����绰 */
                    gsm_cmd_over();     /* ������� */
                    curtime = OSTime;   /* ��¼�ոս���ͨ��ʱ��ʱ�� */
                    calltime = 0;
                    strcpy((char *)pbuf, (char *)gsmdev.incallnum); /* �������뵽pbuf���� */
                    strcat((char *)pbuf, (char *)phone_remind_tbl[4][gui_phy.language]);
                    twin->caption = pbuf;
                    window_draw(twin);  /* �������� */
                    btn_draw(rbtn);     /* ����ť */
                }
            }
        }

        gsm_send_cmd("ATH", "OK", 200); /* �һ� */
        gsm_cmd_over();                 /* ������� */
    }

    window_delete(twin);    /* ɾ������ */
    btn_delete(rbtn);       /* ɾ����ť */
    btn_delete(okbtn);      /* ɾ����ť */
    gui_memin_free(pbuf);   /* �ͷ��ڴ� */
    gsmdev.mode = 0;        /* ���벦��ģʽ */
    system_task_return = 0; /* ��ֹ�˳��������� */
    OSTaskResume(6);        /* �ָ������� */

    if (camflag)
    {
        EXTI->IMR |= 1 << 8;    /* ����line8�ж� */
    }

    phone_incall_task_delete(); /* ɾ������ */
}

/**
 * @brief       ������������
 * @param       ��
 * @retval      0, �ɹ�; ����, �������;
 */
uint8_t phone_incall_task_creat(void)
{
    OS_CPU_SR cpu_sr = 0;
    uint8_t res;

    if (PHONE_INCALL_TASK_STK)return 0;         /* �����Ѿ������� */

    PHONE_INCALL_TASK_STK = gui_memin_malloc(PHONE_INCALL_STK_SIZE * sizeof(OS_STK));

    if (PHONE_INCALL_TASK_STK == 0)return 1;    /* �ڴ�����ʧ�� */

    OS_ENTER_CRITICAL();    /* �����ٽ���(�޷����жϴ��) */
    res = OSTaskCreate(phone_incall_task, (void *)0, (OS_STK *)&PHONE_INCALL_TASK_STK[PHONE_INCALL_STK_SIZE - 1], PHONE_INCALL_TASK_PRIO);
    OS_EXIT_CRITICAL();     /* �˳��ٽ���(���Ա��жϴ��) */
    return res;
}

/**
 * @brief       ɾ����������
 * @param       ��
 * @retval      ��
 */
void phone_incall_task_delete(void)
{
    gui_memin_free(PHONE_INCALL_TASK_STK);  /* �ͷ��ڴ� */
    PHONE_INCALL_TASK_STK = 0;
    OSTaskDel(PHONE_INCALL_TASK_PRIO);      /* ɾ���������� */
}

/**
 * @brief       �绰����������
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t phone_play(void)
{
    _btn_obj *p_btn[14];/* �ܹ�14������ */
    _phonedis_obj *pdis;
    uint8_t btnxsize, btnysize;
    uint8_t btnxydis;
    uint8_t fsize;
    uint8_t lcdtype;
    uint16_t yoff;

    uint8_t i, j;
    uint8_t rval = 0;
    uint8_t res = 0;
    FIL *f_phone = 0;
    uint32_t calltime;
    uint32_t br;

    pdis = gui_memin_malloc(sizeof(_phonedis_obj));

    if (pdis == NULL)return 1;              /* ���벻�ɹ�,ֱ���˳� */

    memset(pdis, 0, sizeof(_phonedis_obj)); /* �������� */
    pdis->inbuf = gui_memin_malloc(PHONE_MAX_INPUT);
    pdis->phonebuf = gui_memin_malloc(PHONE_MAX_INPUT);
    f_phone = (FIL *)gui_memin_malloc(sizeof(FIL)); /* ����FIL�ֽڵ��ڴ����� */

    if (!pdis->phonebuf || !pdis->inbuf || !f_phone)rval = 1;   /* ����ʧ�� */
    else
    {
        if (lcddev.width <= 272)
        {
            res = f_open(f_phone, (const TCHAR *)APP_ASCII_2814, FA_READ); /* ���ļ� */

            if (res == FR_OK)
            {
                asc2_2814 = (uint8_t *)gui_memex_malloc(f_phone->obj.objsize);  /* Ϊ�����忪�ٻ����ַ */

                if (asc2_2814 == 0)rval = 1;
                else res = f_read(f_phone, asc2_2814, f_phone->obj.objsize, (UINT *)&br);   /* һ�ζ�ȡ�����ļ� */

                f_close(f_phone);
            }
        }
        else if (lcddev.width == 320)
        {
            res = f_open(f_phone, (const TCHAR *)APP_ASCII_3618, FA_READ); /* ���ļ� */

            if (res == FR_OK)
            {
                asc2_3618 = (uint8_t *)gui_memex_malloc(f_phone->obj.objsize);  /* Ϊ�����忪�ٻ����ַ */

                if (asc2_3618 == 0)rval = 1;
                else res = f_read(f_phone, asc2_3618, f_phone->obj.objsize, (UINT *)&br);   /* һ�ζ�ȡ�����ļ� */

                f_close(f_phone);
            }
        }
        else if (lcddev.width >= 480)
        {
            res = f_open(f_phone, (const TCHAR *)APP_ASCII_5427, FA_READ);  /* ���ļ� */

            if (res == FR_OK)
            {
                asc2_5427 = (uint8_t *)gui_memex_malloc(f_phone->obj.objsize);  /* Ϊ�����忪�ٻ����ַ */

                if (asc2_5427 == 0)rval = 1;
                else res = f_read(f_phone, asc2_5427, f_phone->obj.objsize, (UINT *)&br);   /* һ�ζ�ȡ�����ļ� */

                f_close(f_phone);
            }
        }

        if (res)rval = res;
    }

    if (rval == 0) /* ����ɹ� */
    {
        lcd_clear(BLACK);

        if (lcddev.width == 240)
        {
            btnxsize = 78;
            btnysize = 40;
            btnxydis = 2;
            fsize = 28;
            pdis->psize = 28;
            pdis->tsize = 12;
            lcdtype = 0;
        }

        if (lcddev.width == 272)
        {
            btnxsize = 86;
            btnysize = 50;
            btnxydis = 4;
            fsize = 28;
            pdis->psize = 28;
            pdis->tsize = 12;
            lcdtype = 0;
        }
        else if (lcddev.width == 320)
        {
            btnxsize = 102;
            btnysize = 60;
            btnxydis = 4;
            fsize = 36;
            pdis->psize = 36;
            pdis->tsize = 16;
            lcdtype = 1;
        }
        else if (lcddev.width == 480)
        {
            btnxsize = 154;
            btnysize = 100;
            btnxydis = 6;
            fsize = 54;
            pdis->psize = 54;
            pdis->tsize = 24;
            lcdtype = 2;
        }
        else if (lcddev.width == 600)
        {
            btnxsize = 190;
            btnysize = 125;
            btnxydis = 10;
            fsize = 54;
            pdis->psize = 54;
            pdis->tsize = 24;
            lcdtype = 2;
        }
        else if (lcddev.width == 800)
        {
            btnxsize = 250;
            btnysize = 150;
            btnxydis = 10;
            fsize = 54;
            pdis->psize = 54;
            pdis->tsize = 24;
            lcdtype = 2;
        }

        pdis->xoff = 0;
        pdis->yoff = 0;
        pdis->width = lcddev.width;
        pdis->height = lcddev.height - 5 * btnysize - 6 * btnxydis;
        yoff = lcddev.height - btnysize * 5 - btnxydis * 6;

        for (i = 0; i < 5; i++) /* ����14����ť */
        {
            for (j = 0; j < 3; j++)
            {
                res = i * 3 + j;

                if (i == 4)
                {
                    if (j == 2)break;

                    p_btn[res] = btn_creat(j * (btnxsize + btnxsize / 2 + (btnxydis * 3) / 2) + btnxydis / 2, yoff + i * (btnysize + btnxydis) + btnxydis, (btnxsize * 3) / 2 + btnxydis / 2, btnysize, 0, 1);  /* ����ͼƬ��ť */

                    if (j == 0)
                    {
                        p_btn[res]->bcfdcolor = 0X76B2;;    /* ����ʱ�ı���ɫ */
                        p_btn[res]->bcfucolor = 0X3DEA;     /* �ɿ�ʱ����ɫ */
                        p_btn[res]->picbtnpathu = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                        p_btn[res]->picbtnpathd = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                    }
                    else
                    {
                        p_btn[res]->bcfdcolor = 0X630C;;    /* ����ʱ�ı���ɫ */
                        p_btn[res]->bcfucolor = 0X4208;     /* �ɿ�ʱ����ɫ */
                        p_btn[res]->picbtnpathu = (uint8_t *)PHONE_DEL_PIC[lcdtype];
                        p_btn[res]->picbtnpathd = (uint8_t *)PHONE_DEL_PIC[lcdtype];
                    }
                }
                else
                {
                    p_btn[res] = btn_creat(j * (btnxsize + btnxydis) + btnxydis / 2, yoff + i * (btnysize + btnxydis) + btnxydis, btnxsize, btnysize, 0, 2); /* ����Բ�ǰ�ť */
                    p_btn[res]->bkctbl[0] = 0X39E7;     /* �߿���ɫ */
                    p_btn[res]->bkctbl[1] = 0X73AE;     /* ��һ�е���ɫ */
                    p_btn[res]->bkctbl[2] = 0X4208;     /* �ϰ벿����ɫ */
                    p_btn[res]->bkctbl[3] = 0X3186;     /* �°벿����ɫ */
                    p_btn[res]->bcfucolor = WHITE;      /* �ɿ�ʱΪ��ɫ */
                    p_btn[res]->bcfdcolor = BLACK;      /* ����ʱΪ��ɫ */
                    p_btn[res]->caption = (uint8_t *)call_btnstr_tbl[res];
                }

                if (lcddev.width <= 272)p_btn[res]->font = 16;
                else p_btn[res]->font = 24;

                if (p_btn[res] == NULL)
                {
                    rval = 1;   /* ����ʧ�� */
                    break;
                }

                p_btn[res]->sta = 0;
                p_btn[res]->font = fsize;
            }
        }
    }

    if (rval == 0)
    {
        for (i = 0; i < 14; i++)btn_draw(p_btn[i]);

        gsmdev.mode = 0;
        memset(pdis->inbuf, 0, PHONE_MAX_INPUT);
        memset(pdis->phonebuf, 0, PHONE_MAX_INPUT);

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(5);    /* ��ʱһ��ʱ�ӽ��� */

            if (system_task_return)break;   /* TPAD���� */
    
            if (g_usart3_rx_sta & 0X8000)     /* ���յ����� */
            {
                if (gsmdev.mode == 1 || gsmdev.mode == 2)
                {
                    if (gsmdev.mode == 1)
                    {
                        if (gsm_check_cmd("+COLP:") || gsm_check_cmd("CONNECT"))    /* �յ�+COLP����CONNECT, ��ʾ���ųɹ� */
                        {
                            pdis->time = 0;
                            calltime = OSTime;  /* ��¼�ոս���ͨ��ʱ��ʱ�� */
                            gsmdev.mode = 2;    /* ���ųɹ� */
                        }
                    }

                    if (gsm_check_cmd("NO CARRIER"))gsmdev.mode = 0;    /* ����ʧ�� */

                    if (gsm_check_cmd("NO ANSWER"))gsmdev.mode = 0;     /* ����ʧ�� */

                    if (gsm_check_cmd("ERROR"))gsmdev.mode = 0;         /* ����ʧ�� */

                    if (gsmdev.mode == 0)
                    {
                        phone_show_clear(pdis);         /* ���ԭ������ʾ */
                        p_btn[12]->bcfdcolor = 0X76B2;  /* ����ʱ�ı���ɫ */
                        p_btn[12]->bcfucolor = 0X3DEA;  /* �ɿ�ʱ����ɫ */
                        p_btn[12]->picbtnpathu = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                        p_btn[12]->picbtnpathd = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                        btn_draw(p_btn[12]);
                        pdis->inlen = 0;
                        pdis->inbuf[pdis->inlen] = 0;   /* ������ */
                        phone_show_phone(pdis, &gsmdev);/* ��ʾ�绰���� */
                    }

                    gsm_cmd_over(); /* ������� */
                }
            }

            if (gsmdev.mode == 2)   /* ͨ���� */
            {
                if ((OSTime - calltime) >= OS_TICKS_PER_SEC) /* ����1������ */
                {
                    calltime = OSTime;
                    pdis->time++;
                    phone_show_time(pdis);/* ��ʾʱ�� */
                }
            }

            for (i = 0; i < 14; i++)
            {
                res = btn_check(p_btn[i], &in_obj);

                if (res && ((p_btn[i]->sta & (1 << 7)) == 0) && (p_btn[i]->sta & (1 << 6))) /* �а����������ɿ�,����TP�ɿ��� */
                {
                    if (i < 12) /* 1~# */
                    {
                        if (gsmdev.mode == 0) /* ��������ģʽ */
                        {
                            if (pdis->plen < PHONE_MAX_INPUT - 1)
                            {
                                u3_printf("AT+CLDTMF=2,\"%c\"\r\n", call_btnstr_tbl[i][0]);
                                pdis->phonebuf[pdis->plen] = call_btnstr_tbl[i][0]; /* ����ַ� */
                                pdis->plen++;
                                pdis->phonebuf[pdis->plen] = 0; /* ��ӽ����� */
                            }

                        }
                        else if (gsmdev.mode == 2) /* ����ͨ������Ч */
                        {
                            if (pdis->inlen < PHONE_MAX_INPUT - 1)
                            {
                                u3_printf("AT+CLDTMF=2,\"%c\"\r\n", call_btnstr_tbl[i][0]);
                                delay_ms(150);
                                u3_printf("AT+VTS=%c\r\n", call_btnstr_tbl[i][0]);
                                pdis->inbuf[pdis->inlen] = call_btnstr_tbl[i][0];   /* ����ַ� */
                                pdis->inlen++;
                                pdis->inbuf[pdis->inlen] = 0; /* ��ӽ����� */
                            }
                        }
                    }

                    if (i == 12 && pdis->plen) /* �����к����ʱ����в��� */
                    {
                        if (gsmdev.mode == 0)
                        {
                            if ((gsmdev.status & 0XC0) != 0XC0) /* GSMģ��/SIM�� ����,��ʾ������Ϣ */
                            {
                                if ((gsmdev.status & 0X80) == 0)window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2, 220, 100, (uint8_t *)phone_remind_tbl[2][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 1 << 6, 1200);
                                else window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2, 220, 100, (uint8_t *)phone_remind_tbl[3][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 1 << 6, 1200);

                                break;
                            }

                            gsmdev.mode = 1; /* ���벦����ģʽ */
                            p_btn[12]->bcfdcolor = 0XFBEF;      /* ����ʱ�ı���ɫ */
                            p_btn[12]->bcfucolor = 0XFA69;      /* �ɿ�ʱ����ɫ */
                            p_btn[12]->picbtnpathu = (uint8_t *)PHONE_HANGUP_PIC[lcdtype];
                            p_btn[12]->picbtnpathd = (uint8_t *)PHONE_HANGUP_PIC[lcdtype];
                            phone_show_clear(pdis);             /* ���ԭ������ʾ */
                            phone_show_phone(pdis, &gsmdev);    /* ��ʾ�绰���� */
                            phone_show_calling(pdis, &gsmdev);  /* ������ */
                            u3_printf("ATD%s;\r\n", pdis->phonebuf);    /* ���� */
                        }
                        else
                        {
                            gsmdev.mode = 0; /* �ָ�����ģʽ */
                            p_btn[12]->bcfdcolor = 0X76B2;      /* ����ʱ�ı���ɫ */
                            p_btn[12]->bcfucolor = 0X3DEA;      /* �ɿ�ʱ����ɫ */
                            p_btn[12]->picbtnpathu = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                            p_btn[12]->picbtnpathd = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                            phone_show_clear(pdis);             /* ���ԭ������ʾ */
                            pdis->inlen = 0;
                            pdis->inbuf[pdis->inlen] = 0;       /* ������ */
                            gsm_send_cmd("ATH", "OK", 200);     /* �һ� */
                            gsm_cmd_over();/* ������� */
                        }

                        btn_draw(p_btn[12]);
                    }

                    if (i == 13) /* �˸� */
                    {
                        if (gsmdev.mode == 0) /* ��������ģʽ */
                        {
                            if (pdis->plen)pdis->plen--;

                            pdis->phonebuf[pdis->plen] = 0; /* ������ */
                        }
                        else if (gsmdev.mode == 2)
                        {
                            if (pdis->inlen)pdis->inlen--;

                            pdis->inbuf[pdis->inlen] = 0;   /* ������ */
                        }
                    }

                    if (gsmdev.mode == 0)phone_show_phone(pdis, &gsmdev); /* ��ʾ�绰���� */

                    if (gsmdev.mode == 2)phone_show_input(pdis, &gsmdev); /* ��ʾ�������� */
                }
            }
        }
    }

    if (gsmdev.mode)
    {
        gsm_send_cmd("ATH", "OK", 200); /* �һ� */
        gsm_cmd_over();/* ������� */
    }

    gui_memin_free(f_phone);            /* �ͷ��ڴ� */
    gui_memin_free(pdis->inbuf);        /* �ͷ��ڴ� */
    gui_memin_free(pdis->phonebuf);     /* �ͷ��ڴ� */
    gui_memin_free(pdis);               /* �ͷ��ڴ� */

    if (lcddev.width <= 272)
    {
        gui_memex_free(asc2_2814);
        asc2_2814 = 0;
    }
    else if (lcddev.width == 320)
    {
        gui_memex_free(asc2_3618);
        asc2_3618 = 0;
    }
    else if (lcddev.width >= 480)
    {
        gui_memex_free(asc2_5427);
        asc2_5427 = 0;
    }

    for (i = 0; i < 14; i++)btn_delete(p_btn[i]); /* ɾ����ť */

    return 0;
}








































