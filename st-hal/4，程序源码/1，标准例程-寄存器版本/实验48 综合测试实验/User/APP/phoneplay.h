/**
 ****************************************************************************************************
 * @file        phoneplay.h
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

#ifndef __PHONEPLAY_H
#define __PHONEPLAY_H

#include "common.h"
#include "./BSP/GSM/gsm.h"


#define PHONE_MAX_INPUT     200     /* �������200�����볤�� */


extern uint8_t *const PHONE_CALL_PIC[3];
extern uint8_t *const PHONE_HANGUP_PIC[3];
extern uint8_t *const PHONE_DEL_PIC[3];

typedef __PACKED_STRUCT
{
    uint16_t xoff;      /* x����ƫ�� */
    uint16_t yoff;      /* y����ƫ�� */
    uint16_t width;     /* ��� */
    uint16_t height;    /* �߶� */

    uint16_t time;      /* ͨ��ʱ�� */

    uint8_t *inbuf;     /* �������ֻ����� */
    uint8_t inlen;      /* �������ֳ��� */
    uint8_t *phonebuf;  /* ���뻺���� */
    uint8_t plen;       /* ���볤�� */
    uint8_t psize;      /* ���������С:28,36,54 */
    uint8_t tsize;      /* ʱ�������С:12,16,24 */
} _phonedis_obj;
extern _phonedis_obj *pdis;


void phone_show_clear(_phonedis_obj *pdis);
void phone_show_time(_phonedis_obj *pdis);
void phone_show_calling(_phonedis_obj *pdis, __gsmdev *gsmx);
void phone_show_phone(_phonedis_obj *pdis, __gsmdev *gsmx);
void phone_show_input(_phonedis_obj *pdis, __gsmdev *gsmx);
void phone_ring(void);
void phone_incall_task(void *pdata);
uint8_t phone_incall_task_creat(void);
void phone_incall_task_delete(void);
uint8_t phone_play(void);

#endif























