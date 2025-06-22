/**
 ****************************************************************************************************
 * @file        sms_vdp.h
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

#ifndef __SMS_VDP_H
#define __SMS_VDP_H


/* Display timing (NTSC) */
//#define MASTER_CLOCK        (3579545)
#define LINES_PER_FRAME     (262)
#define FRAMES_PER_SECOND   (60)
#define CYCLES_PER_LINE     ((MASTER_CLOCK / FRAMES_PER_SECOND) / LINES_PER_FRAME)
#define SMS_PALETTE_SIZE        (0x20)


#if 1
//#define int8 char
//#define int16 short
//#define int32 int
#define BYTE unsigned char
#define u8 unsigned char
#define WORD unsigned short
#define uint8_t unsigned char
#define uint16_t unsigned short
#define DWORD unsigned int
//#define boolean uint8
#endif /* a whole bunch of crap */

extern u8 *VRam;//[0x4000]
//extern u8 *CRam;//[0x40]
//extern BYTE *linebuf;//[0x100];
extern BYTE *cache;//[0x20000];
//extern BYTE *vram_dirty;//[0x200];
//extern BYTE is_vram_dirty;
extern BYTE *lut;//[0x10000];
//extern uint16_t *SMS_Palette;//[PALETTE_SIZE];//32
/* VDP context */
typedef struct _SVDP
{
    BYTE  reg[0x10];
    BYTE  status;
    BYTE  latch;
    BYTE  pending;
    BYTE  buffer;
    BYTE  code;
    BYTE  limit;
    BYTE is_vram_dirty;
    WORD  addr;
    int   ntab;
    int   satb;
    //	uint16_t   left;
    uint16_t   line;
    u8 CRam[0x40];
    BYTE linebuf[0x100];
    BYTE vram_dirty[0x200];
    uint16_t SMS_Palette[32];//0x20
} SVDP;

/* Global data */
extern SVDP *SMS_VDP;

/* Function prototypes */
void vdp_ctrl_w(BYTE data);
//BYTE  vdp_ctrl_r(void);
void vdp_data_w(BYTE data);
//BYTE  vdp_data_r(void);
//void  SMS_VDP_Run(void);

//BYTE  vdp_hcounter_r(void);
void VDP_del_ram(void);
uint8_t VDP_init(void);
void SMS_frame(uint8_t zhen);

#endif /* _VDP_H_ */

