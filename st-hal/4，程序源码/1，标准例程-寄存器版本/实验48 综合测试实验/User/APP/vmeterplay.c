/**
 ****************************************************************************************************
 * @file        vmeterplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-06-06
 * @brief       APP-��ѹ����� ����
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
 * V1.1 20220606
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 *
 * V1.2 20221101
 * 1, ���STM32F103/F407��ϵ��, ����VMETER_ADC_MAX_VAL,����12/16bit ADC
 ****************************************************************************************************
 */

#include "vmeterplay.h"
#include "vmeterfont.c"
#include "paint.h"
#include "./BSP/TIMER/timer.h"
#include "./BSP/ADC/adc.h"

/* ������ʾ */
uint8_t *const vmeter_remindmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "1,ʹ��P7��DAC�Ŷ�ȡADC���ݣ�\r\
2,������׼��һ������ñ.\r\
3,��P7��RV1��DAC��������.\r\
4,����RV1�ۿ�ADC�����仯��",
    "1,ʹ��P7��DAC�_�xȡADC������\r\
2,Ո���Мʂ�һ������ñ.\r\
3,��P7��RV1��DAC�B������.\r\
4,�{��RV1�^��ADC�x��׃����",
    "1,Use P7 DAC pin read data!\r\
2,Prepare a jumper cap.\r\
3,Connect P7 DAC & RV1.\r\
4,Adjust RV1 and watch!",
};

/**
 * @brief       ��ָ����ַ��ʼ��ʾһ��������ַ�
 * @param       x,y             : ��ʼ����
 * @param       xend,yend       : �յ�����
 * @param       offset          : ��ʼ��ʾ��ƫ��
 * @param       color           : ��ɫ
 * @param       size            : �����С
 * @param       chr             : ��ʾ���ַ�
 * @param       mode            : 0,�ǵ�����ʾ;  1,������ʾ;  2,������(ֻ�����ڵ���ģʽ)
 * @retval      ��
 */
void vmeter_show_7seg(uint16_t x, uint16_t y, uint16_t xend, uint16_t yend, uint16_t offset, uint16_t color, uint16_t size, uint8_t chr, uint8_t mode)
{
    uint8_t temp;
    uint16_t t1, t;
    uint16_t tempoff;
    uint16_t y0 = y;
    uint16_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);    /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */

    if (chr == ' ')chr = 0;
    else if (chr == '.')chr = 1;
    else if (chr >= '0')chr = chr - '0' + 2; /* �����0~9,���ȥ'0'+2 */

    if (chr > 11)return;    /* ֻ��12���ַ� */

    for (t = 0; t < csize; t++)
    {
        switch (size)
        {
            case 48:
                temp = dig7_4824[chr][t];   /* ����1206���� */
                break;

            case 64:
                temp = dig7_6432[chr][t];   /* ����1608���� */
                break;

            case 96:
                temp = dig7_9648[chr][t];   /* ����2412���� */
                break;

            default:/* ��֧�ֵ����� */
                return;
        }

        tempoff = offset;

        if (x > xend)return;    /* �������� */

        if (tempoff == 0)       /* ƫ�Ƶ�ַ���� */
        {
            for (t1 = 0; t1 < 8; t1++)
            {
                if (y <= yend)
                {
                    if (temp & 0x80)
                    {
                        if (mode == 0x02)gui_draw_bigpoint(x, y, color);
                        else gui_phy.draw_point(x, y, color);
                    }
                    else if (mode == 0)gui_phy.draw_point(x, y, gui_phy.back_color);
                }

                temp <<= 1;
                y++;

                if ((y - y0) == size)
                {
                    y = y0;
                    x++;
                    break;
                }
            }
        }
        else
        {
            y += 8;

            if ((y - y0) >= size)   /* ����һ���ֵĸ߶��� */
            {
                y = y0; /* y������� */
                tempoff--;
            }
        }
    }
}

