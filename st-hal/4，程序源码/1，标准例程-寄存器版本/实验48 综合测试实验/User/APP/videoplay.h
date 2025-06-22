/**
 ****************************************************************************************************
 * @file        videoplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-��Ƶ������ ����
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

#ifndef __VIDEOPLAY_H
#define __VIDEOPLAY_H

#include "./SYSTEM/sys/sys.h"
#include "./MJPEG/avi.h"
#include "./MJPEG/mjpeg.h"
#include "includes.h"


#define AVI_AUDIO_BUF_SIZE    1024*5        /* ����avi����ʱ,��Ƶbuf��С */
#define AVI_VIDEO_BUF_SIZE    1024*260      /* ����avi����ʱ,��Ƶbuf��С */

/* ��Ƶ���ſ����� */
typedef __PACKED_STRUCT
{
    uint8_t *path;          /* ��ǰ�ļ���·�� */
    uint8_t *name;          /* ��ǰ�������� */
    volatile uint8_t status;/**
                             * bit0:0,��ͣ����;1,��������
                             * bit1:0,���/������;1,��������
                             * ����,����
                             */

    uint16_t curindex;      /* ��ǰ���ŵ���Ƶ�ļ����� */
    uint16_t mfilenum;      /* ��Ƶ�ļ���Ŀ */
    uint32_t *mfindextbl;   /* ��Ƶ�ļ������� */

    FIL *file;                      /* ��Ƶ�ļ�ָ�� */
    volatile uint8_t saiplaybuf;    /* �������ŵ���Ƶ֡������ */
    uint8_t *saibuf[4];             /* ��Ƶ����֡,��4֡,4*AVI_AUDIO_BUF_SIZE */
} __videodev;

extern __videodev videodev;         /* ��Ƶ���ſ����� */


/* ��Ƶ���Ž���,UIλ�ýṹ�� */
typedef __PACKED_STRUCT
{
    /* ����������ز��� */
    uint8_t tpbar_height;   /* �����������߶� */
    uint8_t capfsize;       /* ���������С */
    uint8_t msgfsize;       /* ��ʾ��Ϣ�����С(��Ƶ��/����/��Ƶ��/��Ƶ������/֡��/�ֱ���/����ʱ����) */

    /* �м���Ϣ����ز��� */
    uint8_t msgbar_height;  /* ��Ϣ���߶� */
    uint8_t nfsize;         /* ��Ƶ�������С */
    uint8_t xygap;          /* x,y�����ƫ����,������/����ͼ��/֡�ʼ�� 1 gap,������Ϣ,1/2 gap */
    uint16_t vbarx;         /* ������x���� */
    uint16_t vbary;         /* ������y���� */
    uint16_t vbarwidth;     /* ���������� */
    uint16_t vbarheight;    /* ��������� */
    uint8_t msgdis;         /* ������(������) dis+����ͼ��+vbar+dis+������+֡��+dis */

    /* ���Ž�����ز��� */
    uint8_t prgbar_height;  /* �������߶� */
    uint16_t pbarwidth;     /* ���������� */
    uint16_t pbarheight;    /* ��������� */
    uint16_t pbarx;         /* ������x���� */
    uint16_t pbary;         /* ������y���� */

    /* ��ť����ز��� */
    uint8_t btnbar_height;  /* ��ť���߶� */
} __videoui;

extern __videoui *vui;      /* ��Ƶ���Ž�������� */


void video_time_show(uint16_t sx, uint16_t sy, uint16_t sec);
void video_load_ui(void);
void video_show_vol(uint8_t pctx);
void video_info_upd(__videodev *videodevx, _progressbar_obj *videoprgbx, _progressbar_obj *volprgbx, AVI_INFO *aviinfo, uint8_t flag);
uint8_t video_play(void);
uint8_t video_play_mjpeg(void);
uint8_t video_seek_vids(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf);
uint8_t video_seek(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf, uint32_t dstpos);
uint8_t video_seek_key(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf);    /* ������ʽ������� */

#endif































