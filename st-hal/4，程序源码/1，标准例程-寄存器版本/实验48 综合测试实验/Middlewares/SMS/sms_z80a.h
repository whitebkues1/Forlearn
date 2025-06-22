/**
 ****************************************************************************************************
 * @file        sms_z80a.h
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

#ifndef __SMS_Z80A_H
#define __SMS_Z80A_H

#include <stdint.h>


typedef struct _CPU80
{
    uint8_t   B;    //00 B
    uint8_t   C;    //01 C
    uint8_t   D;    //02 D
    uint8_t   E;    //03 E
    uint8_t   H;    //04 H
    uint8_t   L;    //05 L
    uint8_t   F;    //06 F
    uint8_t   A;    //07 A
    uint16_t  IX;   //08 IX
    uint16_t  IY;   //0a IY
    uint16_t  PC;   //0c PC
    uint16_t  SP;   //0e SP
    uint32_t  MP;   //10 indirizzo 1K Memory Pointer array
    uint32_t  PT;   //14 indirizzo Parity Table bit bandingindirizzoƽ�۱�λ����
    uint32_t  PAR;  //18 HI: CPU ram base address, LO: work variable
    uint8_t   R;    //1c R
    uint8_t   I;    //1d I
    uint8_t   IFF;  //1e IFF
    uint8_t   RBM;  //1f ROM Bank Mask (numero bank -1)
    uint8_t   Bi;   //20 B'
    uint8_t   Ci;   //21 C'
    uint8_t   Di;   //22 D'
    uint8_t   Ei;   //23 E'
    uint8_t   Hi;   //24 H'
    uint8_t   Li;   //25 L'
    uint8_t   Fi;   //26 F'
    uint8_t   Ai;   //27 A'
    uint8_t   CT;   //28 Controller port value�������˿�ֵ
    uint8_t   VV;   //29 VDP_Vertical value
    uint8_t   BS0;  //2a Bank Switch 0 (FFFC)
    uint8_t   BS3;  //2b Bank Switch 3 (FFFF)
    uint32_t  XROM; //2c Cartridge ROM base address
    uint32_t  XRAM; //30 Cartridge RAM base address
    //uint8_t*  Mempnt[64]; //pnt relativo a 1K
    //uint32_t  P_Tab[8];   //Parity Tab
    //uint8_t*  Ram;
} CPU80;
extern CPU80 *Z80A;


int32_t CPU_run(int32_t cycle);  //esegue codice CPU finche' (cycle)>0
void    CPU_Irq(void);           //RST38
void    CPU_Reset(void);         //Reset
int8_t  Z80A_Init(uint8_t *SMS_Ram, uint8_t *Ext_Ram, uint8_t *Ext_Rom, uint8_t RomBanksMask);
void    Z80A_del_ram(void);
void    SetRiga(uint8_t VDP_V);
void    SetController(uint8_t padstate);

#endif
