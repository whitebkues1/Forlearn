/**
 ****************************************************************************************************
 * @file        audioplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       APP-���ֲ����� ����
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

#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H

#include "./SYSTEM/sys/sys.h"
#include "includes.h"
#include "common.h"
#include "lyric.h"


/* ��ͼ��/ͼƬ·�� */
extern uint8_t *const AUDIO_BTN_PIC_TBL[2][5];  /* 5��ͼƬ��ť��·�� */
extern uint8_t *const AUDIO_BACK_PIC[3];        /* ��������ͼƬ */
extern uint8_t *const AUDIO_PLAYR_PIC;          /* ���� �ɿ� */
extern uint8_t *const AUDIO_PLAYP_PIC;          /* ���� ���� */
extern uint8_t *const AUDIO_PAUSER_PIC;         /* ��ͣ �ɿ� */
extern uint8_t *const AUDIO_PAUSEP_PIC;         /* ��ͣ ���� */

/* ����ɫ���� */
#define AUDIO_TITLE_COLOR       0XFFFF          /* ������������ɫ */
#define AUDIO_TITLE_BKCOLOR     0X0000          /* ���������ⱳ��ɫ */

#define AUDIO_INFO_COLOR        0X8410          /* ��Ϣ�������ɫ */
#define AUDIO_MAIN_BKCOLOR      0X18E3          /* ������ɫ */
#define AUDIO_BTN_BKCOLOR       0XDF9F          /* 5�����ư�ť����ɫ */

#define AUDIO_LRC_MCOLOR        0XF810          /* ��ǰ�����ɫΪ��ɫ */
#define AUDIO_LRC_SCOLOR        0XFFFF          /* ǰһ��ͺ�һ������ɫΪ��ɫ */


/* ���ֲ��Ų���������� */
typedef enum
{
    AP_OK = 0X00,       /* ����������� */
    AP_NEXT,            /* ������һ�� */
    AP_PREV,            /* ������һ�� */
    AP_ERR = 0X80,      /* �����д���(û����������,������ʾ����) */
} APRESULT;

/* ���ֲ��ſ����� */
typedef __packed struct
{
    /* 2��I2S�����BUF */
    FIL *file;                      /* ��Ƶ�ļ�ָ�� */
    uint32_t(*file_seek)(uint32_t); /* �ļ�������˺��� */

    volatile uint8_t status;        /* bit0:0,��ͣ����;1,�������� */
                                    /* bit1:0,��������;1,�������� */
                                    /* bit2~3:���� */
                                    /* bit4:0,�����ֲ���;1,���ֲ����� (������) */
                                    /* bit5:0,�޶���;1,ִ����һ���и����(������) */
                                    /* bit6:0,�޶���;1,������ֹ����(���ǲ�ɾ����Ƶ��������),������ɺ�,���������Զ������λ */
                                    /* bit7:0,��Ƶ����������ɾ��/����ɾ��;1,��Ƶ����������������(�������ִ��) */

    uint8_t mode;           /* ����ģʽ */
                            /* 0,ȫ��ѭ��;1,����ѭ��;2,�������; */

    uint8_t *path;          /* ��ǰ�ļ���·�� */
    uint8_t *name;          /* ��ǰ���ŵ�MP3�������� */
    uint16_t namelen;       /* name��ռ�ĵ��� */
    uint16_t curnamepos;    /* ��ǰ��ƫ�� */

    uint32_t totsec ;       /* ���׸�ʱ��,��λ:�� */
    uint32_t cursec ;       /* ��ǰ����ʱ�� */
    uint32_t bitrate;       /* ������(λ��) */
    uint32_t samplerate;        /* ������ */
    uint16_t bps;           /* λ��,����16bit,24bit,32bit */

    uint16_t curindex;      /* ��ǰ���ŵ���Ƶ�ļ����� */
    uint16_t mfilenum;      /* �����ļ���Ŀ */
    uint16_t *mfindextbl;   /* ��Ƶ�ļ������� */

} __audiodev;
extern __audiodev audiodev; /* ���ֲ��ſ����� */


/* ȡ2��ֵ����Ľ�Сֵ */
#ifndef AUDIO_MIN
#define AUDIO_MIN(x,y)	((x)<(y)? (x):(y))
#endif



void audio_start(void);
void audio_stop(void);

void music_play_task(void *pdata);
void audio_stop_req(__audiodev *audiodevx);
uint8_t audio_filelist(__audiodev *audiodevx);
void audio_load_ui(void);
void audio_show_vol(uint8_t pctx);
void audio_time_show(uint16_t sx, uint16_t sy, uint16_t sec);
void audio_info_upd(__audiodev *audiodevx, _progressbar_obj *audioprgbx, _progressbar_obj *volprgbx, _lyric_obj *lrcx);
void audio_lrc_bkcolor_process(_lyric_obj *lrcx, uint8_t mode);
void audio_lrc_show(__audiodev *audiodevx, _lyric_obj *lrcx);
uint8_t audio_task_creat(void);
void audio_task_delete(void);
uint8_t audio_play(void);

#endif












