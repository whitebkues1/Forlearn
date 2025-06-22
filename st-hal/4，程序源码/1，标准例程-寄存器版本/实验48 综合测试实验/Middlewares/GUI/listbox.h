/**
 ****************************************************************************************************
 * @file        listbox.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-05-26
 * @brief       GUI-�б�� ����
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
 * V1.1 20140810
 * 1,�����Գ���Ŀ�Ĺ�����ʾ����
 * 2,����ԭ�����ڵ�һЩСbug
 * 3,�޸Ĺ�����������,ʹ�ø�����
 * 4,ȥ��LBOX_ITEM_HEIGHT�궨��,���ñ�����ʽ����(��gui_phy�ṹ����������)
 * V1.2 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __LISTBOX_H
#define __LISTBOX_H

#include "guix.h"
#include "scrollbar.h"


/* listbox�ϵķ��ŵ�Ĭ����ɫ */
#define LBOX_DFT_LBKCOLOR       0XFFFF      /* �ڲ�����ɫ */
#define LBOX_DFT_LSELCOLOR      0XFFFF      /* ѡ��list���������ɫ */
#define LBOX_DFT_LSELBKCOLOR    0X001F      /* ѡ��list��ı���ɫ */
#define LBOX_DFT_LNCOLOR        0X0000      /* δѡ�е�list������ɫ */
#define LBOX_DFT_RIMCOLOR       0XF800      /* �߿���ɫ */

/* listboxtype */
#define LBOX_TYPE_RIM           0X01        /* �б߿�� */

/* ������� */
#define LBOX_SCB_WIDTH          0X0E        /* ���������Ϊ15������ */


/* list�ṹ��.����ṹ */
typedef __PACKED_STRUCT
{
    void *prevlist;
    void *nextlist;
    uint32_t id;
    uint8_t *name;
} _listbox_list;

/* listbox�ṹ�嶨�� */
typedef __PACKED_STRUCT
{
    uint16_t top;           /* listbox�������� */
    uint16_t left;          /* listbox������� */
    uint16_t width;         /* ��� */
    uint16_t height;        /* �߶� ����Ϊ12/16�ı��� */

    uint8_t type;           /**
                             * ���ͱ����
                             * [bit7]:1,��Ҫ������������(������totalitems>itemsperpage);0,����Ҫ������.(��λ������Զ�����)
                             * [bit6:0]:����
                             */
    
    uint8_t sta;            /* listbox״̬,[bit7]:������־;[bit6]:�����Ч�ı�־;[bit5:0]:��һ�ΰ��µı�� */
    uint8_t id;             /* listbox ��id */
    uint8_t dbclick;        /* ˫��, [7]:0,û��˫��.1,��˫��;  [6~0]:0,����. */

    uint8_t font;           /* �������� 12/16 */
    uint16_t selindex;      /* ѡ�е����� */

    uint16_t lbkcolor;      /* �ڲ�������ɫ */
    uint16_t lnselcolor;    /* list name ѡ�к����ɫ */
    uint16_t lnselbkcolor;  /* list name ѡ�к�ı�����ɫ */
    uint16_t lncolor;       /* list name δѡ�е���ɫ */
    uint16_t rimcolor;      /* �߿���ɫ */


    uint8_t *fname;         /* ��ǰѡ�е�index������ */
    uint16_t namelen;       /* name��ռ�ĵ��� */
    uint16_t curnamepos;    /* ��ǰ��ƫ�� */
    uint32_t oldtime;       /* ��һ�θ���ʱ�� */

    _scrollbar_obj *scbv;   /* ��ֱ������ */
    _listbox_list *list;    /* ���� */
} _listbox_obj;


_listbox_obj *listbox_creat(uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint8_t type, uint8_t font); /* ����listbox */
_listbox_list *list_search(_listbox_list *listx, uint16_t index);   /* ���ұ��Ϊindex��list,��ȡ����ϸ��Ϣ */
void listbox_delete(_listbox_obj *listbox_del);             /* ɾ��listbox */
uint8_t listbox_check(_listbox_obj *listbox, void *in_key); /* ���listbox�İ���״̬ */
uint8_t listbox_addlist(_listbox_obj *listbox, uint8_t *name);      /* ����һ��list */
void listbox_draw_list(_listbox_obj *listbox);              /* ��list */
void listbox_draw_listbox(_listbox_obj *listbox);           /* �ػ�listbox */
void listbox_2click_hook(_listbox_obj *listbox);            /* ˫�����Ӻ��� */

#endif

























