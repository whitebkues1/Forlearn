/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2020-04-23
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
 *
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


uint8_t g_adc_dma_sta = 0;  /* DMA����״̬��־, 0,δ���; 1, ����� */

/**
 * @brief       ADC��ʼ������
 *   @note      ������֧��ADC1/ADC2����ͨ��, ���ǲ�֧��ADC3
 *              ����ʹ��12λ����, ADC����ʱ��=12M, ת��ʱ��Ϊ: �������� + 12.5��ADC����
 *              ��������������: 239.5, ��ת��ʱ�� = 252 ��ADC���� = 21us
 * @param       ��
 * @retval      ��
 */
void adc_init(void)
{
    ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* IO��ʱ��ʹ�� */
    ADC_ADCX_CHY_CLK_ENABLE();      /* ADCʱ��ʹ�� */

    sys_gpio_set(ADC_ADCX_CHY_GPIO_PORT, ADC_ADCX_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */

    RCC->APB2RSTR |= 3 << 9;        /* ADC1 & ADC2 ��λ */
    RCC->APB2RSTR &= ~(3 << 9);     /* ��λ���� */

    /* ADCʱ������ APB2, ��PCLK2, Ƶ��Ϊ72Mhz, ADC���ʱ��һ�㲻Ҫ����14M
     * ��72M��Ƶ������, ����ʹ��6��Ƶ, �õ�PCLK2 / 6 = 12Mhz ��ADCʱ��
     */
    RCC->CFGR &= ~(3 << 14);        /* ADCPRE[1:0] ADCʱ��Ԥ��Ƶ���� */
    RCC->CFGR |= 2 << 14;           /* ����ADCʱ��Ԥ��Ƶϵ��Ϊ 6, �� PCLK2 / 6 = 12Mhz */

    ADC_ADCX->CR1 &= ~(0XF << 16);  /* ����ģʽ���� */
    ADC_ADCX->CR1 |= 0 << 16;       /* ��������ģʽ */
    ADC_ADCX->CR1 &= ~(1 << 8);     /* ��ɨ��ģʽ */

    ADC_ADCX->CR2 &= ~(1 << 1);     /* ����ת��ģʽ */
    ADC_ADCX->CR2 &= ~(7 << 17);
    ADC_ADCX->CR2 |= 7 << 17;       /* �������ת�� */
    ADC_ADCX->CR2 |= 1 << 20;       /* ʹ�����ⲿ����(SWSTART)!!! ����ʹ��һ���¼������� */
    ADC_ADCX->CR2 &= ~(1 << 11);    /* �Ҷ��� */
    
    ADC_ADCX->SQR1 &= ~(0XF << 20); /* L[3:0]���� */
    ADC_ADCX->SQR1 |= 0 << 20;      /* 1��ת���ڹ��������� Ҳ����ֻת����������1 */

    ADC_ADCX->CR2 |= 1 << 0;        /* ����ADת���� */

    ADC_ADCX->CR2 |= 1 << 3;        /* ʹ�ܸ�λУ׼ */

    while (ADC_ADCX->CR2 & 1 << 3); /* �ȴ�У׼���� */

    /* ��λ��������ò���Ӳ�����  ��У׼�Ĵ�������ʼ�����λ������� */
    ADC_ADCX->CR2 |= 1 << 2;        /* ����ADУ׼ */

    while (ADC_ADCX->CR2 & 1 << 2); /* �ȴ�У׼���� */
}

/**
 * @brief       ����ADCͨ������ʱ��
 * @param       adcx : adc�ṹ��ָ��, ADC1 / ADC2
 * @param       ch   : ͨ����, 0~17
 * @param       stime: ����ʱ��  0~7, ��Ӧ��ϵΪ:
 *   @arg       000, 1.5��ADCʱ������        001, 7.5��ADCʱ������
 *   @arg       010, 13.5��ADCʱ������       011, 28.5��ADCʱ������
 *   @arg       100, 41.5��ADCʱ������       101, 55.5��ADCʱ������
 *   @arg       110, 71.5��ADCʱ������       111, 239.5��ADCʱ������
 * @retval      ��
 */
void adc_channel_set(ADC_TypeDef *adcx, uint8_t ch, uint8_t stime)
{
    if (ch < 10)    /* ͨ��0~9,ʹ��SMPR2���� */
    {
        adcx->SMPR2 &= ~(7 << (3 * ch));        /* ͨ��ch ����ʱ����� */
        adcx->SMPR2 |= 7 << (3 * ch);           /* ͨ��ch ������������,����Խ�߾���Խ�� */
    }
    else    /* ͨ��10~17,ʹ��SMPR1���� */
    {
        adcx->SMPR1 &= ~(7 << (3 * (ch - 10))); /* ͨ��ch ����ʱ����� */
        adcx->SMPR1 |= 7 << (3 * (ch - 10));    /* ͨ��ch ������������,����Խ�߾���Խ�� */
    }
}

/**
 * @brief       ���ADCת����Ľ��
 * @param       ch: ͨ����, 0~17
 * @retval      ��
 */
uint32_t adc_get_result(uint8_t ch)
{
    adc_channel_set(ADC_ADCX, ch, 7);   /* ����ADCX��Ӧͨ������ʱ��Ϊ239.5��ʱ������ */

    ADC_ADCX->SQR3 &= ~(0X1F << 5 * 0); /* ��������1ͨ������ */
    ADC_ADCX->SQR3 |= ch << (5 * 0);    /* ��������1 ͨ�� = ch */
    ADC_ADCX->CR2 |= 1 << 22;           /* ��������ת��ͨ�� */

    while (!(ADC_ADCX->SR & 1 << 1));   /* �ȴ�ת������ */

    return ADC_ADCX->DR;                /* ����adcֵ */
}

/**
 * @brief       ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
 * @param       ch      : ͨ����, 0~17
 * @param       times   : ��ȡ����
 * @retval      ͨ��ch��times��ת�����ƽ��ֵ
 */
uint32_t adc_get_result_average(uint8_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)   /* ��ȡtimes������ */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;    /* ����ƽ��ֵ */
}

