/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2020-04-24
 * @brief       DAC ��������
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
 * ����dac_triangular_wave����
 * V1.2 20200424
 * 1, ֧��DMA DAC������Ⲩ��
 * 2, ����dac_dma_wave_init��dac_dma_wave_enable����
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       DAC��ʼ������
 *   @note      ������֧��DAC1_OUT1/2ͨ����ʼ��
 *              DAC������ʱ������APB1, ʱ��Ƶ��=36Mhz=27.8ns
 *              DAC�����buffer�رյ�ʱ��, �������ʱ��: tSETTLING = 3us (F103�����ֲ���д)
 *              ���DAC���������ٶ�ԼΪ:333Khz, ��10����Ϊһ������, ��������33Khz���ҵĲ���
 *
 * @param       outx: Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @retval      ��
 */
void dac_init(uint8_t outx)
{
    uint8_t offset = (outx - 1) * 16;   /* OUT1 / OUT2��DAC�Ĵ��������ƫ����(OUT1 = 0, OUT2 = 16) */

    RCC->APB2ENR |= 1 << 2;;        /* ʹ��DAC OUT1/2��IO��ʱ��(����PA��,PA4/PA5) */
    RCC->APB1ENR |= 1 << 29;        /* ʹ��DAC1��ʱ�� */

    sys_gpio_set(GPIOA, 1 << (4 + outx),/* STM32��Ƭ��, ����PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* DAC����ģʽ����,ģ������ */

    DAC1->CR = 0;                   /* DR�Ĵ������� */
    DAC1->CR |= 1 << (1 + offset);  /* BOFF1/2=1,�ر�DACͨ��1/2������� */
    DAC1->CR |= 0 << (2 + offset);  /* TEN1/2=0,��ʹ�ô������� */
    DAC1->CR |= 0 << (3 + offset);  /* TSEL1/2[3:0]=0,������� */
    DAC1->CR |= 0 << (6 + offset);  /* WAVE1/2[1:0]=0,��ʹ�ò��η��� */
    DAC1->CR |= 0 << (12 + offset); /* DMAEN1/2=0,DAC1/2 DMA��ʹ�� */
    DAC1->CR |= 0 << (14 + offset); /* CEN1/2=0,DAC1/2��������ͨģʽ */

    DAC1->CR |= 1 << (0 + offset);  /* ʹ��DAC1_OUT1/2 */
    DAC1->DHR12R1 = 0;  /* ͨ��1��� 0 */
    DAC1->DHR12R2 = 0;  /* ͨ��2��� 0 */
}

/**
 * @brief       ����ͨ��1/2�����ѹ
 * @param       outx: 1,ͨ��1; 2,ͨ��2
 * @param       vol : 0~3300,����0~3.3V
 * @retval      ��
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* ���ֵ���ڵ���4096, ��ȡ4095 */

    if (outx == 1)   /* ͨ��1 */
    {
        DAC1->DHR12R1 = temp;
    }
    else            /* ͨ��2 */
    {
        DAC1->DHR12R2 = temp;
    }
}

/**
 * @brief       ����DAC_OUT1������ǲ�
 *   @note      ���Ƶ�� �� 1000 / (dt * samples) Khz, ������dt��С��ʱ��,����С��5usʱ, ����delay_us
 *              ����Ͳ�׼��(���ú���,����ȶ���Ҫʱ��,��ʱ��С��ʱ��,��Щʱ���Ӱ�쵽��ʱ), Ƶ�ʻ�ƫС.
 * 
 * @param       maxval : ���ֵ(0 < maxval < 4096), (maxval + 1)������ڵ���samples/2
 * @param       dt     : ÿ�����������ʱʱ��(��λ: us)
 * @param       samples: ������ĸ���, samples����С�ڵ���(maxval + 1) * 2 , ��maxval���ܵ���0
 * @param       n      : ������θ���,0~65535
 *
 * @retval      ��
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval;           /* ������ */
    
    if((maxval + 1) <= samples)return ; /* ���ݲ��Ϸ� */
        
    incval = (maxval + 1) / (samples / 2);  /* ��������� */
    
    for(j = 0; j < n; j++)
    { 
        DAC1->DHR12R1 = 0;  /* �����0 */
        for(i = 0; i < (samples / 2); i++)  /* ��������� */
        { 
             DAC1->DHR12R1 +=  incval;
             delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  /* ����½��� */
        {
             DAC1->DHR12R1 -=  incval;
             delay_us(dt);
        }
    }
}

