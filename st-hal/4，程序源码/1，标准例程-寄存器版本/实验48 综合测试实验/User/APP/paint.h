/**
 ****************************************************************************************************
 * @file        paint.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-11-16
 * @brief       APP-���� ����
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
 * V1.1 20221116
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __PAINT_H
#define __PAINT_H
#include "common.h"


/* RGB565λ���� */
typedef struct
{
    uint16_t b: 5;
    uint16_t g: 6;
    uint16_t r: 5;
} PIX_RGB565;


void paint_new_pathname(uint8_t *pname);
void paint_show_colorval(uint16_t xr,uint16_t yr,uint16_t color);
uint8_t paint_pen_color_set(uint16_t x,uint16_t y,uint16_t* color,uint8_t*caption);
uint8_t paint_pen_size_set(uint16_t x,uint16_t y,uint16_t color,uint8_t *mode,uint8_t*caption);
void paint_draw_point(uint16_t x,uint16_t y,uint16_t color,uint8_t mode);	 
uint8_t paint_play(void);

#endif























