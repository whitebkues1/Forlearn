/**
 ****************************************************************************************************
 * @file        settings.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-���� ����
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

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "./SYSTEM/sys/sys.h"
//#include "./BSP/VS10XX/vs10xx.h"
#include "os.h"
#include "common.h"
//#include "es8388.h"


/* list�ṹ��.����ṹ */
typedef __PACKED_STRUCT
{
    uint8_t syslanguage;        /* Ĭ��ϵͳ���� */
    uint8_t lcdbklight;         /* LED�������� 10~110.10�����;110�������� */
    uint8_t picmode;            /* ͼƬ���ģʽ:0,˳��ѭ������;1,������� */
    uint8_t audiomode;          /* ��Ƶ����ģʽ:0,˳��ѭ������;1,�������;2,����ѭ������ */
    uint8_t videomode;          /* ��Ƶ����ģʽ:0,˳��ѭ������;1,�������;2,����ѭ������ */
    uint8_t saveflag;           /* �����־,0X0A,�������;����,����δ���� */
} _system_setings;

extern _system_setings systemset;   /* ��settings.c�������� */

uint8_t sysset_time_set(uint16_t x, uint16_t y, uint8_t *hour, uint8_t *min, uint8_t *caption);
uint8_t sysset_date_set(uint16_t x, uint16_t y, uint16_t *year, uint8_t *month, uint8_t *date, uint8_t *caption);
uint8_t sysset_bklight_set(uint16_t x, uint16_t y, uint8_t *caption, uint16_t *bkval);
uint8_t sysset_system_update_cpymsg(uint8_t *pname, uint8_t pct, uint8_t mode);
void sysset_system_update(uint8_t *caption, uint16_t sx, uint16_t sy);
void sysset_system_info(uint16_t x, uint16_t y, uint8_t *caption);
void sysset_system_status(uint16_t x, uint16_t y, uint8_t *caption);
void sysset_system_about(uint16_t x, uint16_t y, uint8_t *caption);
uint8_t *set_search_caption(const uint8_t *mcaption);
void sysset_read_para(_system_setings *sysset);
void sysset_save_para(_system_setings *sysset);

uint8_t sysset_play(void);

#endif












