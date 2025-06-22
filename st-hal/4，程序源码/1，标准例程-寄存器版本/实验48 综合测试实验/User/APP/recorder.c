/**
 ****************************************************************************************************
 * @file        recorder.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       APP-¼���� ����
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
 * V1.1 20221028
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�uint8_t/uint16_t/uint32_tΪuint8_t/uint16_t/uint32_t
 *
 ****************************************************************************************************
 */

#include "recorder.h"
#include "calendar.h"
#include "audioplay.h"
#include "./BSP/VS10XX/vs10xx.h"
#include "./BSP/SPI/spi.h"
#include "settings.h"


/* ��λ����ɫ���� */
#define RECORDER_TITLE_COLOR        0XFFFF      /* ¼����������ɫ */
#define RECORDER_TITLE_BKCOLOR      0X0000      /* ¼�������ⱳ��ɫ */

#define RECORDER_VU_BKCOLOR         0X39C7      /* VU Meter����ɫ */
#define RECORDER_VU_L1COLOR         0X07FF      /* VU Meter L1ɫ */
#define RECORDER_VU_L2COLOR         0xFFE0      /* VU Meter L2ɫ */
#define RECORDER_VU_L3COLOR         0xF800      /* VU Meter L3ɫ */

#define RECORDER_TIME_COLOR         0X07FF      /* ʱ����ɫ */
#define RECORDER_MAIN_BKCOLOR       0X18E3      /* ������ɫ */

#define RECORDER_INWIN_FONT_COLOR   0X736C      /* ������Ƕ����ɫ, 0XAD53 */


/* ��ͼƬ�洢·�� */
uint8_t *const RECORDER_DEMO_PIC = "1:/SYSTEM/APP/RECORDER/Demo.bmp";       /* demoͼƬ·�� */
uint8_t *const RECORDER_RECR_PIC = "1:/SYSTEM/APP/RECORDER/RecR.bmp";       /* ¼�� �ɿ� */
uint8_t *const RECORDER_RECP_PIC = "1:/SYSTEM/APP/RECORDER/RecP.bmp";       /* ¼�� ���� */
uint8_t *const RECORDER_PAUSER_PIC = "1:/SYSTEM/APP/RECORDER/PauseR.bmp";   /* ��ͣ �ɿ� */
uint8_t *const RECORDER_PAUSEP_PIC = "1:/SYSTEM/APP/RECORDER/PauseP.bmp";   /* ��ͣ ���� */
uint8_t *const RECORDER_STOPR_PIC = "1:/SYSTEM/APP/RECORDER/StopR.bmp";     /* ֹͣ �ɿ� */
uint8_t *const RECORDER_STOPP_PIC = "1:/SYSTEM/APP/RECORDER/StopP.bmp";     /* ֹͣ ���� */


/* ��˷��������� */
const uint8_t *recoder_set_tbl[GUI_LANGUAGE_NUM] = {"��˷���������", "�����L�����O��", "MIC GAIN SET"};

/* ¼����ʾ��Ϣ */
uint8_t *const recorder_remind_tbl[3][GUI_LANGUAGE_NUM] =
{
    "�Ƿ񱣴��¼���ļ�?", "�Ƿ񱣴�ԓ����ļ�?", "Do you want to save?",
    {"����ֹͣ¼��!", "Ո��ֹͣ���!", "Please stop REC first!",},
    {"�ڴ治��!!", "�ȴ治��!!", "Out of memory!",},
};

/* 00������ѡ������ */
uint8_t *const recorder_modesel_tbl[GUI_LANGUAGE_NUM] =
{
    "¼������", "����O��", "Recorder Set",
};

/* VS1053��WAV¼����bug,���plugin��������������� */
const uint16_t wav_plugin[40] = /* Compressed plugin */
{
    0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */
    0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */
    0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */
    0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */
    0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,
};

/**
 * @brief       ����ADPCM ¼��ģʽ
 * @param       ��
 * @retval      ��
 */
