/**
 ****************************************************************************************************
 * @file        common.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.3
 * @date        2022-05-26
 * @brief       APPͨ�� ����
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
 * V1.3 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "common.h"
#include "spb.h"
#include "appplay.h"
#include "calculator.h"
#include "settings.h"
#include "calendar.h"
#include "phoneplay.h"
//#include "audioplay.h"
//#include "recorder.h"
//#include "tomcatplay.h"
#include "paint.h"


/* ģʽѡ���б�Ĵ������� */
uint8_t *const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "ģʽѡ��", "ģʽ�x��", "Mode Select",
};
/* ��ʾ��Ϣ�Ĵ������� */
uint8_t *const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "��ʾ��Ϣ", "��ʾ��Ϣ", "Remind",
};
/* ���ѱ���Ĵ������� */
uint8_t *const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "�Ƿ񱣴�༭����ļ�?", "�Ƿ񱣴澎݋����ļ�?", "Do you want to save?",
};
/* ����ɾ���Ĵ������� */
uint8_t *const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM] =
{
    "ȷ��ɾ����������?", "�_���h���@�l����?", "Are you sure to delete this SMS",
};
/* �����ļ�ʧ��,��ʾ�Ƿ����SD��? ,200�Ŀ�� */
uint8_t *const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM] =
{
    "�����ļ�ʧ��,����!", "�����ļ�ʧ��,Ո�z��!", "Creat File Failed,Please Check!",
};
/* �������ֱ� */
uint8_t *const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM] =
{
    {"SD��", "SD��", "SD Card",},
    {"FLASH����", "FLASH�űP", "FLASH Disk",},
    {"U��", "U�P", "U Disk",},
};
/* app��Ҫ���ܽ������ */
uint8_t *const APP_MFUNS_CAPTION_TBL[APP_FUNS_NUM][GUI_LANGUAGE_NUM] =
{
    {"����ͼ��", "��ӈD��", "EBOOK",},
    {"�������", "���a���", "PHOTOS",},
    {"", "", "",},  /* ʱ�� */
    {"ϵͳ����", "ϵ�y�O��", "SYSTEM SET",},
    {"", "", "",},  /* ���±� */
    {"������", "�\����", "EXE",},
    {"", "", "",},  /* ��д���� */
    {"", "", "",},  /* ����� */
    {"USB����", "USB�B��", "USB",},
    {"����ͨ��", "�W�jͨ��", "ETHERNET",},
    {"���ߴ���", "�o������", "WIRELESS",},
    {"��ѧ������", "�ƌWӋ����", "Calculator",},
    {"��������", "���I�yԇ", "KEY",},
    {"LED����", "LED�yԇ", "LED",},
    {"������", "���Q��", "BEEP",},
    {"", "", "",},  /* �绰 */
    {"Ӧ������", "��������", "APPLICATIONS",},
    {"��ѹ��", "늉���", "V Meter",},
};

uint8_t *const APP_OK_PIC = "1:/SYSTEM/APP/COMMON/ok.bmp";              /* ȷ��ͼ�� */
uint8_t *const APP_CANCEL_PIC = "1:/SYSTEM/APP/COMMON/cancel.bmp";      /* ȡ��ͼ�� */
uint8_t *const APP_UNSELECT_PIC = "1:/SYSTEM/APP/COMMON/unselect.bmp";  /* δѡ��ͼ�� */
uint8_t *const APP_SELECT_PIC = "1:/SYSTEM/APP/COMMON/select.bmp";      /* ѡ��ͼ�� */
uint8_t *const APP_VOL_PIC = "1:/SYSTEM/APP/COMMON/VOL.bmp";            /* ����ͼƬ·�� */

/**
 * PC2LCD2002����ȡģ����:����ʽ,˳��(��λ��ǰ),����.C51��ʽ.
 * ��������:
 * ���������:ASCII��+��(' '+95)(28~60����)
 * ���������:ASCII��+��(' '+95)(72~144����,�����ʽ:BatangChe)
 * ��ͨ����:ASCII��
 */

uint8_t *const APP_ASCII_S14472 = "1:/SYSTEM/APP/COMMON/fonts144.fon";  /* ���������144*72����������·�� */
uint8_t *const APP_ASCII_S8844 = "1:/SYSTEM/APP/COMMON/fonts88.fon";    /* ���������88*44����������·�� */
uint8_t *const APP_ASCII_S7236 = "1:/SYSTEM/APP/COMMON/fonts72.fon";    /* ���������72*36����������·�� */
uint8_t *const APP_ASCII_S6030 = "1:/SYSTEM/APP/COMMON/fonts60.fon";    /* ���������60*30����������·�� */
uint8_t *const APP_ASCII_5427 = "1:/SYSTEM/APP/COMMON/font54.fon";      /* ��ͨ����54*27����������·�� */
uint8_t *const APP_ASCII_3618 = "1:/SYSTEM/APP/COMMON/font36.fon";      /* ��ͨ����36*18����������·�� */
uint8_t *const APP_ASCII_2814 = "1:/SYSTEM/APP/COMMON/font28.fon";      /* ��ͨ����28*14����������·�� */

uint8_t *asc2_14472 = 0;    /* ��ͨ144*72��������� */
uint8_t *asc2_8844 = 0;     /* ��ͨ����88*44��������� */
uint8_t *asc2_7236 = 0;     /* ��ͨ����72*36��������� */
uint8_t *asc2_s6030 = 0;    /* ���������60*30��������� */
uint8_t *asc2_5427 = 0;     /* ��ͨ����54*27��������� */
uint8_t *asc2_3618 = 0;     /* ��ͨ����36*18��������� */
uint8_t *asc2_2814 = 0;     /* ��ͨ����28*14��������� */



/**
 * ����ԭ�� logo ͼ��(24*24��С)
 * PCtoLCD2002ȡģ��ʽ:����,����ʽ,˳��
 */
const uint8_t APP_ALIENTEK_ICO2424[]=
{
    0x00,0xFF,0x80,0x03,0xFF,0xE0,0x06,0x00,0xF0,0x18,0x70,0x38,0x07,0xFE,0x1C,0x1C,
    0x0F,0x86,0x30,0x01,0xCE,0x4F,0xE0,0x7F,0x3F,0xF8,0x3F,0xFF,0xF0,0x7F,0xFF,0xE0,
    0xFF,0xFF,0xC1,0xFF,0xFF,0x83,0xFF,0xFE,0x07,0xFF,0x7E,0x0F,0xFE,0x7F,0x07,0xF1,
    0x3B,0xC0,0x06,0x30,0xF0,0x3C,0x18,0x3F,0xF0,0x0E,0x07,0x0C,0x0F,0x80,0x30,0x03,
    0xFF,0xE0,0x00,0xFF,0x80,0x00,0x3C,0x00,
};

/* ����ԭ�� logo ͼ��(32*32��С),ȡģ��ʽ���� */
const uint8_t APP_ALIENTEK_ICO3232[]=
{
    0x00,0x1F,0xFC,0x00,0x00,0x7F,0xFF,0x00,0x01,0xFE,0x3F,0x80,0x03,0x80,0x07,0xE0,
    0x06,0x00,0x01,0xF0,0x08,0xFF,0x80,0x78,0x03,0xFF,0xF0,0x38,0x1F,0x00,0xFC,0x1C,
    0x1C,0x00,0x1F,0x1E,0x20,0x78,0x07,0xBE,0x4F,0xFF,0x81,0xFE,0x3F,0xFF,0x80,0xFF,
    0x7F,0xFF,0x01,0xFF,0xFF,0xFE,0x03,0xFF,0xFF,0xFC,0x07,0xFF,0xFF,0xF8,0x0F,0xFF,
    0xFF,0xF0,0x1F,0xFF,0xFF,0xE0,0x3F,0xFF,0x7F,0x80,0xFF,0xFE,0x7F,0x81,0xFF,0xFC,
    0x7F,0xC0,0xFF,0xF1,0x3E,0xE0,0x1F,0x06,0x3C,0x78,0x00,0x1C,0x18,0x3F,0x00,0xF8,
    0x1C,0x0F,0xFF,0xE4,0x0F,0x01,0xFF,0x18,0x07,0x80,0x00,0x30,0x03,0xE0,0x01,0xE0,
    0x01,0xFF,0x8F,0xC0,0x00,0x7F,0xFF,0x00,0x00,0x1F,0xFC,0x00,0x00,0x03,0xC0,0x00,
};