/**
 * @brief       ADC DMA�ɼ��жϷ�����
 * @param       �� 
 * @retval      ��
 */
void ADC_ADCX_DMACx_IRQHandler(void)
{
    if (ADC_ADCX_DMACx_IS_TC())     /* DMA�������? */
    {
        g_adc_dma_sta = 1;          /* ���DMA������� */
        ADC_ADCX_DMACx_CLR_TC();    /* ���DMA��Ӧͨ�� ��������жϱ�־ */
    }
}

/**
 * @brief       ADC DMA��ȡ ��ʼ������
 *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
 * @param       mar         : �洢����ַ 
 * @retval      ��
 */
void adc_dma_init(uint32_t mar)
{
    adc_init(); /* �ȳ�ʼ��ADC */

    /* ����ADC����ת��, DMA����ADC���� */
    ADC_ADCX->CR2 |= 1 << 8;        /* DMA = 1, DMA����ʹ�� */
    ADC_ADCX->CR2 |= 1 << 1;        /* CONT = 1, ����ת��ģʽ */

    ADC_ADCX->SQR3 &= ~(0X1F << 5 * 0);         /* ��������1ͨ������ */
    ADC_ADCX->SQR3 |= ADC_ADCX_CHY << (5 * 0);  /* ��������1 ͨ�� = ADC_ADCX_CHY */

    adc_channel_set(ADC_ADCX, ADC_ADCX_CHY, 7); /* ����ADCX��Ӧͨ������ʱ��Ϊ239.5��ʱ������ */

    /* ʹ��DMAʱ��, ������CPAR��CMAR�Ĵ���, ���������ַΪ: &ADC1->DR */
    dma_basic_config(ADC_ADCX_DMACx, (uint32_t)&ADC1->DR, mar);

    /* ����DMA���� */
    ADC_ADCX_DMACx->CCR = 0;        /* ��λCCR�Ĵ��� */
    ADC_ADCX_DMACx->CCR |= 1 << 1;  /* TCIE =1 , ʹ�ܴ�������ж� */
    ADC_ADCX_DMACx->CCR |= 0 << 4;  /* DIR = 0 , ���赽�洢��ģʽ */
    ADC_ADCX_DMACx->CCR |= 0 << 5;  /* CIRC = 0, ��ѭ��ģʽ(��ʹ����ͨģʽ) */
    ADC_ADCX_DMACx->CCR |= 0 << 6;  /* PINC = 0, �����ַ������ģʽ */
    ADC_ADCX_DMACx->CCR |= 1 << 7;  /* MINC = 1, �洢������ģʽ */
    ADC_ADCX_DMACx->CCR |= 1 << 8;  /* PSIZE[1:0] = 1, �������ݿ��Ϊ: 16λ */
    ADC_ADCX_DMACx->CCR |= 1 << 10; /* MSIZE[1:0] = 1, �洢�����ݿ��: 16λ */
    ADC_ADCX_DMACx->CCR |= 1 << 12; /* PL[1:0] =  1, �е����ȼ� */
    ADC_ADCX_DMACx->CCR |= 0 << 14; /* MEM2MEM = 0 , �Ǵ洢�����洢��ģʽ */

    sys_nvic_init(3, 3, ADC_ADCX_DMACx_IRQn, 2);    /* ��2��������ȼ� */
}

