/**
 ****************************************************************************************************
 * @file        audioplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-29
 * @brief       ��Ƶ���� Ӧ�ô���
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
 * V1.0 20200429
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./APP/audioplayer.h"
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
#include "string.h"


/**
 * @brief       ��ʾ��Ŀ����
 * @param       index   : ��ǰ����
 * @param       total   : ���ļ���
 * @retval      ��
 */
void audio_index_show(uint16_t index, uint16_t total)
{
    /* ��ʾ��ǰ��Ŀ������,������Ŀ�� */
    lcd_show_xnum(30 + 0, 230, index, 3, 16, 0X80, RED);    /* ���� */
    lcd_show_char(30 + 24, 230, '/', 16, 0, RED);
    lcd_show_xnum(30 + 32, 230, total, 3, 16, 0X80, RED);   /* ����Ŀ */
}

void audio_vol_show(uint8_t vol)
{
    lcd_show_string(30 + 110, 230, 200, 16, 16, "VOL:", RED);
    lcd_show_xnum(30 + 142, 230, vol, 2, 16, 0X80, RED);    /* ��ʾ���� */
}

/**
 * @brief       ��ʾ����ʱ��, �����ʵ� ��Ϣ
 * @param       lenth   : �����ܳ���
 * @retval      ��
 */
void audio_msg_show(uint32_t lenth)
{
    static uint16_t playtime = 0;   /* ����ʱ���� */
    static uint16_t bitrate = 0;    /* ���������� */
    uint16_t time = 0;              /*  ʱ����� */
    uint16_t temp = 0;

    if (bitrate == 0)   /* δ���¹� */
    {
        playtime = 0;
        bitrate = vs10xx_get_bitrate(); /* ��ñ����� */
    }

    time = vs10xx_get_decode_time();    /* �õ�����ʱ�� */

    if (playtime == 0)
    {
        playtime = time;
    }
    else if ((time != playtime) && (time != 0)) /* 1sʱ�䵽,������ʾ���� */
    {
        playtime = time;    /* ����ʱ�� */
        temp = vs10xx_get_bitrate();    /* ��ñ����� */

        if (temp != bitrate)
        {
            bitrate = temp; /* ����KBPS */
        }

        /* ��ʾ����ʱ�� */
        lcd_show_xnum(30, 210, time / 60, 2, 16, 0X80, RED);        /* ���� */
        lcd_show_char(30 + 16, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 24, 210, time % 60, 2, 16, 0X80, RED);   /* ���� */
        lcd_show_char(30 + 40, 210, '/', 16, 0, RED);

        /* ��ʾ��ʱ�� */
        if (bitrate)
        {
            time = (lenth / bitrate) / 125; /* �õ������� (�ļ�����(�ֽ�)/(1000/8)/������=���������� */
        }
        else
        {
            time = 0;   /* �Ƿ�λ�� */
        }
        
        lcd_show_xnum(30 + 48, 210, time / 60, 2, 16, 0X80, RED);   /* ���� */
        lcd_show_char(30 + 64, 210, ':', 16, 0, RED);
        lcd_show_xnum(30 + 72, 210, time % 60, 2, 16, 0X80, RED);   /* ���� */

        /* ��ʾλ�� */
        lcd_show_xnum(30 + 110, 210, bitrate, 4, 16, 0X80, RED);    /* ��ʾλ�� */
        lcd_show_string(30 + 142, 210, 200, 16, 16, "Kbps", RED);
        LED0_TOGGLE (); /* DS0��ת */
    }
}

/**
 * @brief       �õ�path·����, Ŀ���ļ����ܸ���
 * @param       path    : ·��
 * @retval      ����Ч�ļ�����
 */
uint16_t audio_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;           /* ��ʱĿ¼ */
    FILINFO *tfileinfo; /* ��ʱ�ļ���Ϣ */

    tfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));   /* �����ڴ� */
    res = f_opendir(&tdir, (const TCHAR *)path);    /* ��Ŀ¼ */

    if (res == FR_OK && tfileinfo)
    {
        while (1)       /* ��ѯ�ܵ���Ч�ļ��� */
        {
            res = f_readdir(&tdir, tfileinfo);  /* ��ȡĿ¼�µ�һ���ļ� */

            if (res != FR_OK || tfileinfo->fname[0] == 0)
            {
                break;  /* ������/��ĩβ��,�˳� */
            }

            res = exfuns_file_type(tfileinfo->fname);
            if ((res & 0XF0) == 0X40)   /* ȡ����λ,�����ǲ��������ļ� */
            {
                rval++; /* ��Ч�ļ�������1 */
            }
        }
    }

    myfree(SRAMIN, tfileinfo);
    return rval;
}