void recorder_enter_rec_mode(void)
{
    /**
     * �����IMA ADPCM,�����ʼ��㹫ʽ����:
     * ������=CLKI/256*d;
     * ����d=0,��2��Ƶ,�ⲿ����Ϊ12.288M.��ôFc=(2*12288000)/256*6=16Khz
     * ���������PCM,������ֱ�Ӿ�д����ֵ
     */
    vs10xx_write_cmd(SPI_BASS, 0x0000);
    vs10xx_write_cmd(SPI_AICTRL0, 8000);    /* ���ò�����,����Ϊ8Khz */
    vs10xx_write_cmd(SPI_AICTRL2, 0);       /* �����������ֵ,0,�������ֵ65536=64X */
    vs10xx_write_cmd(SPI_AICTRL3, 6);       /* ��ͨ��(MIC����������) */
    vs10xx_write_cmd(SPI_CLOCKF, 0X2000);   /* ����VS10XX��ʱ��,MULT:2��Ƶ;ADD:������;CLK:12.288Mhz */
    vs10xx_write_cmd(SPI_MODE, 0x1804);     /* MIC,¼������ */
    delay_ms(5);                            /* �ȴ�����1.35ms */
    vs10xx_load_patch((uint16_t *)wav_plugin, 40);  /* VS1053��WAV¼����Ҫplugin */
}

/**
 * @brief       ����VS1053��AGC
 * @param       agc             : 0,�Զ�����.1~64,��ʾ1~64��
 * @retval      ��
 */
void recoder_set_agc(uint16_t agc)
{
    if (agc >= 64)agc = 65535;
    else agc *= 1024;

    vs10xx_write_cmd(SPI_AICTRL1, agc); /* �������� */
}

/**
 * @brief       ��ʼ��WAVͷ
 * @param       wavhead         : wav�ļ�ͷ�ṹ��
 * @retval      ��
 */
void recorder_wav_init(__WaveHeader *wavhead)
{
    wavhead->riff.ChunkID = 0X46464952;     /* "RIFF" */
    wavhead->riff.ChunkSize = 0;            /* ��δȷ��,�����Ҫ���� */
    wavhead->riff.Format = 0X45564157;      /* "WAVE" */
    wavhead->fmt.ChunkID = 0X20746D66;      /* "fmt " */
    wavhead->fmt.ChunkSize = 16;            /* ��СΪ16���ֽ� */
    wavhead->fmt.AudioFormat = 0X01;        /* 0X01,��ʾPCM;0X01,��ʾIMA ADPCM */
    wavhead->fmt.NumOfChannels = 1;         /* ������ */
    wavhead->fmt.SampleRate = 8000;         /* ������,��λ:Hz, */
    wavhead->fmt.ByteRate = wavhead->fmt.SampleRate * 2;    /* �ֽ�����=������*ͨ����*(ADCλ��/8),����ȷ�� */
    wavhead->fmt.BlockAlign = 2;            /* ���С=ͨ����*(ADCλ��/8) */
    wavhead->fmt.BitsPerSample = 16;        /* 16λPCM */
    wavhead->data.ChunkID = 0X61746164;     /* "data" */
    wavhead->data.ChunkSize = 0;            /* ���ݴ�С,����Ҫ���� */
}

/**
 * @brief       ��VS10XX������
 * @param       address         : �Ĵ�����ַ
 * @retval      ������ֵ
 */
uint16_t recorder_read_data(uint8_t address)
{
    uint16_t temp = 0;

    while (VS10XX_DQ == 0);     /* �ǵȴ�����״̬ */

    VS10XX_XDCS(1);
    VS10XX_XCS(0);
    vs10xx_spi_read_write_byte(VS_READ_COMMAND);    /* ����VS10XX�Ķ����� */
    vs10xx_spi_read_write_byte(address);            /* ��ַ */
    temp = vs10xx_spi_read_write_byte(0xff);        /* ��ȡ���ֽ� */
    temp = temp << 8;
    temp += vs10xx_spi_read_write_byte(0xff);       /* ��ȡ���ֽ� */
    VS10XX_XCS(1);
    return temp;
}

/* ��ƽ��ֵ�� */
const uint16_t vu_val_tbl[10] = {3000, 4500, 6500, 9000, 11000, 14000, 18000, 22000, 27000, 32000};

/**
 * @brief       ���źŵ�ƽ�õ�vu����ֵ
 * @param       signallevel     : �źŵ�ƽ
 * @retval      vuֵ
 */
uint8_t recorder_vu_get(uint16_t signallevel)
{
    uint8_t i;

    for (i = 10; i > 0; i--)
    {
        if (signallevel >= vu_val_tbl[i - 1])break;
    }

    return i;
}