/**
 * @brief       ��ʾlen������
 * @param       x,y             : ��ʼ����
 * @param       len             : ���ֵ�λ��
 * @param       color           : ��ɫ
 * @param       size            : �����С
 * @param       num             : ��ֵ(0~2^64)
 * @param       mode            : ģʽ
 *                                [7]:0,�����;1,���0.
 *                                [3:0]:0,�ǵ�����ʾ;  1,������ʾ;  2,������(ֻ�����ڵ���ģʽ)
 * @retval      ��
 */
void vmeter_show_num(uint16_t x, uint16_t y, uint8_t len, uint16_t color, uint8_t size, long long num, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / gui_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)vmeter_show_7seg(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, '0', mode & 0xf); /* ���0 */
                else vmeter_show_7seg(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, ' ', mode & 0xf);

                continue;
            }
            else enshow = 1;

        }

        vmeter_show_7seg(x + (size / 2)*t, y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, size, temp + '0', mode & 0xf);
    }
}

/**
 * @brief       ��ʾ��ѹֵ,ĩβ��0����ʾ
 * @param       x,y             : ��ʼ����
 * @param       color           : ��ɫ
 * @param       size            : �����С
 * @param       res             : ��ѹֵ
 * @retval      ��
 */
//void vmeter_show_vol(uint16_t x, uint16_t y, uint16_t color, uint8_t size, float res)
//{
//    uint8_t voli = 0;   /* �������� */
//    uint16_t volf = 0;  /* С������ */
//    uint16_t offx = 0;
//    uint8_t i = 0;

//    voli = res;         /* ȡ���������� */
//    volf = (res - voli) * 1000; /* ȡ��С������ */

//    if (volf == 0)
//    {
//        offx = size * 2;    /* ֻ��ʾһ������ */
//        vmeter_show_num(x, y, 5, color, size, voli, 0);
//    }
//    else
//    {
//        while (i < 3)
//        {
//            if ((volf % 10) == 0)
//            {
//                volf /= 10;
//                vmeter_show_7seg(x + offx, y, x + offx + size / 2, y + size, 0, color, size, ' ', 0); /* ���֮ǰ����ʾ */
//                offx += size / 2;
//            }
//            else break;

//            i++;
//        }

//        x = x + i * size / 2;
//        vmeter_show_num(x, y, 1, color, size, voli, 0); /* ��ʾ�������� */
//        vmeter_show_7seg(x + size / 2, y, x + size, y + size, 0, color, size, '.', 0); /* ��ʾС���� */
//        vmeter_show_num(x + size, y, (3 - i), color, size, volf, 1 << 7); /* ��ʾС���� */
//    }
//}



/**
 * @brief       ��ѹ�����
 * @param       ��
 * @retval      δ�õ�
 */
