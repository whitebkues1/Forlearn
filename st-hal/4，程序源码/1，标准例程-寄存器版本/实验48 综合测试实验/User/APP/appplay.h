/**
 ****************************************************************************************************
 * @file        appplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-11-01
 * @brief       APP-Ӧ������ ����
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
 * V1.0 20221101
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __APPPLAY_H
#define __APPPLAY_H
#include "common.h"


#define APPPLAY_EX_BACKCOLOR    0X0000          /* �����ⲿ����ɫ */
#define APPPLAY_IN_BACKCOLOR    0X8C51          /* �����ڲ�����ɫ */
#define APPPLAY_NAME_COLOR      0X001F          /* ��������ɫ */

#define APPPLAY_ALPHA_VAL       18              /* APPѡ��͸�������� */
#define APPPLAY_ALPHA_COLOR     WHITE           /* APP͸��ɫ */

/* ��ͼ��/ͼƬ·�� */
extern uint8_t *const appplay_icospath_tbl[3][16];  /* icos��·���� */
extern uint8_t *const appplay_appname_tbl[3][16];   /* icos���� */

/* APPͼ��������� */
typedef __PACKED_STRUCT _m_app_icos
{
    uint16_t x;         /* ͼ�����꼰�ߴ� */
    uint16_t y;
    uint8_t width;
    uint8_t height;
    uint8_t *path;      /* ͼ��·�� */
    uint8_t *name;      /* ͼ������ */
} m_app_icos;

/* APP������ */
typedef struct _m_app_dev
{
    uint8_t selico; /**
                     * ��ǰѡ�е�ͼ��
                     * 0~15,��ѡ�е�ͼ����
                     * ����,û���κ�ͼ�걻ѡ��
                     */
    
    m_app_icos icos[14];    /* �ܹ�16��ͼ�� */
} m_app_dev;


uint8_t app_play(void);

#endif























