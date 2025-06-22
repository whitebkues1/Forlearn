/**
 ****************************************************************************************************
 * @file        sms_main.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       SMS������ ����
 *              ��������ֲ������ye781205��NESģ��������, �ش˸�л!
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
 * V1.1 20220531
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __SMS_MAIN_H
#define __SMS_MAIN_H

#include "./SYSTEM/sys/sys.h"



extern uint8_t* SMS_romfile;    /* ROMָ�� */


void sms_sram_free(void);
uint8_t sms_sram_malloc(uint32_t romsize);
void sms_set_window(void);

void sms_start(uint8_t bank_mun);
void sms_update_pad(void);
uint8_t sms_load(uint8_t* pname);

void sms_i2s_dma_tx_callback(void);
void sms_sound_open(int sample_rate);
void sms_sound_close(void);
void sms_apu_fill_buffer(int samples,uint16_t* wavebuf);
 
#endif