/* ����ԭ�� logo ͼ��(48*48��С),ȡģ��ʽ���� */
const uint8_t APP_ALIENTEK_ICO4848[] =
{
    0x00,0x00,0x3F,0xFC,0x00,0x00,0x00,0x01,0xFF,0xFF,0x80,0x00,0x00,0x07,0xFF,0xFF,
    0xF0,0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x00,0x00,0x3F,0xC0,0x0F,0xFE,0x00,0x00,0x7C,
    0x00,0x01,0xFF,0x00,0x00,0xE0,0x00,0x00,0x3F,0x80,0x01,0x80,0x38,0x00,0x0F,0xC0,
    0x03,0x07,0xFF,0xE0,0x07,0xE0,0x04,0x7F,0xFF,0xF8,0x03,0xF0,0x09,0xFF,0xFF,0xFF,
    0x00,0xF0,0x03,0xF0,0x00,0xFF,0x80,0x78,0x0F,0x80,0x00,0x0F,0xC0,0x78,0x1E,0x00,
    0x00,0x03,0xF0,0xFC,0x38,0x1F,0xE0,0x00,0xF9,0xFC,0x21,0xFF,0xFE,0x00,0x7F,0xFE,
    0x07,0xFF,0xFF,0xC0,0x3F,0xFE,0x1F,0xFF,0xFF,0xC0,0x0F,0xFE,0x7F,0xFF,0xFF,0x80,
    0x1F,0xFF,0x7F,0xFF,0xFE,0x00,0x3F,0xFF,0xFF,0xFF,0xFE,0x00,0x7F,0xFF,0xFF,0xFF,
    0xF8,0x00,0xFF,0xFF,0xFF,0xFF,0xF0,0x01,0xFF,0xFF,0xFF,0xFF,0xE0,0x03,0xFF,0xFF,
    0xFF,0xFF,0xC0,0x0F,0xFF,0xFF,0xFF,0xFF,0x80,0x1F,0xFF,0xFF,0xFF,0xFF,0x00,0x3F,
    0xFF,0xFF,0x7F,0xFE,0x00,0x7F,0xFF,0xFF,0x7F,0xF8,0x00,0xFF,0xFF,0xFE,0x7F,0xF8,
    0x03,0xFF,0xFF,0xFC,0x7F,0xFC,0x01,0xFF,0xFF,0xF0,0x3F,0xFE,0x00,0x7F,0xFF,0x86,
    0x3F,0xDF,0x80,0x03,0xF8,0x1C,0x1F,0x87,0xC0,0x00,0x00,0x38,0x1F,0x03,0xF8,0x00,
    0x00,0xF0,0x0F,0x01,0xFF,0x00,0x0F,0xE0,0x0F,0x80,0x7F,0xFF,0xFF,0x90,0x07,0xC0,
    0x1F,0xFF,0xFE,0x20,0x03,0xE0,0x03,0xFF,0xF0,0x60,0x03,0xF8,0x00,0x38,0x01,0xC0,
    0x00,0xFE,0x00,0x00,0x07,0x80,0x00,0xFF,0x80,0x00,0x1F,0x00,0x00,0x3F,0xF8,0x00,
    0xFE,0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x00,0x00,0x07,0xFF,0xFF,0xF0,0x00,0x00,0x01,
    0xFF,0xFF,0xC0,0x00,0x00,0x00,0x3F,0xFE,0x00,0x00,0x00,0x00,0x07,0xE0,0x00,0x00,
};

/* α����������취 */
uint32_t g_random_seed = 1;

/**
 * @brief       α�������������
 * @param       seed : ����ֵ
 * @retval      ��
 */
void app_srand(uint32_t seed)
{
    g_random_seed = seed;
}

/**
 * @brief       ��ȡα�����
 * @param       max  : ���ֵ
 * @retval      0 ~ (max-1)�е�һ��ֵ
 */
uint32_t app_get_rand(uint32_t max)
{
    g_random_seed = g_random_seed * 22695477 + 1;
    return (g_random_seed) % max;
}

/**
 * @brief       ��ȡ����ɫ
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       ctbl            : ����ɫ���ָ��
 * @retval      ��
 */
void app_read_bkcolor(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *ctbl)
{
    uint32_t x0, y0, ccnt;
    ccnt = 0;

    for (y0 = y; y0 < y + height; y0++)
    {
        for (x0 = x; x0 < x + width; x0++)
        {
            ctbl[ccnt] = gui_phy.read_point(x0, y0); /* ��ȡ��ɫ */
            ccnt++;
        }
    }
}

/**
 * @brief       �ָ�����ɫ
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       ctbl            : ����ɫ���ָ��
 * @retval      ��
 */
void app_recover_bkcolor(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *ctbl)
{
    uint32_t x0, y0, ccnt;
    ccnt = 0;

    for (y0 = y; y0 < y + height; y0++)
    {
        for (x0 = x; x0 < x + width; x0++)
        {
            gui_phy.draw_point(x0, y0, ctbl[ccnt]); /* ��ȡ��ɫ */
            ccnt++;
        }
    }
}

/**
 * @brief       ���÷ֽ���(2��ɫ)
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       mode            : ���÷ֽ���
 *              [3]:�ұ߷ֽ���
 *              [2]:��߷ֽ���
 *              [1]:�±߷ֽ���
 *              [0]:�ϱ߷ֽ���
 * @retval      ��
 */
void app_gui_tcbar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t mode)
{
    uint16_t halfheight = height / 2;
    gui_fill_rectangle(x, y, width, halfheight, LIGHTBLUE);             /* ���ײ���ɫ(ǳ��ɫ) */
    gui_fill_rectangle(x, y + halfheight, width, halfheight, GRAYBLUE); /* ���ײ���ɫ(����ɫ) */

    if (mode & 0x01)gui_draw_hline(x, y, width, DARKBLUE);

    if (mode & 0x02)gui_draw_hline(x, y + height - 1, width, DARKBLUE);

    if (mode & 0x04)gui_draw_vline(x, y, height, DARKBLUE);

    if (mode & 0x08)gui_draw_vline(x + width - 1, y, width, DARKBLUE);
}

/**
 * @brief       �õ����ֵ�λ��
 * @param       num             : ����
 * @param       dir             : 0,�Ӹ�λ����λ;  1,�ӵ�λ����λ;
 * @retval      ���ֵ�λ��.(���Ϊ10λ)
 *              ע:0,��λ��Ϊ0λ
 */
uint8_t app_get_numlen(long long num, uint8_t dir)
{
#define MAX_NUM_LENTH       10  /* �������ֳ��� */
    uint8_t i = 0, j;
    uint8_t temp = 0;

    if (dir == 0)   /* �Ӹߵ��� */
    {
        i = MAX_NUM_LENTH - 1;

        while (1)
        {
            temp = (num / gui_pow(10, i)) % 10;

            if (temp || i == 0)break;

            i--;
        }
    }
    else            /* �ӵ͵��� */
    {
        j = 0;

        while (1)
        {
            if (num % 10)
            {
                i = app_get_numlen(num, 0);
                return i;
            }

            if (j == (MAX_NUM_LENTH - 1))break;

            num /= 10;
            j++;
        }
    }

    if (i)return i + 1;
    else if (temp)return 1;
    else return 0;
}

/**
 * @brief       ��ʾ��ɫͼ��
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       icosbase        : ����λ��
 * @param       color           : �������ɫ
 * @param       bkcolor         : ����ɫ
 * @retval      ��
 */
