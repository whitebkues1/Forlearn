/**
 ****************************************************************************************************
 * @file        recorder.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-10
 * @brief       ¼����(wav��ʽ) Ӧ�ô���
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
 * �޸�˵��
 * V1.0 20200510
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./APP/recorder.h"
#include "./BSP/VS10XX/vs10xx.h"
#include "./BSP/VS10XX/patch_flac.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./MALLOC/malloc.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./TEXT/text.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TPAD/tpad.h"
#include "string.h"


/* VS1053��WAV¼����bug, ���plugin��������������� */
const uint16_t wav_plugin[40] = /* Compressed plugin */
{
    0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */
    0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */
    0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */
    0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */
    0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,
};

/**
 * @brief       ����PCM ¼��ģʽ
 * @param       agc     : MIC����
 *   @arg       0   , �Զ�����
 *   @arg       512 , 0.5������
 *   @arg       1024, 1������
 *   @arg       ����, �Դ�����, ���65535 = 64������
 * @retval      ��
 */
void recoder_enter_rec_mode(uint16_t agc)
{
    /* �����IMA ADPCM, �����ʼ��㹫ʽ����:
     * ������ Fs = CLKI / 256 * d;
     * CLKI , ��ʾ�ڲ�ʱ��Ƶ��(��Ƶ���Ƶ��)
     * d    , ��ʾSCI_AICTRL0�ķ�Ƶֵ, ע��: ��� d = 0, ���ʾ12��Ƶ
     * ����d = 0, ��2��Ƶ, �ⲿ����Ϊ12.288M. ��ô Fs = (2 * 12288000) / 256 * 12 = 8Khz
     * ���������PCM, ������ֱ�Ӿ�д����ֵ 
     */
    vs10xx_write_cmd(SPI_BASS, 0x0000);
    vs10xx_write_cmd(SPI_AICTRL0, 8000);    /* ���ò�����, ����Ϊ8Khz */
    vs10xx_write_cmd(SPI_AICTRL1, agc);     /* �������� */
    vs10xx_write_cmd(SPI_AICTRL2, 0);       /* �����������ֵ,0,�������ֵ65536=64X */
    vs10xx_write_cmd(SPI_AICTRL3, 6);       /* ��ͨ��(MIC����������), ����PCM */
    vs10xx_write_cmd(SPI_CLOCKF, 0X2000);   /* ����VS10XX��ʱ��,MULT:2��Ƶ;ADD:������;CLK:12.288Mhz */
    vs10xx_write_cmd(SPI_MODE, 0x1804);     /* MIC, ¼������ */
    delay_ms(5);                            /* �ȴ�����1.35ms */
    vs10xx_load_patch((uint16_t *)wav_plugin, 40);  /* VS1053��WAV¼����Ҫpatch */
}

/**
 * @brief       ��ʼ��WAVͷ
 * @param       wavhead : WAVͷ�ṹ��
 * @retval      ��
 */
void recoder_wav_init(__WaveHeader *wavhead)
{
    wavhead->riff.ChunkID = 0X46464952;     /* "RIFF" */
    wavhead->riff.Format = 0X45564157;      /* "WAVE" */
    wavhead->fmt.ChunkID = 0X20746D66;      /* "fmt " */
    wavhead->fmt.ChunkSize = 16;            /* ��СΪ16���ֽ� */
    wavhead->fmt.AudioFormat = 1;           /* 1, ��ʾPCM; 0, ��ʾIMA ADPCM; */
    wavhead->fmt.NumOfChannels = 1;         /* ������ */
    wavhead->fmt.SampleRate = 8000;         /* 8Khz������ �������� */
    wavhead->fmt.ByteRate = wavhead->fmt.SampleRate * 2;    /* �ֽ�����, ���ڲ�����*2(������, 16λ) */
    wavhead->fmt.BlockAlign = 2;            /* ���С,2���ֽ�Ϊһ���� */
    wavhead->fmt.BitsPerSample = 16;        /* 16λPCM */
    wavhead->data.ChunkID = 0X61746164;     /* "data" */
    wavhead->data.ChunkSize = 0;            /* ���ݴ�С, ����Ҫ���� */
}

/**
 * @brief       ��ʾ¼��ʱ��
 * @param       tsec    : ������
 * @retval      ��
 */
