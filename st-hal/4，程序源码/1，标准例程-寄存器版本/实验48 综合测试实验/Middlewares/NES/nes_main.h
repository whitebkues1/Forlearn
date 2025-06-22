/**
 ****************************************************************************************************
 * @file        nes_main.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-29
 * @brief       NES������ ����
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

#ifndef __NES_MAIN_H
#define __NES_MAIN_H

#include "./SYSTEM/sys/sys.h"


#define NES_SKIP_FRAME      2       /* ����ģ������֡��,Ĭ����2֡ */


#define INLINE      static inline
#define int8        char
#define int16       short
#define int32       int
#define uint8       unsigned char
#define uint16      unsigned short
#define uint32      unsigned int
#define boolean     uint8



/* nes��Ϣͷ�ṹ�� */
typedef struct
{
    unsigned char id[3];    /*  'NES' */
    unsigned char ctrl_z;   /*  control-z */
    unsigned char num_16k_rom_banks;
    unsigned char num_8k_vrom_banks;
    unsigned char flags_1;
    unsigned char flags_2;
    unsigned char reserved[8];
} NES_header;

extern uint8_t nes_frame_cnt;   /* nes֡������ */
extern int MapperNo;            /* map��� */
extern int NES_scanline;        /* ɨ���� */
extern NES_header *RomHeader;   /* rom�ļ�ͷ */
extern int VROM_1K_SIZE;
extern int VROM_8K_SIZE;
extern uint8_t cpunmi;          /* cpu�жϱ�־  �� 6502.s���� */
extern uint8_t cpuirq;
extern uint8_t PADdata;         /* �ֱ�1��ֵ */
extern uint8_t PADdata1;        /* �ֱ�1��ֵ */
extern uint8_t lianan_biao;     /* ������־ */

#define  CPU_NMI  cpunmi = 1;
#define  CPU_IRQ  cpuirq = 1;

#define  NES_RAM_SPEED  0       /* 1:�ڴ�ռ��С  0:�ٶȿ� */


void cpu6502_init(void);        /* �� cart.s */
void run6502(uint32_t);         /* �� 6502.s  */
uint8_t nes_load_rom(void);
void nes_sram_free(void);
uint8_t nes_sram_malloc(uint32_t romsize);
uint8_t nes_load(uint8_t *pname);
void nes_set_window(void);
void nes_get_gamepadval(void);
void nes_emulate_frame(void);
void debug_6502(uint16_t reg0, uint8_t reg1);

void nes_sai_dma_tx_callback(void);
int nes_sound_open(int samples_per_sync, int sample_rate);
void nes_sound_close(void);
void nes_apu_fill_buffer(int samples, uint8_t *wavebuf);

extern uint32_t get_crc32(uint8_t *buf, uint32_t len);
#endif







