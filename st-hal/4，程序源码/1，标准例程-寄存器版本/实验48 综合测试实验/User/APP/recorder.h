/**
 ****************************************************************************************************
 * @file        recorder.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       APP-¼���� ����
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

#ifndef __RECORDER_H
#define __RECORDER_H

#include "common.h"


/* ��ͼ��/ͼƬ·�� */
extern uint8_t *const RECORDER_DEMO_PIC;    /* demoͼƬ·�� */
extern uint8_t *const RECORDER_RECR_PIC;    /* ¼�� �ɿ� */
extern uint8_t *const RECORDER_RECP_PIC;    /* ¼�� ���� */
extern uint8_t *const RECORDER_PAUSER_PIC;  /* ��ͣ �ɿ� */
extern uint8_t *const RECORDER_PAUSEP_PIC;  /* ��ͣ ���� */
extern uint8_t *const RECORDER_STOPR_PIC;   /* ֹͣ �ɿ� */
extern uint8_t *const RECORDER_STOPP_PIC;   /* ֹͣ ���� */

/* RIFF�� */
typedef __packed struct
{
    uint32_t ChunkID;           /* chunk id;����̶�Ϊ"RIFF",��0X46464952 */
    uint32_t ChunkSize ;        /* ���ϴ�С;�ļ��ܴ�С-8 */
    uint32_t Format;            /* ��ʽ;WAVE,��0X45564157 */
} ChunkRIFF ;

/* fmt�� */
typedef __packed struct
{
    uint32_t ChunkID;           /* chunk id;����̶�Ϊ"fmt ",��0X20746D66 */
    uint32_t ChunkSize ;        /* �Ӽ��ϴ�С(������ID��Size);����Ϊ:20 */
    uint16_t AudioFormat;       /* ��Ƶ��ʽ;0X10,��ʾ����PCM;0X11��ʾIMA ADPCM */
    uint16_t NumOfChannels;     /* ͨ������;1,��ʾ������;2,��ʾ˫����; */
    uint32_t SampleRate;        /* ������;0X1F40,��ʾ8Khz */
    uint32_t ByteRate;          /* �ֽ�����; */
    uint16_t BlockAlign;        /* �����(�ֽ�); */
    uint16_t BitsPerSample;     /* �����������ݴ�С;4λADPCM,����Ϊ4 */
    //uint16_t ByteExtraData;    /* ���ӵ������ֽ�;2��; ����PCM,û��������� */
    //uint16_t ExtraData;        /* ���ӵ�����,�����������ݿ��С;0X1F9:505�ֽ�  ����PCM,û��������� */
} ChunkFMT;

/* fact�� */
typedef __packed struct
{
    uint32_t ChunkID;           /* chunk id;����̶�Ϊ"fact",��0X74636166; */
    uint32_t ChunkSize ;        /* �Ӽ��ϴ�С(������ID��Size);����Ϊ:4. */
    uint32_t NumOfSamples;      /* ���������� */
} ChunkFACT;

/* data�� */
typedef __packed struct
{
    uint32_t ChunkID;           /* chunk id;����̶�Ϊ"data",��0X61746164 */
    uint32_t ChunkSize ;        /* �Ӽ��ϴ�С(������ID��Size);�ļ���С-60 */
} ChunkDATA;

/* wavͷ */
typedef __packed struct
{
    ChunkRIFF riff;             /* riff�� */
    ChunkFMT fmt;               /* fmt�� */
    //ChunkFACT fact;            /* fact�� ����PCM,û������ṹ�� */
    ChunkDATA data;             /* data�� */
} __WaveHeader;


void recorder_enter_rec_mode(void);
void recorder_wav_init(__WaveHeader *wavhead);/* ��ʼ��WAVͷ */
void recoder_set_agc(uint16_t agc);
uint16_t recorder_read_data(uint8_t address);
uint8_t recorder_vu_get(uint16_t signallevel);
void recorder_vu_meter(uint16_t x, uint16_t y, uint8_t level);
void recorder_show_time(uint16_t x, uint16_t y, uint32_t tsec);
void recorder_show_name(uint16_t x, uint16_t y, uint8_t *name);
void recorder_show_samplerate(uint16_t x, uint16_t y, uint16_t samplerate);
void recorder_load_ui(void);
void recorder_new_pathname(uint8_t *pname);
void recorder_show_agc(uint16_t x, uint16_t y, uint8_t agc);
uint8_t recorder_agc_set(uint16_t x, uint16_t y, uint8_t *agc, uint8_t *caption);
uint8_t recorder_play(void);

#endif























