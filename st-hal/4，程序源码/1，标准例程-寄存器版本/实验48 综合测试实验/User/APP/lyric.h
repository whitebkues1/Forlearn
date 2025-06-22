/**
 ****************************************************************************************************
 * @file        lyric.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       APP-�����ʾʵ�� ����
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
 ****************************************************************************************************
 */

#ifndef __LYRIC_H
#define __LYRIC_H

#include "includes.h"
#include "common.h"


#define LYRIC_BUFFER_SIZE       512     /* ��ʻ���,512�ֽ� */
#define LYRIC_MAX_SIZE          200     /* ��������� */

/* LYRIC�ṹ */
typedef __PACKED_STRUCT
{
    uint32_t time_tbl[LYRIC_MAX_SIZE];  /* LRCʱ��� */
    uint16_t addr_tbl[LYRIC_MAX_SIZE];  /* LRC��ַ�� */
    uint16_t indexsize;                 /* LRC��С,���������(�����ظ���). */
    uint16_t curindex;                  /* ��ǰλ�� */
    uint32_t curtime;                   /* ��ǰʱ��,��λ10ms */

    /* oldostime,����ʱ���� */
    uint32_t oldostime;                 /* ��һ��ucos��ʱ�� */

    /* ʵ�ֹ���Ч����4����Ҫ���� */
    uint8_t  detatime;                  /* ��һ�ι�����,������10ms���� */
    uint8_t  updatetime;                /* ����ʱ��:����ǰ����ǰ�������ʵ�ʱ���,�Լ������ʵĳ���ȷ���Ĺ���ʱ���� */
    uint16_t namelen;
    uint16_t curnamepos;

    //uint16_t bkcolor;                 /* ��ʱ���ɫ */
    uint16_t color;                     /* �����ɫ */
    uint8_t font;                       /* ������� */

    FIL *flrc;                          /* LRC�ļ� */
    uint8_t buf[LYRIC_BUFFER_SIZE];     /* LRC����ļ������� */
    uint16_t *lrcbkcolor[7];            /* 7����ʵı���ɫ�� */
} _lyric_obj;


void lrc_chg_suffix(uint8_t *name, uint8_t *sfx);
_lyric_obj *lrc_creat(void);
void lrc_delete(_lyric_obj *lcrdelete);
uint8_t lrc_str2num(uint8_t *str);
void lrc_analyze(_lyric_obj *lrcx, uint16_t lrcpos, uint8_t *str);
void lrc_sequence(_lyric_obj *lrcx);
uint8_t lrc_read(_lyric_obj *lrcx, uint8_t *path, uint8_t *name);
void lrc_show_linelrc(_lyric_obj *lrcx, uint16_t x, uint16_t y, uint16_t width, uint16_t height);


uint8_t lrc_test(uint8_t *path, uint8_t *name);
#endif












