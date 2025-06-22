/**
 ****************************************************************************************************
 * @file        exeplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-11-16
 * @brief       APP-������ ����
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
 * V1.1 20221116
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __EXEPLAY_H
#define __EXEPLAY_H

#include "common.h"


/** 
 * APP�����Ϣ����
 * ˼·:��BIN�ļ��ȴ�ŵ��ⲿSRAM,Ȼ�����ñ�־λ,����һ����λ.��λ֮��,ϵͳ�жϱ�־λ,
 * �����Ҫ����app,���Ȱѱ�־λ���,Ȼ�����ⲿSRAM��APP���뵽�ڲ�sram,�����ת��app����ʼ
 * ��ַ,��ʼ����app����.
 * ע��:
 * ����Ĭ������APP�ĳߴ����ΪEXEPLAY_APP_SIZE�ֽ�.
 * ʵ��APP��С����ΪAPP_SIZE����ô��
 * SRAM�� 0X20010000 - APP_SIZE ~ 0X20010000, �������ΪAPP��SRAM��
 * SRAM�� 0X20001000 ~ 0X20001000 + APP_SIZE, �������ΪAPP��CODE��
 * ʵ����APP_SIZE����С��EXEPLAY_APP_SIZE
 */

#define EXEPLAY_APP_SIZE    55*1024             /* app��������ߴ�.����Ϊ60K�ֽڣ���������SRAM��ʵ���ϲ���������55K�Ĵ��룩. */
#define EXEPLAY_APP_BASE    0x20001000          /* appִ�д����Ŀ�ĵ�ַ,Ҳ���ǽ�Ҫ���еĴ����ŵĵ�ַ */
#define	EXEPLAY_SRC_BASE    1024*1024*15.9      /* appִ�д����Դ��ַ,Ҳ������λ֮ǰ,app�����ŵĵ�ַ */


typedef  void (*dummyfun)(void);    /* ����һ���������� */
extern dummyfun jump2app;           /* �ٺ���,��PCָ���ܵ��µ�main����ȥ */
void exeplay_write_appmask(uint16_t val);
void exeplay_app_check(void);
uint8_t exe_play(void);

#endif
































