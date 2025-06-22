/**
 ****************************************************************************************************
 * @file        gsm.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-31
 * @brief       GSMģ�� ��������
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
 * V1.0 20221031
 * ��һ�η���
 * ����4G CAT1 GM196Hģ��, 2G GSMҲͨ��
 *
 ****************************************************************************************************
 */

#ifndef __GSM_H
#define __GSM_H

#include "./SYSTEM/sys/sys.h"


#define GSM_MAX_NEWMSG      10      /* ���10������Ϣ */

typedef struct
{
    uint8_t status;     /**
                         * gsm״̬
                         * bit7:0,û���ҵ�ģ��;1,�ҵ�ģ����
                         * bit6:0,SIM��������;1,SIM������
                         * bit5:0,δ�����Ӫ������;1,�ѻ����Ӫ������
                         * bit4:0,�й��ƶ�;1,�й���ͨ
                         * [3:0]:����
                         */
    
    uint8_t mode;       /**
                         * ��ǰ����ģʽ
                         * 0,��������ģʽ/����ģʽ
                         * 1,������
                         * 2,ͨ����
                         * 3,������Ӧ��
                         */

    volatile uint8_t cmdon;     /**
                                 * ����Ƿ���ָ���ڷ��͵ȴ�״̬
                                 * 0,û��ָ���ڵȴ���Ӧ
                                 * 1,��ָ���ڵȴ���Ӧ
                                 */

    uint8_t csq;                /* �ź����� */

    volatile uint8_t newmsg;    /* ����Ϣ����,0,û������Ϣ;����,����Ϣ���� */
    uint8_t newmsgindex[GSM_MAX_NEWMSG];    /* �¶�����SIM���ڵ�����,���¼GSM_MAX_NEWMSG���¶��� */
    uint8_t incallnum[20];      /* ������뻺����,�20λ */
} __gsmdev;

extern __gsmdev gsmdev;         /* gsm������ */


#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8 /* �ߵ��ֽڽ����궨�� */


uint8_t *gsm_check_cmd(uint8_t *str);
uint8_t gsm_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t waittime);
void gsm_cmd_over(void);
uint8_t gsm_chr2hex(uint8_t chr);
uint8_t gsm_hex2chr(uint8_t hex);
void gsm_unigbk_exchange(uint8_t *src, uint8_t *dst, uint8_t mode);
void gsm_cmsgin_check(void);
void gsm_status_check(void);

#endif