/**
 * @brief       ��ʾVU Meter
 * @param       x,y             : ����
 * @param       level           : 0~10
 * @retval      ��
 */
void recorder_vu_meter(uint16_t x, uint16_t y, uint8_t level)
{
    uint8_t i;
    uint16_t vucolor = RECORDER_VU_L1COLOR;

    if (level > 10)return ;

    if (level == 0)
    {
        gui_fill_rectangle(x, y, 218, 10, RECORDER_VU_BKCOLOR); /* ��䱳��ɫ */
        return;
    }

    for (i = 0; i < level; i++)
    {
        if (i == 9)vucolor = RECORDER_VU_L3COLOR;
        else if (i > 5)vucolor = RECORDER_VU_L2COLOR;

        gui_fill_rectangle(x + 22 * i, y, 20, 10, vucolor);     /* ��䱳��ɫ */
    }

    if (level < 10)gui_fill_rectangle(x + level * 22, y, 218 - level * 22, 10, RECORDER_VU_BKCOLOR);    /* ��䱳��ɫ */
}

/**
 * @brief       ��ʾ¼��ʱ��
 * @param       x,y             : ����
 * @param       tsec            : ������
 * @retval      ��
 */
void recorder_show_time(uint16_t x, uint16_t y, uint32_t tsec)
{
    uint8_t min;

    if (tsec >= 60 * 100)min = 99;
    else min = tsec / 60;

    gui_phy.back_color = RECORDER_MAIN_BKCOLOR;
    gui_show_num(x, y, 2, RECORDER_TIME_COLOR, 60, min, 0X80);  /* XX */
    gui_show_ptchar(x + 60, y, lcddev.width, lcddev.height, 0, RECORDER_TIME_COLOR, 60, ':', 0);    /* ":" */
    gui_show_num(x + 90, y, 2, RECORDER_TIME_COLOR, 60, tsec % 60, 0X80);   /* XX */
}

/**
 * @brief       ��ʾ����
 * @param       x,y             : ����(��Ҫ��0��ʼ)
 * @param       name            : ����
 * @retval      ��
 */
void recorder_show_name(uint16_t x, uint16_t y, uint8_t *name)
{
    gui_fill_rectangle(x - 1, y - 1, lcddev.width, 13, RECORDER_MAIN_BKCOLOR);  /* ��䱳��ɫ */
    gui_show_ptstrwhiterim(x, y, lcddev.width, y + 12, 0, BLACK, WHITE, 12, name);
}

/**
 * @brief       ��ʾ������
 * @param       x,y             : ����(��Ҫ��0��ʼ)
 * @param       samplerate      : ������
 * @retval      ��
 */
void recorder_show_samplerate(uint16_t x, uint16_t y, uint16_t samplerate)
{
    uint8_t *buf = 0;
    float temp;
    temp = (float)samplerate / 1000;
    buf = gui_memin_malloc(60); /* �����ڴ� */

    if (buf == 0)return;

    if (samplerate % 1000)sprintf((char *)buf, "%.1fKHz", temp);        /* ��С���� */
    else sprintf((char *)buf, "%dKHz", samplerate / 1000);

    gui_fill_rectangle(x, y, 42, 12, RECORDER_MAIN_BKCOLOR);            /* ��䱳��ɫ */
    gui_show_string(buf, x, y, 42, 12, 12, RECORDER_INWIN_FONT_COLOR);  /* ��ʾagc */
    gui_memin_free(buf);        /* �ͷ��ڴ� */
}

/**
 * @brief       ����¼����������UI
 * @param       ��
 * @retval      ��
 */
void recorder_load_ui(void)
{
    gui_fill_rectangle(0, 0, lcddev.width, gui_phy.tbheight, RECORDER_TITLE_BKCOLOR);           /* ��䱳��ɫ */
    gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, RECORDER_TITLE_COLOR, gui_phy.tbfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[11][gui_phy.language]); /* ��ʾ���� */
    gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight, RECORDER_MAIN_BKCOLOR);             /* ����ɫ */
    minibmp_decode((uint8_t *)RECORDER_DEMO_PIC, (lcddev.width - 100) / 2, 100 + (lcddev.height - 320) / 2, 100, 100, 0, 0);    /* ����100*100��ͼƬDEMO */
    recorder_vu_meter((lcddev.width - 218) / 2, (lcddev.height - 320) / 2 + 200 + 5, 0);        /* ��ʾvu meter; */
    app_gui_tcbar(0, lcddev.height - gui_phy.tbheight, lcddev.width, gui_phy.tbheight, 0x01);   /* �Ϸֽ��� */
}