void app_show_mono_icos(uint16_t x, uint16_t y, uint8_t width, uint8_t height, uint8_t *icosbase, uint16_t color, uint16_t bkcolor)
{
    uint16_t rsize;
    uint16_t i, j;
    uint8_t temp;
    uint8_t t = 0;
    uint16_t x0 = x;    /* ����x��λ�� */
    rsize = width / 8 + ((width % 8) ? 1 : 0); /* ÿ�е��ֽ��� */

    for (i = 0; i < rsize * height; i++)
    {
        temp = icosbase[i];

        for (j = 0; j < 8; j++)
        {
            if (temp & 0x80)gui_phy.draw_point(x, y, color);
            else gui_phy.draw_point(x, y, bkcolor);

            temp <<= 1;
            x++;
            t++;        /* ��ȼ����� */

            if (t == width)
            {
                t = 0;
                x = x0;
                y++;
                break;
            }
        }
    }
}

/**
 * @brief       ��ʾһ��������(֧�ָ���)
 * @note        ����������Ǵ��ҵ�����ʾ��.
 * @param       x,y             : ��ʼ����
 * @param       num             : ����
 * @param       flen            : С����λ��.0,û��С������;1,��һλС��;2,����λС��;�����Դ�����.
 * @param       clen            : Ҫ�����λ��(���ǰһ�εĺۼ�)
 * @param       font            : ����
 * @param       color           : ������ɫ
 * @param       bkcolor         : ����ɫ
 * @retval      ��
 */
void app_show_float(uint16_t x, uint16_t y, long long num, uint8_t flen, uint8_t clen, uint8_t font, uint16_t color, uint16_t bkcolor)
{
    uint8_t offpos = 1;
    uint8_t ilen = 0;                       /* �������ֳ��Ⱥ�С�����ֵĳ��� */
    uint8_t negative = 0;                   /* ������� */
    uint16_t maxlen = (uint16_t)clen * (font / 2);              /* ����ĳ��� */
    gui_fill_rectangle(x - maxlen, y, maxlen, font, bkcolor);   /* ���֮ǰ�ĺۼ� */

    if (num < 0)
    {
        num = -num;
        negative = 1;
    }

    ilen = app_get_numlen(num, 0);  /* �õ���λ�� */
    gui_phy.back_color = bkcolor;

    if (flen)
    {
        gui_show_num(x - (font / 2)*flen, y, flen, color, font, num, 0X80); /* ��ʾС������ */
        gui_show_ptchar(x - (font / 2) * (flen + 1), y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, font, '.', 0); /* ��ʾС���� */
        offpos = 2 + flen;
    }

    if (ilen <= flen)ilen = 1;      /* ����Ҫ��һλ�������� */
    else
    {
        if (flen)offpos = ilen + 1;
        else offpos = ilen;

        ilen = ilen - flen;         /* �õ��������ֵĳ��� */
    }

    num = num / gui_pow(10, flen);  /* �õ��������� */
    gui_show_num(x - (font / 2)*offpos, y, ilen, color, font, num, 0X80);   /* ���0��ʾ */

    if (negative)
    {
        gui_show_ptchar(x - (font / 2) * (offpos + 1), y, gui_phy.lcdwidth, gui_phy.lcdheight, 0, color, font, '-', 0); /* ��ʾ���� */
    }
}

/**
 * @brief       �ļ����������ʾ
 * @param       topname         : �����ʱ��Ҫ��ʾ������
 * @param       mode            : ģʽ
 *              [0]:0,����ʾ�Ϸ�ɫ��;1,��ʾ�Ϸ�ɫ��
 *              [1]:0,����ʾ�·�ɫ��;1,��ʾ�·�ɫ��
 *              [2]:0,����ʾ����;1,��ʾ����
 *              [3~7]:����
 * @retval      ��
 */
void app_filebrower(uint8_t *topname, uint8_t mode)
{
    if (mode & 0X01)app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);                                  /* �·ֽ��� */

    if (mode & 0X02)app_gui_tcbar(0, lcddev.height - gui_phy.tbheight, lcddev.width, gui_phy.tbheight, 0x01);   /* �Ϸֽ��� */

    if (mode & 0X04)gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, topname);
}

/**
 * @brief       ��һ�������м���ʾ����
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       num             : Ҫ��ʾ������
 * @param       len             : λ��
 * @param       font            : ����ߴ�
 * @param       ptcolor         : ������ɫ
 * @param       bkcolor         : ����ɫ
 * @retval      ��
 */
void app_show_nummid(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t num, uint8_t len, uint8_t size, uint16_t ptcolor, uint16_t bkcolor)
{
    uint16_t numlen;
    uint8_t xoff, yoff;
    numlen = (size / 2) * len;  /* ���ֳ��� */

    if (numlen > width || size > height)return;

    xoff = (width - numlen) / 2;
    yoff = (height - size) / 2;
    g_point_color = ptcolor;
    g_back_color = bkcolor;
    lcd_show_xnum(x + xoff, y + yoff, num, len, size, 0X80, g_point_color); /* ��ʾ������� */
}

/**
 * @brief       ��һ��ƽ�����ɵĲ�ɫ��(�����)
 * @note        ���м�Ϊ���,����չ��
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       sergb           : ��ֹ��ɫ
 * @param       mrgb            : �м���ɫ
 * @retval      ��
 */
void app_draw_smooth_line(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t sergb, uint32_t mrgb)
{
    gui_draw_smooth_rectangle(x, y, width / 2, height, sergb, mrgb);                /* ǰ��ν��� */
    gui_draw_smooth_rectangle(x + width / 2, y, width / 2, height, mrgb, sergb);    /* ���ν��� */
}

/**
 * @brief       �жϴ�������ǰֵ�ǲ�����ĳ��������
 * @param       tp              : ������
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�
 * @param       mrgb            : �м���ɫ
 * @retval      0,����������;1,��������;
 */
uint8_t app_tp_is_in_area(_m_tp_dev *tp, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (tp->x[0] <= (x + width) && tp->x[0] >= x && tp->y[0] <= (y + height) && tp->y[0] >= y)return 1;
    else return 0;
}

/**
 * @brief       ��ʾ��Ŀ
 * @param       x,y             : ��ʼ����
 * @param       itemwidth       : ���
 * @param       itemheight      : �߶�
 * @param       name            : ��Ŀ����
 * @param       icopath         : ͼ��·��
 * @param       color           : ������ɫ
 * @param       bkcolor         : ����ɫ
 * @retval      ��
 */
void app_show_items(uint16_t x, uint16_t y, uint16_t itemwidth, uint16_t itemheight, uint8_t *name, uint8_t *icopath, uint16_t color, uint16_t bkcolor)
{
    gui_fill_rectangle(x, y, itemwidth, itemheight, bkcolor);   /* ��䱳��ɫ */
    gui_show_ptstr(x + 5, y + (itemheight - 16) / 2, x + itemwidth - 10 - APP_ITEM_ICO_SIZE - 5, y + itemheight, 0, color, 16, name, 1);    /* ��ʾ��Ŀ���� */
//    SCB_CleanInvalidateDCache();    /* ��cache */

    if (icopath)
    {
        minibmp_decode(icopath, x + itemwidth - 10 - APP_ITEM_ICO_SIZE, y + (itemheight - APP_ITEM_ICO_SIZE) / 2, APP_ITEM_ICO_SIZE, APP_ITEM_ICO_SIZE, 0, 0);  /* ����APP_ITEM_ICO_SIZE*APP_ITEM_ICO_SIZE��bmpͼƬ */
    }
}

/**
 * @brief       ��õ�ǰ��Ŀ��ͼ��·��
 * @param       mode            : 0,��ѡģʽ;1,��ѡģʽ;
 * @param       selpath         : ѡ��ͼ��·��
 * @param       unselpath       : ��ѡ�е�ͼ��·��
 * @param       selx            : ѡ��ͼ����(��ѡģʽ)/��Чͼ������(��ѡģʽ)
 * @param       index           : ��ǰ��Ŀ���
 * @retval      ��
 */
