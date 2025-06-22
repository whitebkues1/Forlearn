/**
 ****************************************************************************************************
 * @file        sms_sn76496.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       SMSģ���� ����
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

#ifndef __SMS_SN76496_H
#define __SMS_SN76496_H

#include "./SYSTEM/sys/sys.h"


#define MAX_76496       4
#define MASTER_CLOCK    3579545
#define Sound_Rate      44100            /* ������ */
#define SNBUF_size      Sound_Rate/60    /* ÿ֡������  1, 44100/60=735;  2, 22050/60=367 */
#define sms_Frame       2                /* ֡��Ƶ���ݻ�����Ŀ */


typedef struct
{
    int  Channel;
    int  SampleRate;
    int  VolTable[16];
    int  Register[8];
    int  Volume[4];
    int  LastRegister;
    int  NoiseFB;
    int  Period[4];
    int  Count[4];
    int  Output[4];
    unsigned int RNG_A;
    unsigned int UpdateStep;
} t_SN76496;
extern t_SN76496 *SN76496;
extern uint16_t *psg_buffer;    /* ��Ƶ���ݻ���,��СΪ:SNBUF_size*2�ֽ� */

void SN76496Write(int data);
void SN76496Update(short *buffer, int length, unsigned char mask);
void SN76496_set_clock(int clock);
void SN76496_set_gain(int gain);
int  SN76496_init(int clock, int volume, int sample_rate);

uint8_t sms_audio_init(void);
void sms_update_Sound(void);
#endif