/**
 * @brief       ͨ��ʱ���ȡ�ļ���
 *  note        ������SD������,��֧��FLASH DISK����
 *              ��ϳ�:����"0:RECORDER/REC20120321210633.wav"���ļ���
 * @param       x,y             : ����(��Ҫ��0��ʼ)
 * @param       name            : ���ֻ�����
 * @retval      ��
 */
void recorder_new_pathname(uint8_t *pname)
{
    rtc_get_time();     /* �õ�ʱ��,�ڸú�����,���calendarȫ�ֽṹ�������Ϣ */

    if (gui_phy.memdevflag & (1 << 0))
    {
        sprintf((char *)pname, "0:RECORDER/REC%04d%02d%02d%02d%02d%02d.wav", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);  /* ��ѡ������SD�� */
    }
}

/**
 * @brief       ��ʾAGC��С
 * @param       x,y             : ����
 * @param       agc             : ����ֵ 0~15,0,�Զ�����;1~15,�Ŵ���
 * @retval      ��
 */
void recorder_show_agc(uint16_t x, uint16_t y, uint8_t agc)
{
    gui_phy.back_color = APP_WIN_BACK_COLOR;                /* ���ñ���ɫΪ��ɫ */
    gui_fill_rectangle(x, y, 24, 12, APP_WIN_BACK_COLOR);   /* ��䱳��ɫ */

    if (agc == 0)
    {
        gui_show_string("AUTO", x, y, 24, 12, 12, RECORDER_INWIN_FONT_COLOR);   /* ��ʾagc */
    }
    else
    {
        gui_show_num(x, y, 2, RECORDER_INWIN_FONT_COLOR, 12, agc, 0X80);        /* ��ʾ��ֵ */
    }
}

/**
 * @brief       agc���ý���.�̶��ߴ�:180*122
 * @param       x,y             : ����
 * @param       agc             : ����ֵ 0~15,0,�Զ�����;1~15,�Ŵ���
 * @param       caption         : ��������
 * @retval      0,�ɹ�����;
 *              ����,����������
 */
