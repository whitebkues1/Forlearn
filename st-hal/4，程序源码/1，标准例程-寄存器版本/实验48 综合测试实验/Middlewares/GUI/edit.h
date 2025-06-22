/**
 ****************************************************************************************************
 * @file        edit.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-�༭�� ����
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
 * V1.1 20140825
 * 1, �޸�edit_check��������ģ�����
 * V1.2 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __EDIT_H
#define __EDIT_H

#include "guix.h"


/* �༭��߿���ɫ���� */
#define EDIT_RIM_LTOC   0XA535      /* ����������ɫ */
#define EDIT_RIM_LTIC   0X8431      /* ����������ɫ */
#define EDIT_RIM_RBOC   0XFFFF      /* ����������ɫ */
#define EDIT_RIM_RBIC   0XDEFC      /* ����������ɫ */
#define EDIT_BACK_COLOR 0XFFFF      /* �༭�򱳾�ɫ */

/* �༭���ı���ɫ���� */
#define EDIT_DFT_TBKC   0XFFFF      /* Ĭ�ϵ��ı�����ɫ */
#define EDIT_DFT_TXTC   0X0000      /* Ĭ�ϵ��ı���ɫ */


/* EDIT�ṹ�嶨�� */
typedef __PACKED_STRUCT
{
    uint16_t top;                   /* �༭�򶥶����� */
    uint16_t left;                  /* �༭��������� */
    uint16_t width;                 /* ���(���ַ����ĳ��ȶ�6����) */
    uint16_t height;                /* �߶�(������߶ȸ�6����) */

    uint8_t id;                     /* �༭��ID */
    
    uint8_t type;                   /**
                                     * �༭������
                                     * [7:3]:����
                                     * [2]:0,���߿�;1,�����߿�(�߿���ñ���ɫ)
                                     * [1]:0,����ʾ���;1,��ʾ���;
                                     * [0]:
                                     *     0,��׼edit,��֮������д��.
                                     *     1,�Զ����edit,��֮���Զ����,�ص�ԭλ.
                                     */

    uint8_t sta;                    /**
                                     * �༭��״̬
                                     * [7:1]:����
                                     * [1]:�����ʾ״̬ 0,δ��ʾ.1,��ʾ.
                                     * [0]:�༭��ѡ��״̬ 0,δѡ��.1,ѡ��.
                                     */

    uint8_t *text;                  /* �༭�������,����<=width-6 */
    uint8_t font;                   /* text�������� */
    uint16_t cursorpos;             /* ����λ��((font/2)*x����) */
    uint32_t edit_timer_old;        /* ��һ��ʱ�� */

    uint16_t textcolor;             /* ������ɫ */
    uint16_t textbkcolor;           /* ���ֱ���ɫ */
} _edit_obj;

_edit_obj *edit_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t id, uint8_t type, uint8_t font);
void edit_delete(_edit_obj *edit_del);
uint16_t edit_get_cursorxpos(_edit_obj *editx);
void edit_draw(_edit_obj *editx);
void edit_read_cursorbkctbl(_edit_obj *editx);
void edit_draw_cursor(_edit_obj *editx, uint16_t color);
void edit_recover_cursorbkctbl(_edit_obj *editx);
void edit_show_cursor(_edit_obj *editx, uint8_t sta);
void edit_cursor_flash(_edit_obj *editx);
void edit_draw_text(_edit_obj *editx);
void edit_add_text(_edit_obj *editx, uint8_t *str);
void edit_draw(_edit_obj *editx);
uint8_t edit_check(_edit_obj *editx, void *in_key);

//void edit_test(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t type, uint8_t sta, uint8_t *text);

#endif
































