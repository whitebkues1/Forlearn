/**
 ****************************************************************************************************
 * @file        netplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-26
 * @brief       APP-������� ����
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
 ****************************************************************************************************
 */

#ifndef __NETPLAY_H
#define __NETPLAY_H

#include "./BSP/CH395Q/ch395q.h"

#define NET_RECV_BUF_MAX_LEN    512     /* Socket���ݽ��ջ�������С */
#define NET_RMEMO_MAXLEN        10000   /* �������ı����ı���󳤶� */
#define NET_SMEMO_MAXLEN        400     /* �������ı����ı���󳤶� */
#define NET_TCP_SERVER_MAX_CONN 7       /* TCP Serverģʽ������������������Χ��1~7 */

#define NET_IP_BACK_COLOR       0x42F0  /* IP��ַ���򱳾���ɫ */
#define NET_MEMO_BACK_COLOR     0xA599  /* �ָ�����ɫ */
#define NET_MSG_FONT_COLOR      0x4A49  /* ��ʾ��Ϣ������ɫ */
#define NET_MSG_BACK_COLOR      0xBE3B  /* ��ʾ��Ϣ������ɫ */
#define NET_COM_RIM_COLOR       0x7BCF  /* ���ա�������������ɫ */

/* �������� */
uint8_t net_play(void);

#endif