uint8_t *app_get_icopath(uint8_t mode, uint8_t *selpath, uint8_t *unselpath, uint8_t selx, uint8_t index)
{
    uint8_t *icopath = 0;

    if (mode)   /* ��ѡģʽ */
    {
        if (selx & (1 << index))icopath = selpath;  /* ��ѡ�е���Ŀ */
        else icopath = unselpath;                   /* �Ƿ�ѡ�е���Ŀ */
    }
    else
    {
        if (selx == index)icopath = selpath;        /* ��ѡ�е���Ŀ */
        else icopath = unselpath;                   /* �Ƿ�ѡ�е���Ŀ */
    }

    return icopath;
}

/**
 * @brief       ��Ŀѡ��
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ��Ⱥ͸߶�(width��СΪ150,height��СΪ72)
 * @param       items[]         : ��Ŀ���ּ�
 * @param       itemsize        : ����Ŀ��(��󲻳���8��)
 * @param       selx            : ���.��ѡģʽʱ,��Ӧ�����ѡ�����.��ѡģʽʱ,��Ӧѡ�����Ŀ.
 * @param       mode            : ģʽ
 *              [7]:0,��OK��ť;1,��OK��ť
 *              [6]:0,����ȡ����ɫ;1,��ȡ����ɫ
 *              [5]:0,��ѡģʽ;1,��ѡģʽ
 *              [4]:0,������ͼ��;1,����ͼ��
 *              [3:0]:����
 * @param       caption         : ��������
 * @retval      0,ok; ����,ȡ�����ߴ���;
 */
uint8_t app_items_sel(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *items[], uint8_t itemsize, uint8_t *selx, uint8_t mode, uint8_t *caption)
{
    uint8_t rval = 0, res;
    uint8_t selsta = 0;     /**
                             * ѡ��״̬Ϊ0,
                             * [7]:����Ƿ��Ѿ���¼��һ�ΰ��µ���Ŀ;
                             * [6:4]:����
                             * [3:0]:��һ�ΰ��µ���Ŀ
                             */
    uint16_t i;

    uint8_t temp;
    uint16_t itemheight = 0;    /* ÿ����Ŀ�ĸ߶� */
    uint16_t itemwidth = 0;     /* ÿ����Ŀ�Ŀ�� */
    uint8_t *unselpath = 0;     /* δѡ�е�ͼ���·�� */
    uint8_t *selpath = 0;       /* ѡ��ͼ���·�� */
    uint8_t *icopath = 0;

    _window_obj *twin = 0;      /* ���� */
    _btn_obj *okbtn = 0;        /* ȷ����ť */
    _btn_obj *cancelbtn = 0;    /* ȡ����ť */

    if (itemsize > 8 || itemsize < 1)return 0xff;   /* ��Ŀ������ */

    if (width < 150 || height < 72)return 0xff;     /* �ߴ���� */

    itemheight = (height - 72) / itemsize - 1;      /* �õ�ÿ����Ŀ�ĸ߶� */
    itemwidth = width - 10;                         /* ÿ����Ŀ�Ŀ�� */

    twin = window_creat(x, y, width, height, 0, 1 | (1 << 5) | ((1 << 6)&mode), 16);        /* �������� */

    if (twin == NULL)
    {
        twin = window_creat(x, y, width, height, 0, 1 | (1 << 5) | ((1 << 6)&mode), 16);    /* ���´������� */
    }

    if (mode & (1 << 7))
    {
        temp = (width - APP_ITEM_BTN1_WIDTH * 2) / 3;
        okbtn = btn_creat(x + temp, y + height - APP_ITEM_BTN_HEIGHT - 5, APP_ITEM_BTN1_WIDTH, APP_ITEM_BTN_HEIGHT, 0, 0x02);   /* ����OK��ť */
        cancelbtn = btn_creat(x + APP_ITEM_BTN1_WIDTH + temp * 2, y + height - APP_ITEM_BTN_HEIGHT - 5, APP_ITEM_BTN1_WIDTH, APP_ITEM_BTN_HEIGHT, 0, 0x02); /* ����CANCEL��ť */

        if (twin == NULL || okbtn == NULL || cancelbtn == NULL)rval = 1;
        else
        {
            okbtn->caption = (uint8_t *)GUI_OK_CAPTION_TBL[gui_phy.language]; /* ȷ�� */
            okbtn->bkctbl[0] = 0X8452;  /* �߿���ɫ */
            okbtn->bkctbl[1] = 0XAD97;  /* ��һ�е���ɫ */
            okbtn->bkctbl[2] = 0XAD97;  /* �ϰ벿����ɫ */
            okbtn->bkctbl[3] = 0X8452;  /* �°벿����ɫ */
        }
    }
    else
    {
        temp = (width - APP_ITEM_BTN2_WIDTH) / 2;
        cancelbtn = btn_creat(x + temp, y + height - APP_ITEM_BTN_HEIGHT - 5, APP_ITEM_BTN2_WIDTH, APP_ITEM_BTN_HEIGHT, 0, 0x02);   /* ����CANCEL��ť */

        if (twin == NULL || cancelbtn == NULL)rval = 1;
    }

    if (rval == 0)   /* ֮ǰ�Ĳ������� */
    {
        twin->caption = caption;
        twin->windowbkc = APP_WIN_BACK_COLOR;
        cancelbtn->caption = (uint8_t *)GUI_CANCEL_CAPTION_TBL[gui_phy.language]; /* ȡ�� */
        cancelbtn->bkctbl[0] = 0X8452;  /* �߿���ɫ */
        cancelbtn->bkctbl[1] = 0XAD97;  /* ��һ�е���ɫ */
        cancelbtn->bkctbl[2] = 0XAD97;  /* �ϰ벿����ɫ */
        cancelbtn->bkctbl[3] = 0X8452;  /* �°벿����ɫ */

        if (mode & (1 << 4))            /* ��Ҫ����ͼ�� */
        {
            if (mode & (1 << 5))        /* ��ѡģʽ */
            {
                unselpath = (uint8_t *)APP_CANCEL_PIC;      /* δѡ�е�ͼ���·�� */
                selpath = (uint8_t *)APP_OK_PIC;            /* ѡ��ͼ���·�� */
            }
            else     /* ��ѡģʽ */
            {
                unselpath = (uint8_t *)APP_UNSELECT_PIC;    /* δѡ�е�ͼ���·�� */
                selpath = (uint8_t *)APP_SELECT_PIC;        /* ѡ��ͼ���·�� */
            }
        }

        window_draw(twin);      /* �������� */
        btn_draw(cancelbtn);    /* ����ť */

        if (mode & (1 << 7))btn_draw(okbtn);	/* ����ť */

        for (i = 0; i < itemsize; i++)
        {
            icopath = app_get_icopath(mode & (1 << 5), selpath, unselpath, *selx, i); /* �õ�ͼ��·�� */
            app_show_items(x + 5, y + 32 + i * (itemheight + 1), itemwidth, itemheight, items[i], icopath, BLACK, twin->windowbkc); /* ��ʾ���е���Ŀ */

            if ((i + 1) != itemsize)app_draw_smooth_line(x + 5, y + 32 + (i + 1) * (itemheight + 1) - 1, itemwidth, 1, 0Xb1ffc4, 0X1600b1); /* ������ */
        }

        while (rval == 0)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

            if (system_task_return)                 /* TPAD���� */
            {
                rval = 1;
                break;
            }

            if (mode & (1 << 7))
            {
                res = btn_check(okbtn, &in_obj);    /* ȷ�ϰ�ť��� */

                if (res)
                {
                    if ((okbtn->sta & 0X80) == 0)   /* ����Ч���� */
                    {
                        rval = 0XFF;
                        break;/* ȷ�ϰ�ť */
                    }
                }
            }

            res = btn_check(cancelbtn, &in_obj);    /* ȡ����ť��� */

            if (res)
            {
                if ((cancelbtn->sta & 0X80) == 0)   /* ����Ч���� */
                {
                    rval = 1;
                    break;  /* ȡ����ť */
                }
            }

            temp = 0XFF; /* �����,���Ϊ0XFF,���ɿ���ʱ��,˵���ǲ�����Ч�����ڵ�.�����0XFF,���ʾTP�ɿ���ʱ��,������Ч������ */

            for (i = 0; i < itemsize; i++)
            {
                if (tp_dev.sta & TP_PRES_DOWN)      /* ������������ */
                {
                    if (app_tp_is_in_area(&tp_dev, x + 5, y + 32 + i * (itemheight + 1), itemwidth, itemheight))   /* �ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ�������� */
                    {
                        if ((selsta & 0X80) == 0)   /* ��û�а��¹� */
                        {
                            icopath = app_get_icopath(mode & (1 << 5), selpath, unselpath, *selx, i); /* �õ�ͼ��·�� */
                            app_show_items(x + 5, y + 32 + i * (itemheight + 1), itemwidth, itemheight, items[i], icopath, BLACK, APP_ITEM_SEL_BKCOLOR); /* ��ѡ��Ŀ */

                            selsta = i;             /* ��¼��һ�ΰ��µ���Ŀ */
                            selsta |= 0X80;         /* ����Ѿ����¹��� */
                        }

                        break;
                    }
                }
                else     /* ���������ɿ��� */
                {
                    if (app_tp_is_in_area(&tp_dev, x + 5, y + 32 + i * (itemheight + 1), itemwidth, itemheight))   /* �ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ�������� */
                    {
                        temp = i;
                        break;
                    }
                }
            }

            if ((selsta & 0X80) && (tp_dev.sta & TP_PRES_DOWN) == 0)   /* �а��¹�,�Ұ����ɿ��� */
            {
                if ((selsta & 0X0F) == temp)    /* �ɿ�֮ǰ������Ҳ���ڰ���ʱ�������� */
                {
                    if (mode & (1 << 5))        /* ��ѡģʽ,ִ��ȡ������ */
                    {
                        if ((*selx) & (1 << temp))*selx &= ~(1 << temp);
                        else *selx |= 1 << temp;
                    }
                    else     /* ��ѡģʽ */
                    {
                        app_show_items(x + 5, y + 32 + (*selx) * (itemheight + 1), itemwidth, itemheight, items[*selx], unselpath, BLACK, twin->windowbkc); /* ȡ��֮ǰѡ�����Ŀ */
                        *selx = temp;
                    }
                }
                else temp = selsta & 0X0F;   /* �õ���ʱ���µ���Ŀ�� */

                icopath = app_get_icopath(mode & (1 << 5), selpath, unselpath, *selx, temp); /* �õ�ͼ��·�� */
                app_show_items(x + 5, y + 32 + temp * (itemheight + 1), itemwidth, itemheight, items[temp], icopath, BLACK, twin->windowbkc); /* ��ѡ��Ŀ */
                selsta = 0; /* ȡ�� */
            }
        }
    }


    window_delete(twin);
    btn_delete(okbtn);
    btn_delete(cancelbtn);
    system_task_return = 0;

    if (rval == 0XFF)return 0;

    return rval;
}

