/**
 ****************************************************************************************************
 * @file        adc3.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-24
 * @brief       ADC3 ��������
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
 * V1.1 20200424
 * 1, �޸�adc3_init, ��Ӷ�ADC3_CHY_GPIO��صĳ�ʼ��
 * 2, ��ͷ�ļ�������� ADC3_CHY ��غ궨��
 ****************************************************************************************************
 */

#ifndef __ADC3_H
#define __ADC3_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ADC3������ ���� */

#define ADC3_CHY_GPIO_PORT              GPIOA
#define ADC3_CHY_GPIO_PIN               SYS_GPIO_PIN1 
#define ADC3_CHY_GPIO_CLK_ENABLE()      do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA��ʱ��ʹ�� */

#define ADC3_CHY                        1                                       /* ͨ��Y,  0 <= Y <= 17 */ 

/******************************************************************************************/

void adc3_init(void);                               /* ADC3��ʼ�� */
void adc3_channel_set(uint8_t ch, uint8_t stime);   /* ADC3ͨ������ */
uint32_t adc3_get_result(uint8_t ch);               /* ���ĳ��ͨ��ֵ  */
uint32_t adc3_get_result_average(uint8_t ch, uint8_t times);/* �õ�ĳ��ͨ����������������ƽ��ֵ */

#endif 