/**
 * @brief       ʹ��һ��ADC DMA���� 
 * @param       cndtr: DMA����Ĵ���
 * @retval      ��
 */
void adc_dma_enable(uint16_t cndtr)
{
    ADC_ADCX->CR2 &= ~(1 << 0);         /* �ȹر�ADC */
    
    dma_enable(ADC_ADCX_DMACx, cndtr);  /* ����ʹ��DMA���� */
    
    ADC_ADCX->CR2 |= 1 << 0;            /* ��������ADC */
    ADC_ADCX->CR2 |= 1 << 22;           /* ��������ת��ͨ�� */
}

/**
 * @brief       ADC Nͨ��(6ͨ��) DMA��ȡ ��ʼ������
 *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
 *              ����,���ڱ������õ���6��ͨ��, �궨���Ƚ϶�����, ���,�������Ͳ����ú궨��ķ�ʽ���޸�ͨ����,
 *              ֱ���ڱ����������޸�, ��������Ĭ��ʹ��PA0~PA5��6��ͨ��.
 *
 *              ע��: ����������ʹ�� ADC_ADCX(Ĭ��=ADC1) �� ADC_ADCX_DMACx( DMA1_Channel1 ) ������ض���
 *              ��Ҫ���޸�adc.h�����������������, ���������ԭ��Ļ����Ͻ����޸�, ������ܵ����޷�����ʹ��.
 *
 * @param       mar         : �洢����ַ 
 * @retval      ��
 */
