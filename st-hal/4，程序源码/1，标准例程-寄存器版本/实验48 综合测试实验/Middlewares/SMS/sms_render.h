/**
 ****************************************************************************************************
 * @file        sms_render.h
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

#ifndef __SMS_RENDER_H
#define __SMS_RENDER_H

#include "sms_vdp.h"


/* formato de cor usado pelo gens (16 bits 555 ou 565) */
#define MAKE_PIXEL(r, g, b) ((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | ((b >> 3) & 0x001F) 

#define BACKDROP_COLOR      (0x10 | (SMS_VDP->reg[7] & 0x0F))

void  render_init(void);
void  render_reset(void);
//void  render_bg_gg(int line);
void  render_bg_sms(int line);
void  render_obj(int line);
void  update_cache(void);
void  palette_sync(int index);
void render_line(int line);

#endif



