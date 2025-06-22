/**
 ****************************************************************************************************
 * @file        gradienter.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-ˮƽ�� ����
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

#ifndef __GRADIENTER_H
#define __GRADIENTER_H

#include "common.h"


void grad_draw_hline(short x0,short y0,uint16_t len,uint16_t color);
void grad_fill_circle(short x0,short y0,uint16_t r,uint16_t color);
uint8_t grad_load_font(void);
void grad_delete_font(void);

uint8_t grad_play(void); 

#endif



