/**
 ****************************************************************************************************
 * @file        can.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       CAN ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __CAN_H
#define __CAN_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* CAN ���� ���� */

#define CAN_RX_GPIO_PORT                GPIOA
#define CAN_RX_GPIO_PIN                 SYS_GPIO_PIN11
#define CAN_RX_GPIO_CLK_ENABLE()        do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA��ʱ��ʹ�� */

#define CAN_TX_GPIO_PORT                GPIOA
#define CAN_TX_GPIO_PIN                 SYS_GPIO_PIN12
#define CAN_TX_GPIO_CLK_ENABLE()        do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA��ʱ��ʹ�� */

/******************************************************************************************/

/* CAN����RX0�ж�ʹ�� */
#define CAN_RX0_INT_ENABLE      0               /* 0,��ʹ��; 1,ʹ��; */

/* ��̬����, ��can.c���Ե��� */
static uint8_t can_pend_msg(uint8_t fifox);     /* CAN ��ѯ���� */
static uint8_t can_tx_status(uint8_t mbox);     /* CAN ��ѯ����״̬ */
static uint8_t can_tx_msg(uint32_t id, uint8_t ide, uint8_t rtr, uint8_t len, uint8_t *data);
static void can_rx_msg(uint8_t fifox, uint32_t *id, uint8_t *ide, uint8_t *rtr, uint8_t *len, uint8_t *data);

/* ����ӿں��� */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf);             /* CAN��������, ��ѯ */
uint8_t can_send_msg(uint32_t id, uint8_t *msg, uint8_t len);   /* CAN�������� */
uint8_t can_init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode); /* CAN��ʼ�� */

#endif

