void recoder_show_time(uint32_t tsec)
{
    /* ��ʾ¼��ʱ�� */
    lcd_show_string(30, 250, 200, 16, 16, "TIME:", RED);
    lcd_show_xnum(30 + 40, 250, tsec / 60, 2, 16, 0X80, RED);   /* ���� */
    lcd_show_char(30 + 56, 250, ':', 16, 0, RED);
    lcd_show_xnum(30 + 64, 250, tsec % 60, 2, 16, 0X80, RED);   /* ���� */
}

/**
 * @brief       ��ȡ���õ��ļ���
 *   @note      �ú���ͨ����ȡ�ͶԱ�SD�� RECORDERĿ¼�µ�wav�ļ����ж�һ���ļ���
 *              �Ƿ����, ���⸲��֮ǰ¼�Ƶ�wav�ļ�. ԭ�������ٷ�����
 *              ��ϳ�: ���� "0:RECORDER/REC12345.wav" ���ļ���
 * @param       pname   : �ҵ��ĺ��ʵ��ļ���
 * @retval      ��
 */
void recoder_new_pathname(uint8_t *pname)
{
    uint8_t res;
    uint16_t index = 0;
    FIL *f_temp;    /* ��ʱ�ļ� */

    f_temp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* �����ڴ� */
    if (f_temp == NULL)return ; /* �ڴ�����ʧ�� */

    while (index < 0XFFFF)  /* ���֧��65536���ļ� */
    {
        sprintf((char *)pname, "0:RECORDER/REC%05d.wav", index);
        res = f_open(f_temp, (const TCHAR *)pname, FA_READ); /* ���Դ�����ļ� */

        if (res == FR_NO_FILE)
        {
            break;  /* ���ļ��������� = ����������Ҫ�� */
        }
        
        index++;
    }
    myfree(SRAMIN, f_temp); /* �ͷ��ڴ� */
}

/**
 * @brief       ��ʾAGC��С
 * @param       agc     : 0, �Զ�����; ����, ���汶��;
 * @retval      ��
 */
void recoder_show_agc(uint8_t agc)
{
    lcd_show_string(30 + 110, 250, 200, 16, 16, "AGC:    ", RED);   /* ��ʾ����,ͬʱ����ϴε���ʾ */

    if (agc == 0)
    {
        lcd_show_string(30 + 142, 250, 200, 16, 16, "AUTO", RED);   /* �Զ�agc */
    }
    else
    {
        lcd_show_xnum(30 + 142, 250, agc, 2, 16, 0X80, RED);        /* ��ʾAGCֵ */
    }
}

/**
 * @brief       ����pname���wav�ļ�(Ҳ������������Ƶ��ʽ)
 * @param       pname   : ��·������Ƶ�ļ���
 * @retval      0, �ɹ�; 0XFF, ���ų���;
 */
uint8_t rec_play_wav(uint8_t *pname)
{
    FIL *fmp3;
    uint16_t br;
    uint8_t res, rval = 0;
    uint8_t *databuf;
    uint16_t i = 0;
    fmp3 = (FIL *)mymalloc(SRAMIN, sizeof(FIL));    /* �����ڴ� */
    databuf = (uint8_t *)mymalloc(SRAMIN, 512);     /* ����512�ֽڵ��ڴ����� */

    if (databuf == NULL || fmp3 == NULL)rval = 0XFF ;   /* �ڴ�����ʧ�� */

    if (rval == 0)
    {
        vs10xx_reset();                 /* Ӳ��λ */
        vs10xx_soft_reset();            /* ��λ */
        vs10xx_set_all();               /* ���������Ȳ��� */
        vs10xx_reset_decode_time();     /* ��λ����ʱ�� */
        res = f_open(fmp3, (const TCHAR *)pname, FA_READ);  /* ���ļ� */

        if (res == 0)   /* �򿪳ɹ� */
        {
            vs10xx_spi_speed_high();    /* ���� */

            while (rval == 0)           /* ������ѭ�� */
            {
                res = f_read(fmp3, databuf, 512, (UINT *)&br); /* ����4096���ֽ� */
                i = 0;

                do
                {
                    if (vs10xx_send_music_data(databuf + i) == 0)
                    {
                        i += 32;    /* ��VS10XX������Ƶ���� */
                    }
                    else
                    {
                        recoder_show_time(vs10xx_get_decode_time());    /* ��ʾ����ʱ�� */
                    }
                } while (i < 512);  /* ѭ������4096���ֽ� */

                if (br != 512 || res != 0)
                {
                    rval = 0;
                    break;          /* ������ */
                }
            }

            f_close(fmp3);
        }
        else
        {
            rval = 0XFF; /* ���ִ��� */
        }
        
        vs10xx_set_speaker(0);      /* �رհ������� */
    }

    myfree(SRAMIN, fmp3);
    myfree(SRAMIN, databuf);
    return rval;
}

