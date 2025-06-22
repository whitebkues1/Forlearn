/**
 ****************************************************************************************************
 * @file        audioplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-29
 * @brief       ��Ƶ���� Ӧ�ô���
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200429
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __AUDIOPLAYER_H
#define __AUDIOPLAYER_H

#include "./SYSTEM/sys/sys.h"


void audio_vol_show(uint8_t vol);
void audio_msg_show(uint32_t lenth);
uint16_t audio_get_tnum(char *path);
void audio_index_show(uint16_t index, uint16_t total);

void audio_play(void);
uint8_t audio_play_song(char *pname);

#endif












