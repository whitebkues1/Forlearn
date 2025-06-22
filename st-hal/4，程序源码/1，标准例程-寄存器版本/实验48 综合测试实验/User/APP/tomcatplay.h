/**
 ****************************************************************************************************
 * @file        tomcatplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       APP-TOMè ����
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
 * 2, �޸�uint8_t/uint16_t/uint32_tΪuint8_t/uint16_t/uint32_t
 *
 ****************************************************************************************************
 */

#ifndef __TOMCATPLAY_H
#define __TOMCATPLAY_H

#include "common.h"


/* ��ͼ��/ͼƬ·�� */
extern uint8_t*const TOMCAT_DEMO_PIC;//demoͼƬ·�� 	      


void tomcat_load_ui(void);
void tomcat_show_spd(uint16_t x,uint16_t y,uint16_t spd);
uint8_t tomcat_agcspd_set(uint16_t x,uint16_t y,uint8_t *agc,uint16_t *speed,uint8_t*caption);
void tomcat_rec_mode(uint8_t agc);
void tomcat_play_wav(uint8_t *buf,uint32_t len);
void tomcat_data_move(uint8_t* buf,uint16_t size,uint16_t dx);
uint8_t tomcat_play(void);

#endif