/**
 * @brief       ¼����
 *   @note      ����¼���ļ�, �������� SD�� RECORDER �ļ�����
 * @param       ��
 * @retval      0, �ɹ�; 0XFF, ���ų���;
 */
uint8_t recoder_play(void)
{
    uint8_t res;
    uint8_t key;
    uint8_t rval = 0;
    __WaveHeader *wavhead = 0;
    uint32_t sectorsize = 0;
    FIL *f_rec = 0;         /* �ļ� */
    DIR recdir;             /* Ŀ¼ */
    UINT bw;                /* д�볤�� */
    uint8_t *recbuf;        /* �����ڴ� */
    uint16_t w;
    uint16_t idx = 0;
    char *pname = 0;
    uint8_t timecnt = 0;    /* ��ʱ�� */
    uint32_t recsec = 0;    /* ¼��ʱ�� */
    uint8_t recagc = 4;     /* Ĭ������Ϊ4 */
    uint8_t rec_sta = 0;    /* ¼��״̬
                             * [7]  : 0, û��¼��; 1, ��¼��;
                             * [6:1]: ����
                             * [0]  : 0, ����¼��; 1, ��ͣ¼��
                             */

    while (f_opendir(&recdir, "0:/RECORDER"))   /* ��¼���ļ��� */
    {
        text_show_string(30, 230, 240, 16, "RECORDER�ļ��д���!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 230, 240, 246, WHITE); /* �����ʾ */
        delay_ms(200);
        f_mkdir("0:/RECORDER"); /* ������Ŀ¼ */
    }

    pname = mymalloc(SRAMIN, 30);                   /* ����30���ֽ��ڴ�,����"0:RECORDER/REC00001.wav" */
    f_rec = (FIL *)mymalloc(SRAMIN, sizeof(FIL));   /* ����FIL�ֽڵ��ڴ����� */
    wavhead = (__WaveHeader *)mymalloc(SRAMIN, sizeof(__WaveHeader)); /* ����__WaveHeader�ֽڵ��ڴ����� */
    recbuf = mymalloc(SRAMIN, 512);

    if (pname == NULL   || f_rec == NULL ||
        wavhead == NULL || recbuf == NULL
        )
    {
        rval = 1;   /* ����ʧ�� */
    }

    if (rval == 0)  /* �ڴ�����OK */
    {
        recoder_enter_rec_mode(1024 * recagc);

        while (vs10xx_read_reg(SPI_HDAT1) >> 8);    /* �ȵ�buf ��Ϊ�����ٿ�ʼ */

        recoder_show_time(recsec);  /* ��ʾʱ�� */
        recoder_show_agc(recagc);   /* ��ʾagc */
        pname[0] = 0;               /* pnameû���κ��ļ��� */

        while (rval == 0)
        {
            key = key_scan(0);

            switch (key)
            {
                case KEY2_PRES:     /* STOP&SAVE */
                    if (rec_sta & 0X80) /* ��¼�� */
                    {
                        wavhead->riff.ChunkSize = sectorsize * 512 + 36;    /* �����ļ��Ĵ�С-8 */
                        wavhead->data.ChunkSize = sectorsize * 512;         /* ���ݴ�С */
                        f_lseek(f_rec, 0);  /* ƫ�Ƶ��ļ�ͷ */
                        f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw);   /* д��ͷ���� */
                        f_close(f_rec);
                        sectorsize = 0;
                    }

                    rec_sta = 0;
                    recsec = 0;
                    LED1(1);        /* �ر�DS1 */
                    lcd_fill(30, 230, 240, 246, WHITE); /* �����ʾ,���֮ǰ��ʾ��¼���ļ��� */
                    recoder_show_time(recsec);  /* ��ʾʱ�� */
                    break;

                case KEY0_PRES:     /* REC/PAUSE */
                    if (rec_sta & 0X01)     /* ԭ������ͣ,����¼�� */
                    {
                        rec_sta &= 0XFE;    /* ȡ����ͣ */
                    }
                    else if (rec_sta & 0X80)/* �Ѿ���¼����,��ͣ */
                    {
                        rec_sta |= 0X01;    /* ��ͣ */
                    }
                    else    /* ��û��ʼ¼�� */
                    {
                        rec_sta |= 0X80;    /* ��ʼ¼�� */
                        recoder_new_pathname((uint8_t *)pname);    /* �õ��µ����� */
                        text_show_string(30, 230, 240, 16, pname + 11, 16, 0, RED); /* ��ʾ��ǰ¼���ļ����� */
                        recoder_wav_init(wavhead);      /* ��ʼ��wav���� */
                        res = f_open(f_rec, pname, FA_CREATE_ALWAYS | FA_WRITE);

                        if (res)            /* �ļ�����ʧ�� */
                        {
                            rec_sta = 0;    /* �����ļ�ʧ��,����¼�� */
                            rval = 0XFE;    /* ��ʾ�Ƿ����SD�� */
                        }
                        else
                        {
                            res = f_write(f_rec, (const void *)wavhead, sizeof(__WaveHeader), &bw); /* д��ͷ���� */
                        }
                    }

                    LED1(!(rec_sta & 0X01));    /* ��ʾ¼��״̬ */
                    break;

                case WKUP_PRES:     /* AGC+ */
                case KEY1_PRES:     /* AGC- */
                    if (key == WKUP_PRES)
                    {
                        recagc++;
                    }
                    else if (recagc)
                    {
                        recagc--;
                    }
                    
                    if (recagc > 15) recagc = 15;   /* ��Χ�޶�Ϊ 0~15. 0, �Զ�AGC; ����AGC����; */

                    recoder_show_agc(recagc);
                    vs10xx_write_cmd(SPI_AICTRL1, 1024 * recagc);   /* ��������,0,�Զ�����.1024�൱��1��,512�൱��0.5�� */
                    break;
            }

            /* ��ȡ���� */
            if (rec_sta == 0X80)    /* �Ѿ���¼���� */
            {
                w = vs10xx_read_reg(SPI_HDAT1);

                if ((w >= 256) && (w < 896))
                {
                    idx = 0;
 
                    while (idx < 512)   /* һ�ζ�ȡ512�ֽ� */
                    {
                        w = vs10xx_read_reg(SPI_HDAT0);
                        recbuf[idx++] = w & 0XFF;
                        recbuf[idx++] = w >> 8;
                    }

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
            else    /* û�п�ʼ¼��������TPAD���� */
            {
                if (tpad_scan(0) && pname[0])   /* �����������������,��pname��Ϊ�� */
                {
                    text_show_string(30, 230, 240, 16, "����:", 16, 0, RED);
                    text_show_string(30 + 40, 230, 240, 16, pname + 11, 16, 0, RED); /* ��ʾ�����ŵ��ļ����� */
                    rec_play_wav((uint8_t *)pname);         /* ����pname */
                    lcd_fill(30, 230, 240, 246, WHITE);     /* �����ʾ,���֮ǰ��ʾ��¼���ļ��� */
                    recoder_enter_rec_mode(1024 * recagc);  /* ���½���¼��ģʽ */

                    while (vs10xx_read_reg(SPI_HDAT1) >> 8);/* �ȵ�buf ��Ϊ�����ٿ�ʼ */

                    recoder_show_time(recsec);  /* ��ʾʱ�� */
                    recoder_show_agc(recagc);   /* ��ʾagc */
                }

                delay_ms(5);
                timecnt++;

                if ((timecnt % 20) == 0)LED0_TOGGLE();  /* DS0��˸ */
            }

            if (recsec != (sectorsize * 4 / 125))   /* ¼��ʱ����ʾ */
            {
                LED0_TOGGLE();              /* DS0��˸ */
                recsec = sectorsize * 4 / 125;
                recoder_show_time(recsec);  /* ��ʾʱ�� */
            }
        }
    }

    myfree(SRAMIN, wavhead);
    myfree(SRAMIN, recbuf);
    myfree(SRAMIN, f_rec);
    myfree(SRAMIN, pname);
    return rval;
}


