/**
 * @brief       �½�ѡ���
 * @param       sel             : ��ǰѡ�е���Ŀ
 * @param       top             : ��ǰ�������Ŀ
 * @param       caption         : ѡ�������
 * @param       items[]         : ��Ŀ���ּ�
 * @param       itemsize        : ����Ŀ��
 * @retval      �������
 *              [7]:0,���µ��Ƿ��ذ���,���߷����˴���;1,������ȷ�ϰ���,����˫����ѡ�е���Ŀ.
 *              [6:0]:0,��ʾ�޴���;����,�������.
 */
uint8_t app_listbox_select(uint8_t *sel, uint8_t *top, uint8_t *caption, uint8_t *items[], uint8_t itemsize)
{
    uint8_t  res;
    uint8_t rval = 0;       /* ����ֵ */
    uint16_t i;

    _btn_obj *rbtn = 0;     /* ���ذ�ť�ؼ� */
    _btn_obj *okbtn = 0;    /* ȷ�ϰ�ť�ؼ� */
    _listbox_obj *tlistbox; /* listbox */

    if (*sel >= itemsize || *top >= itemsize)return 2; /* ��������/�����Ƿ� */

    app_filebrower(caption, 0X07);  /* ��ʾ���� */
    tlistbox = listbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.tbfsize); /* ����һ��filelistbox */

    if (tlistbox == NULL)rval = 1;  /* �����ڴ�ʧ�� */
    else    /* �����Ŀ */
    {
        for (i = 0; i < itemsize; i++)
        {
            res = listbox_addlist(tlistbox, items[i]);

            if (res)
            {
                rval = 1;
                break;
            }
        }
    }

    if (rval == 0)   /* �ɹ��������Ŀ */
    {
        tlistbox->scbv->topitem = *top;
        tlistbox->selindex = *sel;
        listbox_draw_listbox(tlistbox);
        rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */
        okbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* ����ȷ�����ְ�ť */

        if (rbtn == NULL || okbtn == NULL)rval = 1; /* û���㹻�ڴ湻���� */
        else
        {
            rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language];  /* ���� */
            rbtn->font = gui_phy.tbfsize; /* �����µ������С */
            rbtn->bcfdcolor = WHITE;    /* ����ʱ����ɫ */
            rbtn->bcfucolor = WHITE;    /* �ɿ�ʱ����ɫ */
            btn_draw(rbtn);             /* ����ť */


            okbtn->caption = (uint8_t *)GUI_OK_CAPTION_TBL[gui_phy.language];   /* ���� */
            okbtn->font = gui_phy.tbfsize; /* �����µ������С */
            okbtn->bcfdcolor = WHITE;   /* ����ʱ����ɫ */
            okbtn->bcfucolor = WHITE;   /* �ɿ�ʱ����ɫ */
            btn_draw(okbtn);            /* ����ť */
        }
    }

    while (rval == 0)
    {
        tp_dev.scan(0);
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);	/* �õ�������ֵ */
        delay_ms(1000 / OS_TICKS_PER_SEC);  /* ��ʱһ��ʱ�ӽ��� */

        if (system_task_return)break;       /* TPAD���� */

        res = btn_check(rbtn, &in_obj);     /* ���ذ�ť��� */

        if (res)if (((rbtn->sta & 0X80) == 0))break;/* �˳� */

        res = btn_check(okbtn, &in_obj);    /* ȷ�ϰ�ť��� */

        if (res)
        {
            if (((okbtn->sta & 0X80) == 0))   /* ��ť״̬�ı��� */
            {
                *top = tlistbox->scbv->topitem; /* ��¼�˳�ʱѡ�����Ŀ */
                *sel = tlistbox->selindex;
                rval |= 1 << 7;             /* ���˫�������� */
            }
        }

        listbox_check(tlistbox, &in_obj);   /* ɨ�� */

        if (tlistbox->dbclick == 0X80)      /* ˫���� */
        {
            *top = tlistbox->scbv->topitem; /* ��¼�˳�ʱѡ�����Ŀ */
            *sel = tlistbox->selindex;
            rval |= 1 << 7;     /* ���˫�������� */
        }
    }

    listbox_delete(tlistbox);   /* ɾ��listbox */
    btn_delete(okbtn);          /* ɾ����ť */
    btn_delete(rbtn);           /* ɾ����ť */
    return rval;
}

/**
 * @brief       ��������
 * @param       x,y             : ��ʼ����
 * @param       widht,height    : ��Ⱥ͸߶�,��ȱ������100,�߶ȱ������90
 *                                ����widht:�����ַ�����*6+20,��width>100
 *                                ����height:������Ŀ��*16+90
 * @param       caption         : ����
 * @param       str             : �ַ���(��Ŀ֮���Իس���:'\r'����,��Ŀ�߶ȹ̶�Ϊ16)
 * @retval      ��
 */