uint8_t recorder_agc_set(uint16_t x, uint16_t y, uint8_t *agc, uint8_t *caption)
{
    uint8_t rval = 0, res;
    _window_obj *twin = 0;      /* ���� */
    _btn_obj *rbtn = 0;         /* ȡ����ť */
    _btn_obj *okbtn = 0;        /* ȷ����ť */
    _progressbar_obj *agcprgb;  /* AGC���ý����� */
    uint8_t tempagc = *agc;

    twin = window_creat(x, y, 180, 122, 0, 1 | 1 << 5, 16);     /* �������� */
    agcprgb = progressbar_creat(x + 10, y + 52, 160, 15, 0X20); /* ���������� */

    if (agcprgb == NULL)rval = 1;

    okbtn = btn_creat(x + 20, y + 82, 60, 30, 0, 0x02);         /* ������ť */
    rbtn = btn_creat(x + 20 + 60 + 20, y + 82, 60, 30, 0, 0x02);/* ������ť */

    if (twin == NULL || rbtn == NULL || okbtn == NULL || rval)rval = 1;
    else
    {
        /* ���ڵ����ֺͱ���ɫ */
        twin->caption = caption;
        twin->windowbkc = APP_WIN_BACK_COLOR;
        
        /* ���ذ�ť����ɫ */
        rbtn->bkctbl[0] = 0X8452;   /* �߿���ɫ */
        rbtn->bkctbl[1] = 0XAD97;   /* ��һ�е���ɫ */
        rbtn->bkctbl[2] = 0XAD97;   /* �ϰ벿����ɫ */
        rbtn->bkctbl[3] = 0X8452;   /* �°벿����ɫ */
        okbtn->bkctbl[0] = 0X8452;  /* �߿���ɫ */
        okbtn->bkctbl[1] = 0XAD97;  /* ��һ�е���ɫ */
        okbtn->bkctbl[2] = 0XAD97;  /* �ϰ벿����ɫ */
        okbtn->bkctbl[3] = 0X8452;  /* �°벿����ɫ */
        agcprgb->totallen = 15;     /* ���AGCΪ15 */
        agcprgb->curpos = tempagc;  /* ��ǰ�ߴ� */
        rbtn->caption = (uint8_t *)GUI_CANCEL_CAPTION_TBL[gui_phy.language];    /* ����Ϊȡ�� */
        okbtn->caption = (uint8_t *)GUI_OK_CAPTION_TBL[gui_phy.language];       /* ����Ϊȷ�� */
        window_draw(twin);          /* �������� */
        btn_draw(rbtn);             /* ����ť */
        btn_draw(okbtn);            /* ����ť */
        progressbar_draw_progressbar(agcprgb);
        gui_show_string("AGC:", x + 10, y + 38, 24, 12, 12, RECORDER_INWIN_FONT_COLOR); /* ��ʾSIZE */
        recorder_show_agc(x + 10 + 24, y + 38, tempagc);

        while (rval == 0)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

            if (system_task_return)break;           /* TPAD���� */

            res = btn_check(rbtn, &in_obj);         /* ȡ����ť��� */

            if (res && ((rbtn->sta & 0X80) == 0))   /* ����Ч���� */
            {
                rval = 1;
                break;/* �˳� */
            }

            res = btn_check(okbtn, &in_obj);        /* ȷ�ϰ�ť��� */
 
            if (res && ((okbtn->sta & 0X80) == 0))  /* ����Ч���� */
            {
                rval = 0XFF;
                break;/* ȷ���� */
            }

            res = progressbar_check(agcprgb, &in_obj);

            if (res && (tempagc != agcprgb->curpos))    /* �������Ķ��� */
            {
                tempagc = agcprgb->curpos;  /* �������µĽ�� */
                recorder_show_agc(x + 10 + 24, y + 38, tempagc);
                recoder_set_agc(tempagc);   /* �������� */
            }
        }
    }

    window_delete(twin);        /* ɾ������ */
    btn_delete(rbtn);           /* ɾ����ť */
    btn_delete(okbtn);          /* ɾ����ť */
    progressbar_delete(agcprgb);/* ɾ�������� */
    system_task_return = 0;

    if (rval == 0XFF)
    {
        *agc = tempagc;
        return 0;
    }

    return rval;
}

/**
 * @brief       ¼����
 *  note        ����¼���ļ�,��������SD��RECORDER�ļ�����.
 * @param       ��
 * @retval      ��
 */
