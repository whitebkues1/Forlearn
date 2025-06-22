/**
 ****************************************************************************************************
 * @file        facereco.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-����ʶ�� ����
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
 * V1.1 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __FACEFRECO_H
#define __FACEFRECO_H

#include "common.h"


/* ��������ڵ� */
typedef  struct __frec_node
{
    uint8_t *pname;             /* ����ָ�� */
    uint8_t *pnum;              /* �ֻ�����ָ�� */
    uint8_t *sex;               /* �Ա��ַ���ָ�� */
    uint8_t index;              /* ������,�����λ��,Ҳ����ʶ��ID. */
    struct __frec_node *next;   /* ָ����һ���ڵ� */
} frec_node;

/* ����ʶ����ƽṹ�� */
typedef __PACKED_STRUCT
{
    uint16_t xoff;              /* ����ͷ��LCD������ʾ,x�����ƫ�� */
    uint16_t yoff;              /* ����ͷ��LCD������ʾ,y�����ƫ�� */
    uint16_t width;             /* ����ͷ��LCD������ʾ�Ŀ�� */
    uint16_t height;            /* ����ͷ��LCD������ʾ�ĸ߶� */
    uint16_t *databuf;          /* ͼ�����ݻ����� */
    frec_node *face;            /* ������Ϣ */
} _frec_obj;

extern _frec_obj frec_dev;      /* ����ʶ����ƽṹ�� */
extern uint16_t frec_curline;   /* ����ͷ�������,��ǰ�б��,����ʶ���� */
extern volatile uint8_t memshow_flag;   /* ����ʶ�������б�־ */

frec_node *frec_node_creat(void);

void frec_node_free(frec_node *pnode, uint8_t mode);
uint8_t frec_node_insert(frec_node *head, frec_node *pnew, uint16_t i);
uint8_t frec_node_delete(frec_node *head, uint16_t i);
frec_node *frec_node_getnode(frec_node *head, uint16_t i);
uint16_t frec_node_getsize(frec_node *head);
void frec_node_destroy(frec_node *head);


void frec_set_image_center(_frec_obj *frecdev);
void frec_get_image_data(uint16_t *dbuf, uint16_t xoff, uint16_t yoff, uint16_t xsize, uint16_t width);

uint8_t frec_add_a_face_info(_frec_obj *frecdev);

uint8_t appplay_frec_read_a_face(frec_node *pnode, _frec_obj *frecdev, uint8_t index);

uint8_t appplay_frec_add_a_face(frec_node *pnode, _frec_obj *frecdev, uint8_t mode);

void frec_show_picture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
uint8_t frec_play(void);

#endif