uint8_t vmeter_play(void)
{
    uint16_t hx, hy, vx, vy;    /* 16�������ݺ͵�ѹֵx,y���� */
    uint16_t wx, wy;            /* �����洰��x,y���� */
    uint8_t segsize = 0;
    uint8_t fontsize = 16;
    uint8_t res = 0;
    uint8_t wr = 0;
    uint8_t segyoff = 0;
    FIL *f_seg = 0;

    float vol;
    uint32_t adcx;
    uint32_t oldadcx = 0;
    uint8_t t = 0;
    uint32_t br;

    app_muti_remind_msg((lcddev.width - 210) / 2, (lcddev.height - 158) / 2, 210, 158, APP_REMIND_CAPTION_TBL[gui_phy.language], vmeter_remindmsg_tbl[gui_phy.language]);

    lcd_clear(LGRAY);
    app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02); /* �·ֽ��� */
    gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[17][gui_phy.language]); /* ��ʾ���� */

    if (lcddev.width <= 272)
    {
        segsize = 48;
        fontsize = 24;
        segyoff = 5;
        wr = 10;
    }
    else if (lcddev.width == 320)
    {
        segsize = 64;
        fontsize = 32;
        segyoff = 7;
        wr = 13;
    }
    else if (lcddev.width >= 480)
    {
        segsize = 96;
        fontsize = 36;
        segyoff = 14;
        wr = 20;
        f_seg = (FIL *)gui_memin_malloc(sizeof(FIL)); /* ����FILENFO�ڴ� */
        res = f_open(f_seg, (const TCHAR *)APP_ASCII_3618, FA_READ); /* ���ļ� */

        if (res == FR_OK)
        {
            asc2_3618 = (uint8_t *)gui_memex_malloc(f_seg->obj.objsize);    /* Ϊ�����忪�ٻ����ַ */

            if (asc2_3618)res = f_read(f_seg, asc2_3618, f_seg->obj.objsize, (UINT *)&br);  /* һ�ζ�ȡ�����ļ� */
            else res = 0XFF;
        }
    }

    if (res == 0)
    {
        adc_init();
        wx = (lcddev.width - segsize * 2.5 - 4 * fontsize) / 2;
        wy = (lcddev.height - gui_phy.tbheight - segsize * 3 - fontsize / 2) / 2 + gui_phy.tbheight;
        hx = wx + 5 * fontsize / 2;
        hy = wy + segsize / 2;
        vx = hx;
        vy = hy + segsize + fontsize / 2;
        gui_draw_arcrectangle(wx, wy, segsize * 2.5 + fontsize * 4, 3 * segsize + fontsize / 2, wr, 1, BLACK, BLACK);
        gui_show_string("ADC", wx + fontsize / 2, hy + segsize - fontsize - segyoff, 1.5 * fontsize, fontsize, fontsize, WHITE);            /* ��ʾADC */
        gui_show_string("D", hx + segsize * 2.5 + fontsize / 2, hy + segsize - fontsize - segyoff, fontsize, fontsize, fontsize, WHITE);    /* ��ʾH */
        gui_show_string("Uin", wx + fontsize / 2, vy + segsize - fontsize - segyoff, 1.5 * fontsize, fontsize, fontsize, WHITE);            /* ��ʾADC */
        gui_show_string("V", hx + segsize * 2.5 + fontsize / 2, vy + segsize - fontsize - segyoff, fontsize, fontsize, fontsize, WHITE);    /* ��ʾH */

        while (1)
        {
            t++;

            if (t >= 10)
            {
                t = 0;
                OSSchedLock();      /* ��ֹ������� */
                adcx = adc_get_result_average(ADC_ADCX_CHY, 10);  /* ��ȡͨ��5��ת��ֵ��10��ȡƽ�� */
                OSSchedUnlock();    /* ����������� */

                if (oldadcx != adcx)    /* ��ѹ�б仯 */
                {
                    oldadcx = adcx;
                    vol = (float)adcx * (3.3 / VMETER_ADC_MAX_VAL);
                    
                    gui_phy.back_color = BLACK;
                    vmeter_show_num(hx, hy, 5, RED, segsize, oldadcx, 0);   /* ��ʾԭʼ���� */
                    adcx = vol;         /* ȡ�������� */
                    vmeter_show_num(vx, vy, 1, RED, segsize, adcx, 0);      /* ��ʾ�������� */
                    vmeter_show_7seg(vx + segsize / 2, vy, vx + segsize, vy + segsize, 0, RED, segsize, '.', 0); /* ��ʾС���� */
                    vol = vol - adcx;
                    adcx = vol * 10000;

                    if ((adcx % 10) >= 5)adcx = adcx / 10 + 1; /* �������� */
                    else adcx = adcx / 10;

                    vmeter_show_num(vx + segsize, vy, 3, RED, segsize, adcx, 1 << 7); /* ��ʾС������ */
                }
            }
            else delay_ms(10);

            if (system_task_return)break;   /* TPAD���� */
        }
    }

    gui_memin_free(f_seg);
    gui_memex_free(asc2_3618);
    //ADC1->CR &= ~(1 << 0);      /* �ر�ADת����(H750) */
    ADC1->CR2 &= ~(1 << 0);     /* �ر�ADת���� */
    return res;
}
