void app_muti_remind_msg(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *caption, uint8_t *str)
{
    _window_obj *twin = 0;  /* ���� */
    _btn_obj *qbtn = 0;     /* ȷ�ϰ�ť */
    uint8_t rval = 0;
    uint8_t *p, *p1;
    uint8_t items = 1;
    uint8_t i;
    uint8_t *tbuf;

    if (str == NULL)return ;

    p = p1 = str;

    while (1)   /* ȷ���ж��ٸ���Ŀ(�س���) */
    {
        p = (uint8_t *)strstr((char *)p1, "\r");    /* ���һ��� */

        if (p == NULL)break;

        p1 = p + 1;
        items++;
    }

    twin = window_creat(x, y, width, height, 0, 1 | 1 << 5, 16);    /* �������� */
    tbuf = mymalloc(SRAMIN, strlen((char *)str));                   /* �����ڴ� */
    qbtn = btn_creat(x + (width - APP_ITEM_BTN2_WIDTH) / 2, y + 40 + 16 * items + 10, APP_ITEM_BTN2_WIDTH, APP_ITEM_BTN_HEIGHT, 0, 0x02); /* ������ť */

    if (twin && qbtn && tbuf)
    {
        /* ���ڵ����ֺͱ���ɫ */
        twin->caption = caption;
        twin->windowbkc = APP_WIN_BACK_COLOR;
        /* ���ذ�ť����ɫ */
        qbtn->bkctbl[0] = 0X8452;   /* �߿���ɫ */
        qbtn->bkctbl[1] = 0XAD97;   /* ��һ�е���ɫ */
        qbtn->bkctbl[2] = 0XAD97;   /* �ϰ벿����ɫ */
        qbtn->bkctbl[3] = 0X8452;   /* �°벿����ɫ */
        qbtn->caption = (uint8_t *)GUI_OK_CAPTION_TBL[gui_phy.language]; /* ����Ϊȷ�� */

        window_draw(twin);          /* �������� */
        btn_draw(qbtn);             /* ������ */
        gui_phy.back_color = APP_WIN_BACK_COLOR;
        strcpy((char *)tbuf, (char *)str);
        p = p1 = tbuf;

        for (i = 0; i < items; i++)
        {
            p = (uint8_t *)strstr((char *)p1, "\r"); /* ���һ��� */

            if (p)*p = 0; /* ��ӽ����� */

            gui_show_ptstr(x + 10, y + 32 + 8 + 16 * i, x + width - 10, y + 32 + 8 + 16 * i + 16, 0, BLACK, 12, p1, 0); /* ��ʾ���� */
            p1 = p + 1;
        }

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

            if (system_task_return)break;           /* TPAD���� */

            rval = btn_check(qbtn, &in_obj);        /* ���ذ�ť��� */

            if (rval && (qbtn->sta & 0X80) == 0)break;  /* ����Ч���� */
        }
    }

    window_delete(twin);
    btn_delete(qbtn);
    myfree(SRAMIN, tbuf);
    system_task_return = 0;     /* ȡ��TPAD */
}

/**
 * @brief       ������·����Ҫ���޸�
 * @param       des             : Ŀ�껺����
 * @param       diskx           : �µĴ��̱��
 * @param       src             : ԭ·��
 * @retval      ��
 */
void app_system_file_chgdisk(uint8_t *des, uint8_t *diskx, uint8_t *src)
{
    //static uint8_t t=0;
    strcpy((char *)des, (const char *)src);
    des[0] = diskx[0];
    //printf("path[%d]:%s\r\n",t++,des);    /* ��ӡ�ļ��� */
}

/**
 * @brief       �����������ĳ���ļ�
 * @param       diskx           : ���̱��
 * @param       fname           : �ļ���(��·��)
 * @retval      0,����; 1,�ڴ����;
 */
uint8_t app_file_check(uint8_t *diskx, uint8_t *fname)
{
    FIL *f_check;
    uint8_t *path;
    uint8_t res = 0;
    f_check = (FIL *)gui_memin_malloc(sizeof(FIL)); /* ����FIL�ֽڵ��ڴ����� */
    path = gui_memin_malloc(200);   /* Ϊpath�����ڴ� */

    if (!f_check || !path)   /* ����ʧ�� */
    {
        gui_memin_free(f_check);
        gui_memin_free(path);
        return 1;
    }

    app_system_file_chgdisk(path, diskx, fname);    /* �޸�·�� */
    res = f_open(f_check, (const TCHAR *)path, FA_READ);    /* ֻ����ʽ���ļ� */

    f_close(f_check);           /* �ر��ļ� */
    gui_memin_free(f_check);
    gui_memin_free(path);
    return res;
}

/**
 * @brief       ϵͳ�ܹ���Ҫ��ͼ��/ͼƬ/ϵͳ�ļ� �ȼ��
 * @note        ������е�ϵͳ�ļ��Ƿ�����
 * @param       diskx           : ���̱��
 * @retval      0,����; ����,�������;
 */
uint8_t app_system_file_check(uint8_t *diskx)
{
    uint8_t i, j;
    uint8_t rval = 0;
    uint8_t res = 0;

    while (1)
    {
        /* ���9��SPB����ͼƬ */
        for (j = 0; j < 3; j++)
        {
            for (i = 0; i < 4; i++)
            {
                res = app_file_check(diskx, spb_bkpic_path_tbl[j][i]);

                if (res)break;  /* ������ */

                rval++;
            }

            if (res)break;      /* ������ */
        }

        if (rval != 12)break;
        rval -= 3;              /* ʵ��ֻ��9��ͼ�� */

        /* ��⻬������ͼ�����(3�����͵���Ļ,ÿ���SPB_ICOS_NUM��) */
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < SPB_ICOS_NUM; j++)
            {
                res = app_file_check(diskx, spb_icos_path_tbl[i][j]);

                if (res)break;  /* ������ */

                rval++;
            }
        }

        if (rval != 9 + 3 * SPB_ICOS_NUM)break;

        /* ���3��SPB��ͼ��,3�����͵���Ļ,�ܹ�9�� */
        for (j = 0; j < 3; j++)
        {
            for (i = 0; i < 3; i++)
            {
                res = app_file_check(diskx, spb_micos_path_tbl[j][i]);

                if (res)break;  /* ������ */

                rval++;
            }

            if (res)break;
        }

        if (j != 3 && i != 3)break;

        /* APP/COMMON�ļ����� 12���ļ���� */
        res = app_file_check(diskx, APP_OK_PIC);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_CANCEL_PIC);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_UNSELECT_PIC);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_SELECT_PIC);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_VOL_PIC);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_S14472);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_S8844);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_S7236);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_S6030);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_5427);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_3618);

        if (res)break;  /* ������ */

        rval++;
        res = app_file_check(diskx, APP_ASCII_2814);

        if (res)break;  /* ������ */

        rval++;

//        /* APP/AUDIO �ļ�����15���ļ���� */
//        for (j = 0; j < 2; j++)
//        {
//            for (i = 0; i < 5; i++)
//            {
//                res = app_file_check(diskx, AUDIO_BTN_PIC_TBL[j][i]);

//                if (res)break;  /* ������ */

//                rval++;
//            }

//            if (res)break;
//        }

//        if (j != 2 && i != 5)break;

//        for (j = 0; j < 3; j++)
//        {
//            res = app_file_check(diskx, AUDIO_BACK_PIC[j]);

//            if (res)break;  /* ��ʧ�� */

//            rval++;
//        }
//        
//        if (j != 3)break;   /* ������ */
//        
//        res = app_file_check(diskx, AUDIO_PAUSER_PIC);

//        if (res)break;  /* ������ */

//        rval++;
//        res = app_file_check(diskx, AUDIO_PAUSEP_PIC);

