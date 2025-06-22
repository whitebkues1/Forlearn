/**
 ****************************************************************************************************
 * @file        qrplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-��ά��ʶ��&���� ����
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

#ifndef __QRPLAY_H
#define __QRPLAY_H

#include "common.h"


#define QR_BACK_COLOR           0XA599      /* ����ɫ */

/* ��qrplay.c���涨�� */
extern volatile uint8_t qr_dcmi_rgbbuf_sta; /* RGB BUF״̬ */
extern uint16_t qr_dcmi_curline;            /* ����ͷ�������,��ǰ�б�� */

void qr_cursor_show(uint8_t csize);
void qr_dcmi_rx_callback(void);
void qr_decode_show_result(uint8_t* result);
void qr_decode_play(void);
void qr_encode_play(void);
void qr_play(void);

#endif























