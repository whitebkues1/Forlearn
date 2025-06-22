/**
 ****************************************************************************************************
 * @file        adc3.c
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

#include "./BSP/ADC/adc3.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ADC3��ʼ������
 *   @note      ������֧��ADC3����ͨ��, ���ǲ�֧��ADC1 / ADC2
 *              ����ʹ��12λ����, ADC����ʱ��=12M, ת��ʱ��Ϊ: �������� + 12.5��ADC����
 *              ��������������: 239.5, ��ת��ʱ�� = 252 ��ADC���� = 21us
 * @param       ��
 * @retval      ��
 */
void adc3_init(void)
{
    ADC3_CHY_GPIO_CLK_ENABLE();     /* IO��ʱ��ʹ�� */

    sys_gpio_set(ADC3_CHY_GPIO_PORT, ADC3_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */

    RCC->APB2ENR |= 1 << 15;        /* ADC3ʱ��ʹ�� */
    RCC->APB2RSTR |= 1 << 15;       /* ADC3��λ */
    RCC->APB2RSTR &= ~(1 << 15);    /* ADC3������λ */
    
    /* ADCʱ������ APB2, ��PCLK2, Ƶ��Ϊ72Mhz, ADC���ʱ��һ�㲻Ҫ����14M
     * ��72M��Ƶ������, ����ʹ��6��Ƶ, �õ�PCLK2 / 6 = 12Mhz ��ADCʱ��
     */
    RCC->CFGR &= ~(3 << 14);        /* ADCPRE[1:0] ADCʱ��Ԥ��Ƶ���� */
    RCC->CFGR |= 2 << 14;           /* ����ADCʱ��Ԥ��Ƶϵ��Ϊ 6, �� PCLK2 / 6 = 12Mhz */

    ADC3->CR1 &= ~(0XF << 16);  /* ����ģʽ���� */
    ADC3->CR1 |= 0 << 16;       /* ��������ģʽ */
    ADC3->CR1 &= ~(1 << 8);     /* ��ɨ��ģʽ */

    ADC3->CR2 &= ~(1 << 1);     /* ����ת��ģʽ */
    ADC3->CR2 &= ~(7 << 17);
    ADC3->CR2 |= 7 << 17;       /* �������ת�� */
    ADC3->CR2 |= 1 << 20;       /* ʹ�����ⲿ����(SWSTART)!!! ����ʹ��һ���¼������� */
    ADC3->CR2 &= ~(1 << 11);    /* �Ҷ��� */

    ADC3->SQR1 &= ~(0XF << 20); /* L[3:0]���� */
    ADC3->SQR1 |= 0 << 20;      /* 1��ת���ڹ��������� Ҳ����ֻת����������1 */

    ADC3->CR2 |= 1 << 0;        /* ����ADת���� */

    ADC3->CR2 |= 1 << 3;        /* ʹ�ܸ�λУ׼ */

    while (ADC3->CR2 & 1 << 3); /* �ȴ�У׼���� */

    /* ��λ��������ò���Ӳ�����  ��У׼�Ĵ�������ʼ�����λ������� */
    ADC3->CR2 |= 1 << 2;        /* ����ADУ׼ */

    while (ADC3->CR2 & 1 << 2); /* �ȴ�У׼���� */
}

/**
 * @brief       ����ADC3ͨ������ʱ��
 * @param       ch   : ͨ����, 0~17
 * @param       stime: ����ʱ��  0~7, ��Ӧ��ϵΪ:
 *   @arg       000, 1.5��ADCʱ������        001, 7.5��ADCʱ������
 *   @arg       010, 13.5��ADCʱ������       011, 28.5��ADCʱ������
 *   @arg       100, 41.5��ADCʱ������       101, 55.5��ADCʱ������
 *   @arg       110, 71.5��ADCʱ������       111, 239.5��ADCʱ������
 * @retval      ��
 */
void adc3_channel_set(uint8_t ch, uint8_t stime)
{
    if (ch < 10)              /* ͨ��0~9,ʹ��SMPR2���� */
    {
        ADC3->SMPR2 &= ~(7 << (3 * ch));        /* ͨ��ch ����ʱ����� */
        ADC3->SMPR2 |= 7 << (3 * ch);           /* ͨ��ch ������������,����Խ�߾���Խ�� */
    }
    else     /* ͨ��10~17,ʹ��SMPR1���� */
    {
        ADC3->SMPR1 &= ~(7 << (3 * (ch - 10))); /* ͨ��ch ����ʱ����� */
        ADC3->SMPR1 |= 7 << (3 * (ch - 10));    /* ͨ��ch ������������,����Խ�߾���Խ�� */
    }
}

/**
 * @brief       ���ADCת����Ľ��
 * @param       ch: ͨ����, 0~17
 * @retval      ��
 */
uint32_t adc3_get_result(uint8_t ch)
{
    adc3_channel_set(ch, 7);        /* ����ADCX��Ӧͨ������ʱ��Ϊ239.5��ʱ������ */

    ADC3->SQR3 &= ~(0X1F << 5 * 0); /* ��������1ͨ������ */
    ADC3->SQR3 |= ch << (5 * 0);    /* ��������1 ͨ�� = ch */
    ADC3->CR2 |= 1 << 22;           /* ��������ת��ͨ�� */

    while (!(ADC3->SR & 1 << 1));   /* �ȴ�ת������ */

    return ADC3->DR;                /* ����adcֵ */
}

/**
 * @brief       ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
 * @param       ch      : ͨ����, 0~17
 * @param       times   : ��ȡ����
 * @retval      ͨ��ch��times��ת�����ƽ��ֵ
 */
uint32_t adc3_get_result_average(uint8_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)   /* ��ȡtimes������ */
    {
        temp_val += adc3_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* ����ƽ��ֵ */
}


















