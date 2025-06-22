/**
 ****************************************************************************************************
 * @file        ds18b20.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-26
 * @brief       DS18B20�����¶ȴ����� ��������
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

#ifndef __DS18B20_H
#define __DS18B20_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* DS18B20���� ���� */

#define DS18B20_DQ_GPIO_PORT                GPIOG
#define DS18B20_DQ_GPIO_PIN                 SYS_GPIO_PIN11
#define DS18B20_DQ_GPIO_CLK_ENABLE()        do{ RCC->APB2ENR |= 1 << 8; }while(0)   /* PG��ʱ��ʹ�� */

/******************************************************************************************/

/* IO�������� */
#define DS18B20_DQ_OUT(x)       sys_gpio_pin_set(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, x)  /* ���ݶ˿���� */
#define DS18B20_DQ_IN           sys_gpio_pin_get(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN)     /* ���ݶ˿����� */


uint8_t ds18b20_init(void);         /* ��ʼ��DS18B20 */
uint8_t ds18b20_check(void);        /* ����Ƿ����DS18B20 */
short ds18b20_get_temperature(void);/* ��ȡ�¶� */

#endif















