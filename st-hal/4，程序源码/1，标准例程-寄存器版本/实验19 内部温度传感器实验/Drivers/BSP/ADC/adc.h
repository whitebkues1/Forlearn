/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.3
 * @date        2020-04-24
 * @brief       ADC ��������
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
 * V1.0 20200423
 * ��һ�η���
 * V1.1 20200423
 * 1,֧��ADC��ͨ��DMA�ɼ� 
 * 2,����adc_dma_init��adc_dma_enable����.
 * V1.2 20200423
 * 1,֧��ADC��ͨ��DMA�ɼ� 
 * 2,����adc_nch_dma_init����.
 * V1.3 20200424
 * 1,֧���ڲ��¶ȴ������¶Ȳɼ�
 * 2,����adc_temperature_init��adc_get_temperature����.
 *
 ****************************************************************************************************
 */

#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ADC������ ���� */

#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADCX_CHY_GPIO_PIN               SYS_GPIO_PIN1 
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA��ʱ��ʹ�� */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CHY                        1                               /* ͨ��Y,  0 <= Y <= 17 */ 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ RCC->APB2ENR |= 1 << 9; }while(0)   /* ADC1 ʱ��ʹ�� */


/* ADC��ͨ��/��ͨ�� DMA�ɼ� DMA��ͨ�� ����
 * ע��: ADC1��DMAͨ��ֻ����: DMA1_Channel1, ���ֻҪ��ADC1, �����ǲ��ܸĶ���
 *       ADC2��֧��DMA�ɼ�
 *       ADC3��DMAͨ��ֻ����: DMA2_Channel5, ������ʹ�� ADC3 ����Ҫ�޸�
 */
#define ADC_ADCX_DMACx                      DMA1_Channel1
#define ADC_ADCX_DMACx_IRQn                 DMA1_Channel1_IRQn
#define ADC_ADCX_DMACx_IRQHandler           DMA1_Channel1_IRQHandler

#define ADC_ADCX_DMACx_IS_TC()              ( DMA1->ISR & (1 << 1) )    /* �ж� DMA1_Channel1 ������ɱ�־, ����һ���ٺ�����ʽ,
                                                                         * ���ܵ�����ʹ��, ֻ������if��������� 
                                                                         */
#define ADC_ADCX_DMACx_CLR_TC()             do{ DMA1->IFCR |= 1 << 1; }while(0) /* ��� DMA1_Channel1 ������ɱ�־ */


/* ADC �¶ȴ�����ͨ�� ���� */ 

#define ADC_TEMPSENSOR_CHX                  16

/******************************************************************************************/


void adc_init(void);                /* ADC��ʼ�� */
uint32_t adc_get_result(uint8_t ch);/* ���ĳ��ͨ��ֵ  */
void adc_channel_set(ADC_TypeDef *adcx, uint8_t ch, uint8_t stime); /* ADCͨ������ */
uint32_t adc_get_result_average(uint8_t ch, uint8_t times); /* �õ�ĳ��ͨ����������������ƽ��ֵ */
void  adc_dma_init(uint32_t mar);       /* ADC DMA�ɼ���ʼ�� */
void adc_dma_enable( uint16_t cndtr);   /* ʹ��һ��ADC DMA�ɼ����� */
void adc_nch_dma_init(uint32_t mar);    /* ADC��ͨ�� DMA�ɼ���ʼ�� */
void adc_temperature_init(void);        /* ADC�¶Ȳɼ���ʼ������ */
short adc_get_temperature(void);        /* ��ȡ�ڲ��¶ȴ������¶�ֵ */

#endif 