//        if (res)break;  /* ������ */

//        rval++;
        
        /* APP/PHONE �ļ�����9���ļ���� */
        for (j = 0; j < 3; j++)
        {
            res = app_file_check(diskx, PHONE_CALL_PIC[j]);

            if (res)break;  /* ������ */

            rval++;
        }

        if (j != 3)break;

        for (j = 0; j < 3; j++)
        {
            res = app_file_check(diskx, PHONE_HANGUP_PIC[j]);

            if (res)break;  /* ������ */

            rval++;
        }

        if (j != 3)break;

        for (j = 0; j < 3; j++)
        {
            res = app_file_check(diskx, PHONE_DEL_PIC[j]);

            if (res)break;  /* ������ */

            rval++;
        }

        if (j != 3)break;

//        /* APP/RECORDER �ļ�����7���ļ���� */
//        res = app_file_check(diskx, RECORDER_DEMO_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        res = app_file_check(diskx, RECORDER_RECR_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        res = app_file_check(diskx, RECORDER_RECP_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        res = app_file_check(diskx, RECORDER_PAUSER_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        res = app_file_check(diskx, RECORDER_PAUSEP_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        res = app_file_check(diskx, RECORDER_STOPR_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        res = app_file_check(diskx, RECORDER_STOPP_PIC); 

//        if (res)break;  /* ��ʧ�� */

//        rval++;
//        
//        /* APP/TOMCAT �ļ�����1���ļ���� */
//        res = app_file_check(diskx, TOMCAT_DEMO_PIC); 

//        if (res)break;  /* ��ʧ�� */
//        
//        rval++;
//         
//        /* APP/PAINT �ļ�����1���ļ���� */
//        res = app_file_check(diskx, PAINT_COLOR_TBL_PIC);

//        if (res)break;          /* ������ */

//        rval++;
         
       /* ���Ӧ�ó���48��ͼ�� */
        for (j = 0; j < 3; j++)
        {
            for (i = 0; i < 16; i++)
            {
                res = app_file_check(diskx, appplay_icospath_tbl[j][i]);

                if (res)break;  /* ������ */

                rval++;
            }

            if (res)break;
        }

        if (j != 3 && i != 16)break;

        printf("\r\ntotal system files:%d\r\n", rval);
        rval = 0;
        break;
    }

    return rval + res;
}

/****************************************************************************************************/
/* ���´��������ʵ��SYSTEM�ļ��и��� */
uint8_t *const APP_SYSTEM_APPPATH = "/SYSTEM/APP";      /* APP�ļ���·�� �� */
uint8_t *const APP_SYSTEM_SPBPATH = "/SYSTEM/SPB";      /* SPB�ļ���·�� */
uint8_t *const APP_SYSTEM_DSTPATH = "1:/SYSTEM";        /* ϵͳ�ļ�Ŀ��·�� */

uint8_t *const APP_TESTFILE_SRCPATH = "/�������ļ�";     /* �����ļ�Դ·�� */
uint8_t *const APP_TESTFILE_DSTPATH = "1:";             /* �����ļ�Ŀ��·�� */


/* �ļ�������Ϣ��ʾ��������� */
static uint16_t cpdmsg_x;
static uint16_t cpdmsg_y;
static uint8_t  cpdmsg_fsize;


 /**
 * @brief       ϵͳ������ʱ��,������ʾ���½���
 * @param       pname           : �����ļ�����
 * @param       pct             : �ٷֱ�
 * @param       mode            : ģʽ
 *              [0]:�����ļ���
 *              [1]:���°ٷֱ�pct
 *              [2]:�����ļ���
 *              [3~7]:����
 * @retval      0,����; 1,��������;
 */
uint8_t app_boot_cpdmsg(uint8_t *pname, uint8_t pct, uint8_t mode)
{
    if (mode & 0X01)
    {
        lcd_fill(cpdmsg_x + 9 * (cpdmsg_fsize / 2), cpdmsg_y, cpdmsg_x + (9 + 16) * (cpdmsg_fsize / 2), cpdmsg_y + cpdmsg_fsize, BLACK);    /* ����ɫ */
        lcd_show_string(cpdmsg_x + 9 * (cpdmsg_fsize / 2), cpdmsg_y, 16 * (cpdmsg_fsize / 2), cpdmsg_fsize, cpdmsg_fsize, (char *)pname, g_point_color);    /* ��ʾ�ļ���,�16���ַ���� */
        printf("\r\nCopy File:%s\r\n", pname);
    }

    if (mode & 0X04)printf("Copy Folder:%s\r\n", pname);

    if (mode & 0X02) /* ���°ٷֱ� */
    {
        lcd_show_string(cpdmsg_x + (9 + 16 + 1 + 3) * (cpdmsg_fsize / 2), cpdmsg_y, lcddev.width, lcddev.height, cpdmsg_fsize, "%", g_point_color);
        lcd_show_num(cpdmsg_x + (9 + 16 + 1) * (cpdmsg_fsize / 2), cpdmsg_y, pct, 3, cpdmsg_fsize, g_point_color); /* ��ʾ��ֵ */
        printf("File Copyed:%d\r\n", pct);
    }

    return 0;
}

/**
 * @brief       ����app_boot_cpdmsg����ʾ����
 * @param       x,y             : ��ʼ����
 * @param       fisze           : �����С
 * @retval      ��
 */
void app_boot_cpdmsg_set(uint16_t x, uint16_t y, uint8_t fsize)
{
    cpdmsg_x = x;
    cpdmsg_y = y;
    cpdmsg_fsize = fsize;
}

/**
 * @brief       ϵͳ����
 * @param       fcpymsg         : �ٷֱ���ʾ����
 * @param       src             : �ļ���Դ,"0:",SD��;"2:",U��.
 * @retval      0,����; 0XFF,ǿ���˳�; ����,����; 
 */
uint8_t app_system_update(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *src)
{
    uint32_t totsize = 0;
    uint32_t cpdsize = 0;
    uint8_t res = 0;
    uint8_t *p_app;
    uint8_t *p_spb;
    uint8_t *p_test;
    uint8_t rval;
    p_app = gui_memin_malloc(100);
    p_spb = gui_memin_malloc(100);
    p_test = gui_memin_malloc(100);

    if (p_test == NULL)
    {
        gui_memin_free(p_app);
        gui_memin_free(p_spb);
        gui_memin_free(p_test);
        return 1;/* �ڴ�����ʧ�� */
    }

    sprintf((char *)p_app, "%s%s", src, APP_SYSTEM_APPPATH);
    sprintf((char *)p_spb, "%s%s", src, APP_SYSTEM_SPBPATH);
    sprintf((char *)p_test, "%s%s", src, APP_TESTFILE_SRCPATH);

    totsize += exfuns_get_folder_size(p_app);   /* �õ�3���ļ��е��ܴ�С */
    totsize += exfuns_get_folder_size(p_spb);
    totsize += exfuns_get_folder_size(p_test);
    printf("totsize:%d\r\n", totsize);

    f_mkdir((const TCHAR *)APP_SYSTEM_DSTPATH); /* ǿ�ƴ���Ŀ���ļ��� */
    res = exfuns_folder_copy(fcpymsg, p_app, (uint8_t *)APP_SYSTEM_DSTPATH, &totsize, &cpdsize, 1);

    if (res)
    {
        if (res != 0xff)rval = 1;   /* ��һ�׶θ��Ƴ��� */
        else rval = 0XFF;           /* ǿ���˳� */
    }
    else        /* ��һ�׶�OK,�����ڶ��׶� */
    {
        res = exfuns_folder_copy(fcpymsg, p_spb, (uint8_t *)APP_SYSTEM_DSTPATH, &totsize, &cpdsize, 1);

        if (res)
        {
            if (res != 0xff)rval = 2;   /* �ڶ��׶θ��Ƴ��� */
            else rval = 0XFF;           /* ǿ���˳� */
        }
        else    /* �ڶ��׶�OK,���������׶� */
        {
            res = exfuns_folder_copy(fcpymsg, p_test, (uint8_t *)APP_TESTFILE_DSTPATH, &totsize, &cpdsize, 1);

            if (res)
            {
                if (res != 0xff)rval = 3;   /* �����׶θ��Ƴ��� */
                else rval = 0XFF;           /* ǿ���˳� */
            }
            else
            {
                rval = 0;   /* �����׶η�ǿ���ԣ����������޴���,��ǿ���˳� */
            }
        }
    }

    gui_memin_free(p_app);
    gui_memin_free(p_spb);
    gui_memin_free(p_test);
    return rval;
}