void adc_nch_dma_init(uint32_t mar)
{
    adc_init(); /* �ȳ�ʼ��ADC */

    /* ����ADC����ת��, DMA����ADC���� */
    ADC_ADCX->CR1 |= 1 << 8;        /* SCAN = 1, ɨ��ģʽ */
    ADC_ADCX->CR2 |= 1 << 8;        /* DMA = 1, DMA����ʹ�� */
    ADC_ADCX->CR2 |= 1 << 1;        /* CONT = 1, ����ת��ģʽ */
    
    ADC_ADCX->SQR1 &= ~(0XF << 20); /* L[3:0]���� */
    ADC_ADCX->SQR1 |= 5 << 20;      /* 6��ת���ڹ��������� Ҳ����ת����������1~6 */
    
    ADC_ADCX->SQR3  = 0 << 0;       /* SQ1[4:0] = 0, ��������1 = ͨ��0(PA0) */
    ADC_ADCX->SQR3 |= 1 << 5;       /* SQ2[4:0] = 1, ��������2 = ͨ��1(PA1) */
    ADC_ADCX->SQR3 |= 2 << 10;      /* SQ3[4:0] = 2, ��������3 = ͨ��2(PA2) */
    ADC_ADCX->SQR3 |= 3 << 15;      /* SQ4[4:0] = 3, ��������4 = ͨ��3(PA3) */
    ADC_ADCX->SQR3 |= 4 << 20;      /* SQ5[4:0] = 4, ��������5 = ͨ��4(PA4) */
    ADC_ADCX->SQR3 |= 5 << 25;      /* SQ6[4:0] = 5, ��������5 = ͨ��5(PA5) */

    
    adc_channel_set(ADC_ADCX, ADC_ADCX_CHY, 7); /* ����ADCX��Ӧͨ������ʱ��Ϊ239.5��ʱ������ */

    /* ����ADC1ͨ��0~5��Ӧ��IO��ģ������ */
    /* PA0��Ӧ ADC1_IN0 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN0,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */
    
    /* PA1��Ӧ ADC1_IN1 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN1,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */
    
    /* PA2��Ӧ ADC1_IN2 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN2,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */
    
    /* PA3��Ӧ ADC1_IN3 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN3,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */
    
    /* PA4��Ӧ ADC1_IN4 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN4,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */
                 
    /* PA5��Ӧ ADC1_IN5*/
    sys_gpio_set(GPIOA, SYS_GPIO_PIN5,
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* AD�ɼ�����ģʽ����,ģ������ */

    /* ����ADC1ͨ��14~19�Ĳ���ʱ�� */
    adc_channel_set(ADC_ADCX, 0, 7);/* ����ADCX��ͨ��0(��ӦPA0��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */
    adc_channel_set(ADC_ADCX, 1, 7);/* ����ADCX��ͨ��1(��ӦPA1��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */
    adc_channel_set(ADC_ADCX, 2, 7);/* ����ADCX��ͨ��2(��ӦPA2��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */
    adc_channel_set(ADC_ADCX, 3, 7);/* ����ADCX��ͨ��3(��ӦPA3��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */
    adc_channel_set(ADC_ADCX, 4, 7);/* ����ADCX��ͨ��4(��ӦPA4��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */
    adc_channel_set(ADC_ADCX, 5, 7);/* ����ADCX��ͨ��5(��ӦPA5��)�Ĳ���ʱ��Ϊ239.5��ʱ������ */

    /* ʹ��DMAʱ��, ������CPAR��CMAR�Ĵ���, ���������ַΪ: &ADC1->DR */
    dma_basic_config(ADC_ADCX_DMACx, (uint32_t)&ADC1->DR, mar);

    /* ����DMA���� */
    ADC_ADCX_DMACx->CCR = 0;        /* ��λCCR�Ĵ��� */
    ADC_ADCX_DMACx->CCR |= 1 << 1;  /* TCIE =1 , ʹ�ܴ�������ж� */
    ADC_ADCX_DMACx->CCR |= 0 << 4;  /* DIR = 0 , ���赽�洢��ģʽ */
    ADC_ADCX_DMACx->CCR |= 0 << 5;  /* CIRC = 0, ��ѭ��ģʽ(��ʹ����ͨģʽ) */
    ADC_ADCX_DMACx->CCR |= 0 << 6;  /* PINC = 0, �����ַ������ģʽ */
    ADC_ADCX_DMACx->CCR |= 1 << 7;  /* MINC = 1, �洢������ģʽ */
    ADC_ADCX_DMACx->CCR |= 1 << 8;  /* PSIZE[1:0] = 1, �������ݿ��Ϊ: 16λ */
    ADC_ADCX_DMACx->CCR |= 1 << 10; /* MSIZE[1:0] = 1, �洢�����ݿ��: 16λ */
    ADC_ADCX_DMACx->CCR |= 1 << 12; /* PL[1:0] =  1, �е����ȼ� */
    ADC_ADCX_DMACx->CCR |= 0 << 14; /* MEM2MEM = 0 , �Ǵ洢�����洢��ģʽ */

    sys_nvic_init(3, 3, ADC_ADCX_DMACx_IRQn, 2);    /* ��2��������ȼ� */
}


























