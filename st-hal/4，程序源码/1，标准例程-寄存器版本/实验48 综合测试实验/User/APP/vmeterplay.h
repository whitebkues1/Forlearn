/**
 ****************************************************************************************************
 * @file        vmeterplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-06-06
 * @brief       APP-��ѹ����� ����
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
 *
 * V1.2 20221101
 * 1, ���STM32F103/F407��ϵ��, ����VMETER_ADC_MAX_VAL,����12/16bit ADC
 ****************************************************************************************************
 */

#ifndef __VMETERPLAY_H
#define __VMETERPLAY_H

#include "common.h"

/* ����ADC�ɼ���������ֵ: 2^λ�� - 1  */
#define VMETER_ADC_MAX_VAL      4095


void vmeter_show_7seg(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend,uint16_t offset,uint16_t color,uint16_t size,uint8_t chr,uint8_t mode);
void vmeter_show_num(uint16_t x,uint16_t y,uint8_t len,uint16_t color,uint8_t size,long long num,uint8_t mode);
uint8_t vmeter_play(void);

#endif

