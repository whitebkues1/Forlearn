/**
 ****************************************************************************************************
 * @file        rgbledplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-RGB�ʵƲ��� ����
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
 * V1.1 20220606
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __RGBLEDPLAY_H
#define __RGBLEDPLAY_H

#include "common.h"
#include "./BSP/LED/led.h"


#define LEDR(X)     LED0(X)
#define LEDG(X)     LED1(X)
#define LEDB(X)     LED2(X)



uint8_t rgbled_palette_draw(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
void rgbled_show_colorval(uint16_t x,uint16_t y,uint8_t size,uint16_t color);
void rgbled_io_config(uint8_t af);
void rgbled_pwm_set(uint16_t color);
uint8_t rgbled_play(void); 

#endif










