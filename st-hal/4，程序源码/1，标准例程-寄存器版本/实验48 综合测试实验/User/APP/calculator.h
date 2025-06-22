/**
 ****************************************************************************************************
 * @file        calculator.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-��ѧ������ ����
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

#ifndef __CALCULATOR_H
#define __CALCULATOR_H

#include "common.h"


/* ��ͼ��/ͼƬ·�� */
extern uint8_t *const CALC_UP_BTN_TBL[29];      /* �ɿ�ʱ����ͼ��·�� */
extern uint8_t *const CALC_DOWN_BTN_TBL[29];    /* ����ʱ����ͼ��·�� */


typedef __PACKED_STRUCT
{
    uint16_t xoff;      /* x����ƫ��:2,10,10 */
    uint16_t yoff;      /* y����ƫ��:10,20,50 */
    uint16_t width;     /* ��� */
    uint16_t height;    /* �߶� */
    uint8_t xdis;       /* ��������x��������С:2,4,2 */
    uint8_t ydis;       /* ��������y��������С:5,11,19 */
    uint8_t fsize;      /* ����������С:28,36,60 */
} _calcdis_obj;
extern _calcdis_obj *cdis;

uint8_t calc_play(void);
uint8_t calc_show_res(_calcdis_obj *calcdis, double res);
void calc_input_fresh(_calcdis_obj *calcdis, uint8_t *calc_sta, uint8_t *inbuf, uint8_t len);
void calc_ctype_show(_calcdis_obj *calcdis, uint8_t ctype);
void calc_show_flag(_calcdis_obj *calcdis, uint8_t fg);
void calc_show_exp(_calcdis_obj *calcdis, short exp);
uint8_t calc_exe(_calcdis_obj *calcdis, double *x1, double *x2, uint8_t *buf, uint8_t ctype, uint8_t *calc_sta);
void calc_show_inbuf(_calcdis_obj *calcdis, uint8_t *buf);
void calc_load_ui(_calcdis_obj *calcdis);

#endif























