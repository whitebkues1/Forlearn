/**
 ****************************************************************************************************
 * @file        spb.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-10-26
 * @brief       SPBЧ��ʵ�� ����(103/407ϵ������)
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
 * 2, �޸�uint8_t/uint16_t/uint32_tΪuint8_t/uint16_t/uint32_t
 *
 * V1.2 20221026
 * 1, ����103��407���ص�,���������
 ****************************************************************************************************
 */

#ifndef __SPB_H
#define __SPB_H

#include "./BSP/SPBLCD/spblcd.h"

/* ֧�ֵ��������� */
#define GUI_LANGUAGE_NUM    3


#define SPB_PAGE_NUM        2       /* SPBҳ��,ÿҳ����8��ͼ�� */
#define SPB_ICOS_NUM        15      /* SPB����������ͼ���� */

#define SPB_MOVE_WIN        2       /* SPB������ⴰ��ֵ */
#define SPB_MOVE_MIN        8       /* SPB��Ϊ�ǻ�������Сֵ,��������Ϊ�ǻ���,���ǲ�һ���ᷭҳ */
#define SPB_MOVE_ACT        50      /* SPB����������ֵ,�������ֵ������з�ҳ */


#define SPB_ALPHA_VAL       18      /* SPBѡ��͸�������� */
#define SPB_ALPHA_COLOR     WHITE   /* SPB͸��ɫ */
#define SPB_FONT_COLOR      BLUE    /* SPB������ɫ */
#define SPB_MICO_BKCOLOR    0XA5BB  /* micoͼ�걳��ɫ */


extern uint8_t *const spb_bkpic_path_tbl[3][4];
extern uint8_t *const spb_icos_path_tbl[3][SPB_ICOS_NUM];
extern uint8_t *const spb_micos_path_tbl[3][3];

/* SPB ͼ����ƽṹ�� */
typedef __PACKED_STRUCT _m_spb_icos
{
    uint16_t x;         /* ͼ�����꼰�ߴ� */
    uint16_t y;
    uint8_t width;
    uint8_t height;
    uint8_t *path;      /* ͼ��·��ָ�� */
    uint8_t *name;      /* ͼ������ָ�� */
} m_spb_icos;


/* SPB ������ */
typedef struct _m_spb_dev
{
    uint16_t oldxpos;       /* ��һ��TP��x����λ�� */
    uint16_t curxpos;       /* ��ǰtp���µ�x���� */
    uint16_t curypos;       /* ��ǰtp���µ�y���� */
    uint16_t spbsta;        /**
                             * spb״̬ 
                             * [15]:��һ�ΰ��±�־
                             * [14]:������־;
                             * [13~0]:��������
                             */

    uint16_t pos;           /* ��ǰ֡��λ��(x����) */
    uint16_t oldpos;        /* ��������ʱ��֡λ�� */

    uint8_t stabarheight;   /* ״̬���߶� */
    uint16_t spbheight;     /* SPB��������߶� */
    uint16_t spbwidth;      /* SPB����������,һ�����LCD�ֱ��ʿ�� */
    uint16_t spbahwidth;    /* SPB����Ԥ�����,һ�����LCD��ȵ�1/4 */
    uint16_t spbfontsize;   /* SPB�����С */

    uint8_t selico;         /* ��ǰѡ�е�ͼ��: 0~8,��ѡ�е�ͼ����; ����,û���κ�ͼ�걻ѡ�� */
    m_spb_icos icos[SPB_ICOS_NUM];  /* ��ͼ���� */
    m_spb_icos micos[3];            /* 3����ͼ�� */
} m_spb_dev;

extern m_spb_dev spbdev;


uint8_t spb_init(uint8_t mode);
uint8_t spb_load_icos(uint8_t idx);

uint8_t spb_load_micos(void);
void spb_gsm_signal_show(uint16_t x, uint16_t y, uint8_t signal);
void spb_stabar_msg_show(uint8_t clr);
uint8_t spb_load_mui(void);
void spb_frame_move(uint8_t dir, uint8_t skips, uint16_t pos);
void spb_unsel_micos(uint8_t selx);
void spb_set_sel(uint8_t sel);
uint8_t spb_move_chk(void);

#endif




