/**
 * @brief       DAC DMA������γ�ʼ������
 *   @note      ������֧��DAC1_OUT1/2ͨ����ʼ��
 *              DAC������ʱ������APB1, ʱ��Ƶ��=36Mhz=27.8ns
 *              DAC�����buffer�رյ�ʱ��, �������ʱ��: tSETTLING = 4us (F103�����ֲ���д)
 *              ���DAC���������ٶ�ԼΪ:250Khz, ��10����Ϊһ������, ��������25Khz���ҵĲ���
 *
 * @param       outx: Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @param       par         : �����ַ
 * @param       mar         : �洢����ַ
 * @retval      ��
 */
void dac_dma_wave_init(uint8_t outx, uint32_t par, uint32_t mar)
{
    DMA_Channel_TypeDef *dmax_chy;
    uint8_t offset = (outx - 1) * 16;   /* OUT1 / OUT2��DAC�Ĵ��������ƫ����(OUT1 = 0, OUT2 = 16) */

    if (outx == 1)
    {
        dmax_chy = DMA2_Channel3;       /* OUT1��ӦDMA2_Channel3 */
    }
    else
    {
        dmax_chy = DMA2_Channel4;       /* OUT2��ӦDMA2_Channel4 */

    }
    
    RCC->APB1ENR |= 1 << 5;             /* TIM7ʱ��ʹ�� */ 
    
    dac_init(outx);                     /* ��ʼ��DAC ��Ӧͨ�� */
    /* �в��첿�� �������� */
    DAC1->CR |= 1 << (12 + offset);     /* DMAEN1/2 = 1,ͨ��1/2 DMAʹ�� */
    DAC1->CR |= 2 << (3 + offset);      /* TSEL1/2[2:0] = 2, TIM7_TRGO���� */
    DAC1->CR |= 1 << (2 + offset);      /* TEN1/2 = 1,����ʹ�� */

    /* ʹ��DMAʱ��, ������CPAR��CMAR�Ĵ��� */
    dma_basic_config(dmax_chy, par, mar);

    dmax_chy->CCR = 0;          /* ��λCCR�Ĵ��� */
    dmax_chy->CCR |= 1 << 4;    /* DIR = 1 , �洢��������ģʽ */
    dmax_chy->CCR |= 1 << 5;    /* CIRC = 1, ѭ��ģʽ */
    dmax_chy->CCR |= 0 << 6;    /* PINC = 0, �����ַ������ģʽ */
    dmax_chy->CCR |= 1 << 7;    /* MINC = 1, �洢������ģʽ */
    dmax_chy->CCR |= 1 << 8;    /* PSIZE[1:0] = 1, �������ݿ��Ϊ: 16λ */
    dmax_chy->CCR |= 1 << 10;   /* MSIZE[1:0] = 1, �洢�����ݿ��: 16λ */
    dmax_chy->CCR |= 1 << 12;   /* PL[1:0] =  1, �е����ȼ� */
    dmax_chy->CCR |= 0 << 14;   /* MEM2MEM = 0 , �Ǵ洢�����洢��ģʽ */
}

/**
 * @brief       DAC DMAʹ�ܲ������
 *   @note      TIM7������ʱ��Ƶ��(f)����APB1, f = 36M * 2 = 72Mhz.
 *              DAC����Ƶ�� ftrgo = f / ((psc + 1) * (arr + 1))
 *              ����Ƶ�� = ftrgo / ndtr; 
 *
 * @param       outx        : Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @param       ndtr        : DMAͨ�����δ���������
 * @param       arr         : TIM7���Զ���װ��ֵ
 * @param       psc         : TIM7�ķ�Ƶϵ��
 * @retval      ��
 */
void dac_dma_wave_enable(uint8_t outx, uint16_t cndtr, uint16_t arr, uint16_t psc)
{
    TIM7->CR1 &= ~(1 << 0);         /* �رն�ʱ��TIMX,ֹͣ���� */
    if (outx == 1)
    {
        dma_enable(DMA2_Channel3, cndtr);   /* ��������DMA2_Channel3���� */
    }
    else
    {
        dma_enable(DMA2_Channel4, cndtr);   /* ��������DMA2_Channel4���� */
    }

    TIM7->ARR = arr;                /* �趨�������Զ���װֵ */
    TIM7->PSC = psc;                /* ����Ԥ��Ƶ��  */
    TIM7->CR2 |= 2 << 4;            /* ��ʱ��7�ĸ����¼�����DACת�� */ 
    TIM7->CR1 |= 1 << 0;            /* ʹ�ܶ�ʱ��TIMX */ 
}






















































