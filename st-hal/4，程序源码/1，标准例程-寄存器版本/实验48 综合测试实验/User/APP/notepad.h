/**
 ****************************************************************************************************
 * @file        notepad.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-���±� ����
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

#ifndef __NOTEPAD_H
#define __NOTEPAD_H

#include "common.h"


/* �������ı༭����.Ҳ�����½��ı��ļ��Ĵ�С.���ߴ�һ�����ļ�������������ӵĳ��� */
#define     NOTEPAD_EDIT_LEN    2048


void notepad_new_pathname(uint8_t *pname);
uint8_t notepad_play(void);

#endif























