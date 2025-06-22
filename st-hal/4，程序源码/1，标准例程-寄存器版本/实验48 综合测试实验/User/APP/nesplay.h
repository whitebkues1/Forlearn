/**
 ****************************************************************************************************
 * @file        nesplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-10-28
 * @brief       APP-NESģ���� ����
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
 * V1.1 20160627
 * ������SMS��Ϸ��֧��
 * V1.2 20221028
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __NESPLAY_H
#define __NESPLAY_H

#include "common.h"


extern uint8_t nesruning ;  /* �˳�NES�ı�־ */
extern uint8_t frame_cnt;   /* ͳ��֡�� */
 
void nes_clock_set(uint8_t PLL);
void load_nes(uint8_t* path);
uint8_t nes_play(void);

#endif




