/**
 * @brief       �õ�STM32�����к�
 * @param       sn0,sn1,sn2     : 3���������к�
 * @retval      ��
 */
void app_getstm32_sn(uint32_t *sn0, uint32_t *sn1, uint32_t *sn2)
{
    *sn0 = *(volatile uint32_t *)(0x1FFFF7E8);
    *sn1 = *(volatile uint32_t *)(0x1FFFF7EC);
    *sn2 = *(volatile uint32_t *)(0x1FFFF7F0);
}

/**
 * @brief       ��ӡSN
 * @param       ��
 * @retval      ��
 */
void app_usmart_getsn(void)
{
    uint32_t sn0, sn1, sn2;
    app_getstm32_sn(&sn0, &sn1, &sn2);
    printf("\r\nSerial Number:%X%X%X\r\n", sn0, sn1, sn2);
}

/**
 * @brief       �õ��汾��Ϣ
 * @param       buf             : �汾������(������Ҫ6���ֽ�,�籣��:V1.00)
 * @param       ver             : �汾�궨��
 * @param       len             : �汾λ��.1,����ֻ��1��λ�İ汾,V1;2,����2��λ�汾,V1.0;3,����3��λ�汾,V1.00;
 * @retval      ��
 */
void app_get_version(uint8_t *buf, uint32_t ver, uint8_t len)
{
    uint8_t i = 0;
    buf[i++] = 'V';

    if (len == 1)
    {
        buf[i++] = ver % 10 + '0';
    }
    else if (len == 2)
    {
        buf[i++] = (ver / 10) % 10 + '0';
        buf[i++] = '.';
        buf[i++] = ver % 10 + '0';
    }
    else
    {
        buf[i++] = (ver / 100) % 10 + '0';
        buf[i++] = '.';
        buf[i++] = (ver / 10) % 10 + '0';
        buf[i++] = ver % 10 + '0';
    }

    buf[i] = 0; /* ��������� */
}

/**
 * @brief       ��ʼ������ϵͳ����
 * @note        ����ϵͳ����,��������,ES8388���ݵ�
 * @param       ��
 * @retval      ��
 */
uint8_t app_system_parameter_init(void)
{
    sysset_read_para(&systemset);   /* ��ȡϵͳ������Ϣ */
//    vs10xx_read_para(&vsset);       /* ��ȡVS10XX�������� */
    calendar_read_para(&alarm);     /* ��ȡ������Ϣ */

    if (systemset.saveflag != 0X0A || systemset.syslanguage > 2) /* ֮ǰû�б����/������Ϣ���� */
    {
        systemset.syslanguage = 0;  /* Ĭ��Ϊ�������� */
        systemset.lcdbklight = 100; /* ����Ĭ������ */
        systemset.picmode = 0;      /* ͼƬ˳�򲥷� */
        systemset.videomode = 0;    /* ��Ƶ����Ĭ���ǰ�˳�򲥷� */
        systemset.saveflag = 0X0A;  /* ���ñ����� */
        sysset_save_para(&systemset);   /* ����ϵͳ������Ϣ */
    }
    
//    if (vsset.saveflag != 0X0A)     /* ֮ǰû�б���� */
//    {
//        vsset.mvol = 220;           /* �������� */
//        vsset.bflimit = 6;
//        vsset.bass = 15;
//        vsset.tflimit = 10;
//        vsset.treble = 15;
//        vsset.effect = 0;
//        vsset.speakersw = 1;        /* ���ȿ��� */
//        vsset.saveflag = 0X0A;      /* ���ñ����� */
//        vs10xx_save_para(&vsset);   /* ����VS1053��������Ϣ */
//    }
    
    if (alarm.saveflag != 0X0A)     /* ֮ǰû�б���� */
    {
        alarm.weekmask = 0X3E;      /* ������������Ϊ��1~5���� */
        alarm.ringsta = 3;          /* ����״̬��������־Ϊ3 */
        alarm.hour = 6;             /* ����ʱ������Ϊ6:00 */
        alarm.min = 0;
        alarm.saveflag = 0X0A;      /* ���ñ����� */
        calendar_save_para(&alarm); /* ��������������Ϣ */
    }

    gui_phy.language = systemset.syslanguage;   /* �������� */
    tim8_ch2n_pwm_init(100, 72 - 1);            /* ��ʼ��TIM8_CH2N PWM���,��ΪLCD�ı������ */

    if (systemset.lcdbklight == 0)app_lcd_auto_bklight();   /* �Զ����� */
    else if (lcddev.id == 0X1963)lcd_ssd_backlight_set(systemset.lcdbklight);
    else LCD_BLPWM_VAL = systemset.lcdbklight;              /* �ֶ����� */

    calendar_alarm_init((_alarm_obj *)&alarm, &calendar);   /* ��ʼ������ */
    return 0;
}

/**
 * @brief       LCD�����Զ�����
 * @param       ��
 * @retval      ��
 */
void app_lcd_auto_bklight(void)
{
    uint8_t lsens;
    lsens = lsens_get_val() * 0.7;

    if (lcddev.id == 0X1963)lcd_ssd_backlight_set(30 + lsens);
    else LCD_BLPWM_VAL = 30 + lsens;
}

/**
 * @brief       �ָ�Ĭ������
 * @param       ��
 * @retval      ��
 */
void app_set_default(void)
{
    /* ϵͳ���ò���,�ָ�Ĭ������ */
    systemset.syslanguage = 0;      /* Ĭ��Ϊ�������� */
    systemset.lcdbklight = 100;     /* ����Ĭ��Ϊ���� */
    systemset.picmode = 0;          /* ͼƬ˳�򲥷� */
    systemset.videomode = 0;        /* ��Ƶ����Ĭ���ǰ�˳�򲥷� */
    systemset.saveflag = 0X0A;      /* ���ñ����� */
    sysset_save_para(&systemset);   /* ����ϵͳ������Ϣ */

//    /* VS1053���ò���,�ָ�Ĭ������ */
//    vsset.mvol = 220;               /* �������� */
//    vsset.bflimit = 6;
//    vsset.bass = 15;
//    vsset.tflimit = 10;
//    vsset.treble = 15;
//    vsset.effect = 0;
//    vsset.speakersw = 1;            /* ���ȿ��� */
//    vsset.saveflag = 0X0A;          /* ���ñ����� */
//    vs10xx_save_para(&vsset);       /* ����VS1053��������Ϣ */

    /* �������,�ָ�Ĭ������ */
    alarm.weekmask = 0X3E;          /* ������������Ϊ��1~5���� */
    alarm.ringsta = 3;              /* ����״̬��������־Ϊ3 */
    alarm.hour = 6;                 /* ����ʱ������Ϊ6:00 */
    alarm.min = 0;
    alarm.saveflag = 0X0A;          /* ���ñ����� */
    calendar_save_para(&alarm);     /* ��������������Ϣ */

    gui_phy.language = systemset.syslanguage;   /* �������� */


    if (lcddev.id == 0X1963)lcd_ssd_backlight_set(systemset.lcdbklight);
    else LCD_BLPWM_VAL = systemset.lcdbklight;              /* ���ñ������� */

    calendar_alarm_init((_alarm_obj *)&alarm, &calendar);   /* ��ʼ������ */
}














