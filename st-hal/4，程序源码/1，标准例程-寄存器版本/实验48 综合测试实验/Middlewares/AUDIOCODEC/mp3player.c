/**
 ****************************************************************************************************
 * @file        mp3player.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       MP3�������� ����
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
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 *
 ****************************************************************************************************
 */
 
#include "audioplay.h"

#include "./AUDIOCODEC/mp3player.h"
#include "./BSP/VS10XX/vs10xx.h"
#include "./BSP/VS10XX/patch_flac.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/KEY/key.h"


/**
 * @brief       ��ȡ��ǰ���ŵ���Ϣ
 * @param       fx              : �ļ�ָ��
 * @retval      ��
 */
void mp3_get_info(FIL *fx)
{
    static uint16_t temps = 0;
    uint32_t temp;
    temp = vs10xx_get_decode_time();

    if (temp != temps)
    {
        temps = temp;

        if (audiodev.samplerate == 0)   /* ��ȡ��Ƶ������ */
        {
            audiodev.samplerate = vs10xx_read_reg(SPI_AUDATA) & 0XFFFE; /* �������λ */
        }

        temp = vs10xx_get_bitrate();    /* �õ�λ�� */
        temp *= 1000;

        if (audiodev.bitrate != temp)
        {
            audiodev.bitrate = temp;
            audiodev.totsec = fx->obj.objsize / (audiodev.bitrate / 8); /* ���¼�����ʱ�� */
        }
    }
}

/**
 * @brief       �õ���ǰ����ʱ��
 * @param       fx              : �ļ�ָ��
 * @retval      ��
 */
void mp3_get_curtime(FIL *fx)
{
    audiodev.cursec = fx->fptr * audiodev.totsec / (fx->obj.objsize);   /* ��ǰ���ŵ��ڶ�������? */
}

/**
 * @brief       mp3�ļ�������˺���
 * @param       pos             : ��Ҫ��λ�����ļ�λ��
 * @retval      ��ǰ�ļ�λ��(����λ��Ľ��)
 */
uint32_t mp3_file_seek(uint32_t pos)
{
    if (pos > audiodev.file->obj.objsize)
    {
        pos = audiodev.file->obj.objsize;
    }

    f_lseek(audiodev.file, pos);
    return audiodev.file->fptr;
}

/**
 * @brief       ����һ������
 * @param       fname           : MP3�ļ�·��.
 * @retval      0, �����������
 *              [b7]:0,����״̬;1,����״̬
 *              [b6:0]:b7=0ʱ,��ʾ������
 *                     b7=1ʱ,��ʾ�д���(���ﲻ�ж��������,0X80~0XFF,�����Ǵ���)
 */
uint8_t mp3_play_song(uint8_t *pname)
{
    uint16_t br;
    uint8_t res;
    uint8_t *databuf;
    uint16_t i = 0;
    uint8_t t = 0;
    databuf = (uint8_t *)mymalloc(SRAMIN, 4096);    /* ����4096�ֽڵ��ڴ����� */
    audiodev.file = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    audiodev.file_seek = mp3_file_seek;

    if (!databuf || !audiodev.file) /* �ڴ�����ʧ�� */
    {
        myfree(SRAMIN, databuf);
        myfree(SRAMIN, audiodev.file);
        return AP_ERR;              /* ���� */
    }

    audiodev.totsec = 0;            /* ��ʱ������ */
    audiodev.cursec = 0;            /* ��ǰ����ʱ������ */
    audiodev.bitrate = 0;           /* λ������ */
    audiodev.samplerate = 0;        /* ���������� */
    audiodev.bps = 16;              /* ȫ������Ϊ16λ */
    vs10xx_restart_play();          /* �������� */
    vs10xx_set_all();               /* ������������Ϣ */
    vs10xx_reset_decode_time();     /* ��λ����ʱ�� */
    res = exfuns_file_type((char*)pname);   /* �õ��ļ���׺ */

    if (res == 0x43)                /* �����flac,����patch */
    {
        vs10xx_load_patch((uint16_t *)vs1053b_patch, VS1053B_PATCHLEN);
    }

    res = f_open(audiodev.file, (const TCHAR *)pname, FA_READ); /* ���ļ� */

    if (res == 0)                   /* �򿪳ɹ� */
    {
        audio_start();              /* ��ʼ���� */
        vs10xx_spi_speed_high();    /* ���� */

        while (res == 0)
        {
            res = f_read(audiodev.file, databuf, 4096, (UINT *)&br);    /* ����4096���ֽ� */

            if (res)    /* �����ݳ����� */
            {
                res = AP_ERR;
                break;
            }

            i = 0;

            do          /* ������ѭ�� */
            {
                if (vs10xx_send_music_data(databuf + i) == 0)   /* ��VS10XX������Ƶ���� */
                {
                    i += 32;
                }
                else
                {
                    while (audiodev.status & (1 << 1))          /* ���������� */
                    {
                        t++;

                        if (t == 40) /* 200ms����һ�� */
                        {
                            mp3_get_info(audiodev.file);
                        }

                        delay_ms(1000 / OS_TICKS_PER_SEC);
                        mp3_get_curtime(audiodev.file);

                        if (audiodev.status & 0X01)break;       /* û�а�����ͣ */
                    }

                    if ((audiodev.status & (1 << 1)) == 0)      /* �����������/������� */
                    {
                        res = AP_NEXT; /* �������ϼ�ѭ�� */
                        break;
                    }
                }
            } while (i < 4096); /* ѭ������4096���ֽ� */

            if (br != 4096 || res != 0)
            {
                res = AP_OK;    /* ������� */
                break;          /* ������ */
            }
        }

        audio_stop();           /* �ر���Ƶ��� */
    }
    else res = AP_ERR;          /* ������ */

    f_close(audiodev.file);
    myfree(SRAMIN, databuf);
    myfree(SRAMIN, audiodev.file);
    return res;
}