uint8_t recorder_play(void)
{
    uint8_t res;
    uint8_t rval = 0;
    __WaveHeader *wavhead = 0;
    FIL *f_rec = 0;             /* ¼���ļ� */
    uint32_t br, bw;
    
    uint32_t sectorsize = 0;
    _btn_obj *rbtn = 0;         /* ȡ����ť */
    _btn_obj *mbtn = 0;         /* ѡ�ť */
    _btn_obj *recbtn = 0;       /* ¼����ť */
    _btn_obj *stopbtn = 0;      /* ֹͣ¼����ť */

    uint16_t *pset_bkctbl = 0;  /* ����ʱ����ɫָ�� */
    uint32_t recsec = 0;        /* ¼��ʱ�� */

    uint8_t *recbuf;            /* �����ڴ� */
    uint8_t *pname = 0;
    uint16_t w;
    uint16_t idx = 0;

    uint8_t timecnt = 0;
    uint8_t vulevel = 0;
    short tempval;
    uint8_t temp;
    uint16_t maxval = 0;

    uint8_t rec_sta = 0;        /**
                                 * ¼��״̬ 
                                 * [7]:0,û�п���¼��;1,�Ѿ�����¼��;
                                 * [6:1]:����
                                 * [0]:0,����¼��;1,��ͣ¼��;
                                 */

    uint8_t recagc = 4;         /* Ĭ������Ϊ4�� */

    f_rec = (FIL *)gui_memin_malloc(sizeof(FIL));   /* ����FIL�ֽڵ��ڴ����� */
    wavhead = (__WaveHeader *)gui_memin_malloc(sizeof(__WaveHeader)); /* ����__WaveHeader�ֽڵ��ڴ����� */
    pname = gui_memin_malloc(60);   /* ����30���ֽ��ڴ�,����"0:RECORDER/REC20120321210633.wav" */
    recbuf = gui_memin_malloc(512); /* ����512�ֽ��ڴ� */
    pset_bkctbl = gui_memex_malloc(180 * 272 * 2);  /* Ϊ����ʱ�ı���ɫ�������ڴ� */

    if (!recbuf || !f_rec || !wavhead || !pname || !pset_bkctbl)rval = 1;
    else
    {
        /* �������� */
        res = f_open(f_rec, (const TCHAR *)APP_ASCII_S6030, FA_READ); /* ���ļ��� */

        if (res == FR_OK)
        {
            asc2_s6030 = (uint8_t *)gui_memex_malloc(f_rec->obj.objsize);   /* Ϊ�����忪�ٻ����ַ */

            if (asc2_s6030 == 0)rval = 1;
            else
            {
                res = f_read(f_rec, asc2_s6030, f_rec->obj.objsize, (UINT *)&br);   /* һ�ζ�ȡ�����ļ� */
            }
        }

        if (res)rval = 1;

        recorder_load_ui(); /* װ�������� */
        rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */
        mbtn = btn_creat(0, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03);  /* �������ְ�ť */
        recbtn = btn_creat((lcddev.width - 96) / 3, (lcddev.height - 320) / 2 + 215 + 18, 48, 48, 0, 1);            /* ����ͼƬ��ť */
        stopbtn = btn_creat((lcddev.width - 96) * 2 / 3 + 48, (lcddev.height - 320) / 2 + 215 + 18, 48, 48, 0, 1);  /* ����ͼƬ��ť */

        if (!rbtn || !mbtn || !recbtn || !stopbtn)rval = 1; /* û���㹻�ڴ湻���� */
        else
        {
            rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language];      /* ���� */
            rbtn->font = gui_phy.tbfsize;   /* �����µ������С */
            rbtn->bcfdcolor = WHITE;        /* ����ʱ����ɫ */
            rbtn->bcfucolor = WHITE;        /* �ɿ�ʱ����ɫ */

            mbtn->caption = (uint8_t *)GUI_OPTION_CAPTION_TBL[gui_phy.language];    /* ���� */
            mbtn->font = gui_phy.tbfsize;   /* �����µ������С */
            mbtn->bcfdcolor = WHITE;        /* ����ʱ����ɫ */
            mbtn->bcfucolor = WHITE;        /* �ɿ�ʱ����ɫ */

            recbtn->picbtnpathu = (uint8_t *)RECORDER_RECR_PIC;
            recbtn->picbtnpathd = (uint8_t *)RECORDER_PAUSEP_PIC;
            recbtn->bcfucolor = 0X0001;     /* ����䱳�� */
            recbtn->bcfdcolor = 0X0001;     /* ����䱳�� */
            recbtn->sta = 0;

            stopbtn->picbtnpathu = (uint8_t *)RECORDER_STOPR_PIC;
            stopbtn->picbtnpathd = (uint8_t *)RECORDER_STOPP_PIC;
            stopbtn->bcfucolor = 0X0001;    /* ����䱳�� */
            stopbtn->bcfdcolor = 0X0001;    /* ����䱳�� */
            recbtn->sta = 0;
        }
    }

    if (rval == 0)
    {
        if (gui_phy.memdevflag & (1 << 0))f_mkdir("0:RECORDER");    /* ǿ�ƴ����ļ���,��¼������ */

        btn_draw(rbtn);
        btn_draw(mbtn);
        btn_draw(recbtn);
        recbtn->picbtnpathu = (uint8_t *)RECORDER_PAUSER_PIC;
        recbtn->picbtnpathd = (uint8_t *)RECORDER_RECP_PIC;
        btn_draw(stopbtn);

        if (audiodev.status & (1 << 7)) /* ��ǰ�ڷŸ�??����ֹͣ */
        {
            audio_stop_req(&audiodev);  /* ֹͣ��Ƶ���� */
            audio_task_delete();        /* ɾ�����ֲ������� */
        }

        rec_sta = 0;
        recsec = 0;
        recorder_enter_rec_mode();      /* ����¼��ģʽ,������AGC */
        recorder_show_samplerate((lcddev.width - 218) / 2, (lcddev.height - 320) / 2 + 200 + 5 - 15, 8000); /* ��ʾ������ */
        recoder_set_agc(recagc);        /* �������� */

        while (vs10xx_read_reg(SPI_HDAT1) >> 8);    /* �ȵ�buf ��Ϊ�����ٿ�ʼ */

        recorder_show_time((lcddev.width - 150) / 2, 40 + (lcddev.height - 320) / 2, recsec); /* ��ʾʱ�� */

        while (rval == 0)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */

            if (system_task_return)break;           /* TPAD���� */

            res = btn_check(rbtn, &in_obj);         /* ��鷵�ذ�ť */

            if (res && ((rbtn->sta & (1 << 7)) == 0) && (rbtn->sta & (1 << 6)))break; /* ���ذ�ť */

            res = btn_check(mbtn, &in_obj);         /* ������ð�ť */

            if (res && ((mbtn->sta & (1 << 7)) == 0) && (mbtn->sta & (1 << 6)))
            {
                app_read_bkcolor((lcddev.width - 180) / 2, (lcddev.height - 272) / 2, 180, 272, pset_bkctbl); /* ��ȡ����ɫ */
                res = recorder_agc_set((lcddev.width - 180) / 2, (lcddev.height - 122) / 2, &recagc, (uint8_t *)recoder_set_tbl[gui_phy.language]); /* ����AGC */
                recoder_set_agc(recagc);            /* �������� */
                app_recover_bkcolor((lcddev.width - 180) / 2, (lcddev.height - 272) / 2, 180, 272, pset_bkctbl); /* �ָ�����ɫ */
            }

            res = btn_check(recbtn, &in_obj);       /* ���¼����ť */

            if (res && ((recbtn->sta & (1 << 7)) == 0) && (recbtn->sta & (1 << 6)))
            {
                if (rec_sta & 0X01)     /* ԭ������ͣ,����¼�� */
                {
                    rec_sta &= 0XFE;    /* ȡ����ͣ */
                    recbtn->picbtnpathu = (uint8_t *)RECORDER_RECR_PIC;
                    recbtn->picbtnpathd = (uint8_t *)RECORDER_PAUSEP_PIC;
                }
                else if (rec_sta & 0X80)    /* �Ѿ���¼����,��ͣ */
                {
                    rec_sta |= 0X01;    /* ��ͣ */
                    recbtn->picbtnpathu = (uint8_t *)RECORDER_PAUSER_PIC;
                    recbtn->picbtnpathd = (uint8_t *)RECORDER_RECP_PIC;
                }
                else    /* ��û��ʼ¼�� */
                {
                    rec_sta |= 0X80;    /* ��ʼ¼�� */
                    sectorsize = 0;     /* �ļ���С����Ϊ0 */
                    recbtn->picbtnpathu = (uint8_t *)RECORDER_RECR_PIC;
                    recbtn->picbtnpathd = (uint8_t *)RECORDER_PAUSEP_PIC;
                    pname[0] = '\0';    /* ��ӽ����� */
                    recorder_new_pathname(pname);   /* �õ��µ����� */
                    recorder_show_name(2, gui_phy.tbheight + 4, pname); /* ��ʾ���� */
                    recorder_wav_init(wavhead);     /* ��ʼ��wav���� */
                    res = f_open(f_rec, (const TCHAR *)pname, FA_CREATE_ALWAYS | FA_WRITE);

                    if (res)            /* �ļ�����ʧ�� */
                    {
                        rec_sta = 0;    /* �����ļ�ʧ��,����¼�� */
                        rval = 0XFE;    /* ��ʾ�Ƿ����SD�� */
                    }
                    else res = f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw);   /* д��ͷ���� */
                }
            }

            res = btn_check(stopbtn, &in_obj);  /* ���ֹͣ��ť */
 
            if (res && ((recbtn->sta & (1 << 7)) == 0) && (recbtn->sta & (1 << 6)))
            {
                if (rec_sta & 0X80)   /* ��¼�� */
                {
                    wavhead->riff.ChunkSize = sectorsize * 512 + 36;    /* �����ļ��Ĵ�С-8; */
                    wavhead->data.ChunkSize = sectorsize * 512;         /* ���ݴ�С */
                    f_lseek(f_rec, 0);  /* ƫ�Ƶ��ļ�ͷ */
                    f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw); /* д��ͷ���� */
                    f_close(f_rec);
                    sectorsize = 0;
                    recbtn->picbtnpathu = (uint8_t *)RECORDER_RECR_PIC;
                    recbtn->picbtnpathd = (uint8_t *)RECORDER_PAUSEP_PIC;
                    btn_draw(recbtn);
                    recbtn->picbtnpathu = (uint8_t *)RECORDER_PAUSER_PIC;
                    recbtn->picbtnpathd = (uint8_t *)RECORDER_RECP_PIC;
                }

                rec_sta = 0;
                recsec = 0;
                recorder_show_name(2, gui_phy.tbheight + 4, "");        /* ��ʾ���� */
                recorder_show_time((lcddev.width - 150) / 2, 40 + (lcddev.height - 320) / 2, recsec); /* ��ʾʱ�� */
            }

            /* ��ȡ���� */
            spi1_set_speed(SPI_SPEED_16);   /* ���õ�����ģʽ,4.5Mhz */
            
            w = recorder_read_data(SPI_HDAT1);

            if ((w >= 256) && (w < 896))
            {
                idx = 0;

                while (idx < 512)   /* һ�ζ�ȡ512�ֽ� */
                {
                    w = recorder_read_data(SPI_HDAT0);
                    tempval = (short)w;

                    if (tempval < 0)tempval = -tempval;

                    if (maxval < tempval)maxval = tempval;

                    recbuf[idx++] = w & 0XFF;
                    recbuf[idx++] = w >> 8;
                }

                timecnt++;

                if ((timecnt % 2) == 0)
                {
                    temp = recorder_vu_get(maxval);

                    if (temp > vulevel)vulevel = temp;
                    else if (vulevel)vulevel--;

                    recorder_vu_meter((lcddev.width - 218) / 2, (lcddev.height - 320) / 2 + 200 + 5, vulevel); /* ��ʾvu meter */
                    maxval = 0;
                }

                if (rec_sta == 0X80)   /* �Ѿ���¼���� */
                {
                    res = f_write(f_rec, recbuf, 512, &bw); /* д���ļ� */

                    if (res)
                    {
                        printf("err:%d\r\n", res);
                        printf("bw:%d\r\n", bw);
                        break;      /* д����� */
                    }

                    sectorsize++;   /* ����������1,ԼΪ32ms */
                }
            }
            else delay_ms(1000 / OS_TICKS_PER_SEC);     /* ��ʱһ��ʱ�ӽ��� */

            if (recsec != ((sectorsize * 512) / wavhead->fmt.ByteRate)) /* ¼��ʱ����ʾ */
            {
                recsec = (sectorsize * 512) / wavhead->fmt.ByteRate;    /* ¼��ʱ�� */
                recorder_show_time((lcddev.width - 150) / 2, 40 + (lcddev.height - 320) / 2, recsec); /* ��ʾʱ�� */
            }
        }
    }
    else
    {
        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)recorder_remind_tbl[2][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* �ڴ���� */
        delay_ms(2000);
    }

    if (rval == 0XFE)   /* �����ļ�ʧ����,��Ҫ��ʾ�Ƿ����SD�� */
    {
        window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)APP_CREAT_ERR_MSG_TBL[gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* ��ʾSD���Ƿ���� */
        delay_ms(2000); /* �ȴ�2���� */
    }

    if (rec_sta & 0X80) /* �������¼��,����ʾ�������¼���ļ� */
    {
        res = window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, "", (uint8_t *)recorder_remind_tbl[0][gui_phy.language], 12, 0, 0X03, 0);

        if (res == 1)   /* ��Ҫ���� */
        {
            wavhead->riff.ChunkSize = sectorsize * 512 + 36;    /* �����ļ��Ĵ�С-8; */
            wavhead->data.ChunkSize = sectorsize * 512;         /* ���ݴ�С */
            f_lseek(f_rec, 0);                                  /* ƫ�Ƶ��ļ�ͷ */
            f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw);   /* д��ͷ���� */
            f_close(f_rec);
        }
    }

    vs10xx_reset();	/* Ӳ��λVS1003 */

    /* �ͷ��ڴ� */
    gui_memin_free(recbuf);
    gui_memin_free(f_rec);
    gui_memin_free(wavhead);
    gui_memin_free(pname);
    gui_memex_free(pset_bkctbl);
    gui_memex_free(asc2_s6030);
    asc2_s6030 = 0; /* ���� */
    btn_delete(rbtn);
    btn_delete(mbtn);
    btn_delete(recbtn);
    btn_delete(stopbtn);
    return rval;
}



