/**
 * @brief       ��������
 * @param       ��
 * @retval      ��
 */
void audio_play(void)
{
    uint8_t res;
    DIR mp3dir;             /* Ŀ¼ */
    FILINFO *mp3fileinfo;   /* �ļ���Ϣ */
    char *pname;            /* ��·�����ļ��� */
    uint16_t totmp3num;     /* �����ļ����� */
    uint16_t curindex;      /* ͼƬ��ǰ���� */
    uint8_t key;            /* ��ֵ */
    uint16_t temp;
    uint16_t *mp3offsettbl; /* ���������� */

    while (f_opendir(&mp3dir, "0:/MUSIC"))  /* ��ͼƬ�ļ��� */
    {
        text_show_string(30, 190, 240, 16, "MUSIC�ļ��д���!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 190, 240, 206, WHITE); /* �����ʾ */
        delay_ms(200);
    }

    totmp3num = audio_get_tnum("0:/MUSIC"); /* �õ�����Ч�ļ��� */

    while (totmp3num == NULL)   /* �����ļ�����Ϊ0 */
    {
        text_show_string(30, 190, 240, 16, "û�������ļ�!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 190, 240, 146, WHITE); /* �����ʾ */
        delay_ms(200);
    }

    mp3fileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO)); /* Ϊ���ļ������������ڴ� */
    pname = mymalloc(SRAMIN,  2 * FF_MAX_LFN + 1);              /* Ϊ��·�����ļ��������ڴ� */
    mp3offsettbl = mymalloc(SRAMIN, 2 * totmp3num);             /* ����2*totmp3num���ֽڵ��ڴ�, ���ڴ�������ļ����� */

    while (mp3fileinfo == NULL || pname == NULL || mp3offsettbl == NULL) /* �ڴ������� */
    {
        text_show_string(30, 190, 240, 16, "�ڴ����ʧ��!", 16, 0, RED);
        delay_ms(200);
        lcd_fill(30, 190, 240, 146, WHITE); /* �����ʾ */
        delay_ms(200);
    }

    vs10xx_reset();
    vs10xx_soft_reset();
    
    vsset.mvol = 220;                       /* Ĭ����������Ϊ220 */
    audio_vol_show((vsset.mvol - 100) / 5); /* ����������: 100~250, ��ʾ��ʱ��, ���չ�ʽ(vol-100)/5, ��ʾ, Ҳ����0~30 */

    /* ��¼���� */
    res = f_opendir(&mp3dir, "0:/MUSIC");   /* ��Ŀ¼ */

    if (res == FR_OK)
    {
        curindex = 0;   /* ��ǰ����Ϊ0 */

        while (1)       /* ȫ����ѯһ�� */
        {
            temp = mp3dir.dptr;                     /* ��¼��ǰoffset */
            res = f_readdir(&mp3dir, mp3fileinfo);  /* ��ȡĿ¼�µ�һ���ļ� */

            if (res != FR_OK || mp3fileinfo->fname[0] == 0)
            {
                break;  /* ������/��ĩβ��,�˳� */
            }

            res = exfuns_file_type(mp3fileinfo->fname);

            if ((res & 0XF0) == 0X40)   /* ȡ����λ,�����ǲ��������ļ� */
            {
                mp3offsettbl[curindex] = temp;  /* ��¼���� */
                curindex++;
            }
        }
    }

    curindex = 0;   /* ��0��ʼ��ʾ */
    res = f_opendir(&mp3dir, (const TCHAR *)"0:/MUSIC");    /* ��Ŀ¼ */

    while (res == FR_OK)   /* �򿪳ɹ� */
    {
        dir_sdi(&mp3dir, mp3offsettbl[curindex]);   /* �ı䵱ǰĿ¼���� */
        res = f_readdir(&mp3dir, mp3fileinfo);      /* ��ȡĿ¼�µ�һ���ļ� */

        if (res != FR_OK || mp3fileinfo->fname[0] == 0)
        {
            break;  /* ������/��ĩβ��,�˳� */
        }

        strcpy((char *)pname, "0:/MUSIC/");         /* ����·��(Ŀ¼) */
        strcat((char *)pname, (const char *)mp3fileinfo->fname);    /* ���ļ������ں��� */
        lcd_fill(30, 190, lcddev.width, 190 + 16, WHITE);           /* ���֮ǰ����ʾ */
        text_show_string(30, 190, lcddev.width - 30, 16, mp3fileinfo->fname, 16, 0, RED);   /* ��ʾ�������� */
        audio_index_show(curindex + 1, totmp3num);
        key = audio_play_song(pname);               /* �������MP3 */

        if (key == KEY2_PRES)       /* ��һ�� */
        {
            if (curindex)
            {
                curindex--;
            }
            else
            {
                curindex = totmp3num - 1;
            }
        }
        else if (key == KEY0_PRES)  /* ��һ�� */
        {
            curindex++;

            if (curindex >= totmp3num)
            {
                curindex = 0;   /* ��ĩβ��ʱ��, �Զ���ͷ��ʼ */
            }
        }
        else
        {
            break;      /* �����˴��� */
        }
    }

    myfree(SRAMIN, mp3fileinfo);    /* �ͷ��ڴ� */
    myfree(SRAMIN, pname);          /* �ͷ��ڴ� */
    myfree(SRAMIN, mp3offsettbl);   /* �ͷ��ڴ� */
}

