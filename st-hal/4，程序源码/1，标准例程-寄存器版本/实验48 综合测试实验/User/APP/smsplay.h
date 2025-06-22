/**
 ****************************************************************************************************
 * @file        smsplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-���Ų��� ����
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

#ifndef __SMSPLAY_H
#define __SMSPLAY_H

#include "common.h"
#include "./BSP/GSM/gsm.h"


#define SMS_RMEMO_MAXLEN        2000        /* ����������2K�ֽ����� */

#define SMS_PNUM_BACK_COLOR     0X42F0      /* �ֻ�����(��ϵ��/�ռ���)���򱳾�ɫ */
#define SMS_PNUM_NAME_COLOR     WHITE       /* ��ϵ��/�ռ���������ɫ */
#define SMS_PNUM_NUM_COLOR      WHITE       /* ������ɫ */

#define SMS_COM_RIM_COLOR       0X9D36      /* ͨ�÷ָ��� */

#define SMS_RMSG_BACK_COLOR     0XBE3B      /* ���Ž���ʱ�����򱳾�ɫ */
#define SMS_RMSG_FONT_COLOR     BLACK       /* ʱ��������ɫ */

#define SMS_RMEMO_BACK_COLOR    WHITE       /* ���յĶ���MEMO�ؼ�����ɫ */
#define SMS_RMEMO_FONT_COLOR    BLACK       /* ���յĶ���������ɫ */

#define SMS_SAREA_BACK_COLOR    0XA599      /* �������򱳾�ɫ */
#define SMS_SMEMO_BACK_COLOR    WHITE       /* ���Ͷ���MEMO�ؼ�����ɫ */
#define SMS_SMEMO_FONT_COLOR    BLACK       /* ���Ͷ���������ɫ */

/* ��������ڵ� */
typedef  struct __sms_node
{
    uint8_t *pnum;      /* �绰����ָ�� */
    uint8_t *msg;       /* ��������ָ�� */
    uint8_t *time;      /* ����ʱ��ָ�� */
    uint8_t *item;      /* ����չʾ��ָ��=pnum+msg+time */
    uint8_t index;      /* �ö�����SIM����λ�� */
    uint8_t readed;     /* 0,δ������;1,�Ѷ����� */
    struct __sms_node *next;    /* ָ����һ���ڵ� */
} sms_node;

sms_node *sms_node_creat(void);

void sms_node_free(sms_node *pnode, uint8_t mode);
uint8_t sms_node_insert(sms_node *head, sms_node *pnew, uint16_t i);
uint8_t sms_node_delete(sms_node *head, uint16_t i);
sms_node *sms_node_getnode(sms_node *head, uint16_t i);
uint16_t sms_node_getsize(sms_node *head);
void sms_node_destroy(sms_node *head);

void sms_remind_msg(uint8_t mode);
uint8_t sms_read_sms(sms_node *pnode, uint16_t index, uint8_t mode);
uint8_t sms_delete_sms(sms_node **head, uint16_t index);
uint8_t sms_get_smsnum(uint16_t *num, uint16_t *max);
uint8_t sms_msg_select(uint16_t *sel, uint16_t *top, uint8_t *caption, sms_node *head);
uint16_t sms_read_all_sms(sms_node *head, uint16_t *maxnum);
void sms_send_sms(sms_node *pnode, uint8_t mode);
uint8_t sms_play(void);

#endif



















