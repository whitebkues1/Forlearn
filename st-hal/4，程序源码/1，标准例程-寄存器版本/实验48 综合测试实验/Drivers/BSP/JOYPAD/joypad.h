/**
 ****************************************************************************************************
 * @file        joypad.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-26
 * @brief       ��Ϸ�ֱ� ��������
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
 * V1.0 20200426
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __JOYPAD_H
#define __JOYPAD_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* ���� ���� */

#define JOYPAD_CLK_GPIO_PORT            GPIOD
#define JOYPAD_CLK_GPIO_PIN             SYS_GPIO_PIN3
#define JOYPAD_CLK_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 5; }while(0)   /* PD��ʱ��ʹ�� */

#define JOYPAD_LAT_GPIO_PORT            GPIOB
#define JOYPAD_LAT_GPIO_PIN             SYS_GPIO_PIN11
#define JOYPAD_LAT_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 3; }while(0)   /* PB��ʱ��ʹ�� */

#define JOYPAD_DATA_GPIO_PORT           GPIOB
#define JOYPAD_DATA_GPIO_PIN            SYS_GPIO_PIN10
#define JOYPAD_DATA_GPIO_CLK_ENABLE()   do{ RCC->APB2ENR |= 1 << 3; }while(0)   /* PB��ʱ��ʹ�� */

/******************************************************************************************/

/* �ֱ��������� */
#define JOYPAD_CLK(x)   sys_gpio_pin_set(JOYPAD_CLK_GPIO_PORT, JOYPAD_CLK_GPIO_PIN, x)  /* JOYPAD_CLK */
#define JOYPAD_LAT(x)   sys_gpio_pin_set(JOYPAD_LAT_GPIO_PORT, JOYPAD_LAT_GPIO_PIN, x)  /* JOYPAD_LATCH */
#define JOYPAD_DATA     sys_gpio_pin_get(JOYPAD_DATA_GPIO_PORT, JOYPAD_DATA_GPIO_PIN)   /* JOYPAD_DATA */


/* ��̬���� */
static void joypad_delay(uint16_t t);   /* JOYPAD ��ʱ */

/* �ӿں��� */
void joypad_init(void);     /* JOYPAD ��ʼ�� */
uint8_t joypad_read(void);  /* JOYPAD ��ȡ���� */

#endif
