/**
 * @brief       ����һ��ָ���ĸ���
 * @param       pname   : ��·�����ļ���
 * @retval      ���Ž��
 *   @arg       KEY0_PRES , ��һ��
 *   @arg       KEY2_PRES , ��һ��
 *   @arg       ����      , ����
 */
uint8_t audio_play_song(char *pname)
{
    FIL *fmp3;
    uint16_t br;
    uint8_t res, rval;
    uint8_t *databuf;
    uint16_t i = 0;
    uint8_t key;

    rval = 0;
    fmp3 = (FIL *)mymalloc(SRAMIN, sizeof(FIL));    /* �����ڴ� */
    databuf = (uint8_t *)mymalloc(SRAMIN, 4096);    /* ����4096�ֽڵ��ڴ����� */

    if (databuf == NULL || fmp3 == NULL)rval = 0XFF ;   /* �ڴ�����ʧ�� */

    if (rval == 0)
    {
        vs10xx_restart_play();          /* �������� */
        vs10xx_set_all();               /* ������������Ϣ */
        vs10xx_reset_decode_time();     /* ��λ����ʱ�� */
        res = exfuns_file_type(pname);  /* �õ��ļ���׺ */

        if (res == T_FLAC)              /* �����flac,����patch */
        {
            vs10xx_load_patch((uint16_t *)vs1053b_patch, VS1053B_PATCHLEN);
        }

        res = f_open(fmp3, (const TCHAR *)pname, FA_READ); /* ���ļ� */

        if (res == 0)   /* �򿪳ɹ� */
        {
            vs10xx_spi_speed_high();    /* ���� */

            while (rval == 0)
            {
                res = f_read(fmp3, databuf, 4096, (UINT *)&br);     /* ����4096���ֽ� */
                i = 0;

                do      /* ������ѭ�� */
                {
                    if (vs10xx_send_music_data(databuf + i) == 0)   /* ��VS10XX������Ƶ���� */
                    {
                        i += 32;
                    }
                    else
                    {
                        key = key_scan(0);

                        switch (key)
                        {
                            case KEY0_PRES: /* ��һ�� */
                            case KEY2_PRES: /* ��һ�� */
                                rval = key;
                                break;

                            case WKUP_PRES: /* �������� */
                                if (vsset.mvol < 250)
                                {
                                    vsset.mvol += 5;
                                    vs10xx_set_volume(vsset.mvol);
                                }
                                else
                                {
                                    vsset.mvol = 250;
                                }
                                
                                audio_vol_show((vsset.mvol - 100) / 5); /* ����������:100~250,��ʾ��ʱ��,���չ�ʽ(vol-100)/5,��ʾ,Ҳ����0~30 */
                                break;

                            case KEY1_PRES:	/* ������ */
                                if (vsset.mvol > 100)
                                {
                                    vsset.mvol -= 5;
                                    vs10xx_set_volume(vsset.mvol);
                                }
                                else
                                {
                                    vsset.mvol = 100;
                                }

                                audio_vol_show((vsset.mvol - 100) / 5); /* ����������:100~250,��ʾ��ʱ��,���չ�ʽ(vol-100)/5,��ʾ,Ҳ����0~30 */
                                break;
                        }

                        audio_msg_show(fmp3->obj.objsize);  /* ��ʾ��Ϣ */
                    }
                } while (i < 4096); /* ѭ������4096���ֽ� */

                if (br != 4096 || res != 0)
                {
                    rval = KEY0_PRES;
                    break;  /* ������ */
                }
            }

            f_close(fmp3);
        }
        else
        {
            rval = 0XFF;    /* ���ִ��� */
        }
    }

    myfree(SRAMIN, databuf);
    myfree(SRAMIN, fmp3);
    return rval;
}




























