/**
 ****************************************************************************************************
 * @file        dma.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       DMA ��������
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
 *
 ****************************************************************************************************
 */

#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ����1 TX DMA��ʼ������
 * @param       mar         : �洢����ַ
 * @retval      ��
 */
void dma_usart1_tx_config(uint32_t mar)
{
    DMA_Channel_TypeDef *dmax_chy;
    
    dmax_chy = DMA1_Channel4;   /* USART1_TXʹ�õ�DMAͨ��Ϊ: DMA1_Channel4 */

    /* ʹ��DMAʱ��, ������CPAR��CMAR�Ĵ���, ���������ַΪ: &USART1->DR */
    dma_basic_config(dmax_chy, (uint32_t)&USART1->DR, mar);

    dmax_chy->CCR = 0X00000000; /* ��λCCR�Ĵ��� */
    dmax_chy->CCR |= 1 << 4;    /* DIR = 1 , �洢��������ģʽ */
    dmax_chy->CCR |= 0 << 5;    /* CIRC = 0, ��ѭ��ģʽ(��ʹ����ͨģʽ) */
    dmax_chy->CCR |= 0 << 6;    /* PINC = 0, �����ַ������ģʽ */
    dmax_chy->CCR |= 1 << 7;    /* MINC = 1, �洢������ģʽ */
    dmax_chy->CCR |= 0 << 8;    /* PSIZE[1:0] = 0, �������ݿ��Ϊ: 8λ */
    dmax_chy->CCR |= 0 << 10;   /* MSIZE[1:0] = 0, �洢�����ݿ��: 8λ */
    dmax_chy->CCR |= 1 << 12;   /* PL[1:0] =  1, �е����ȼ� */
    dmax_chy->CCR |= 0 << 14;   /* MEM2MEM = 0 , �Ǵ洢�����洢��ģʽ */
}

/**
 * @brief       DMA��������
 *   @note      �������DMA���һЩ�����Ե�����, ����: DMAʱ��ʹ�� / ���������ַ �� �洢����ַ
 *              �������ò���(CCR�Ĵ���), ���û��Լ�����ʵ��
 *
 * @param       dmax_chy    : DMA��ͨ��, DMA1_Channel1 ~ DMA1_Channel7, DMA2_Channel1 ~ DMA2_Channel5
 *                            ĳ�������Ӧ�ĸ�DMA, �ĸ�ͨ��, ��ο�<<STM32���Ĳο��ֲ� V10>> 10.3.7��
 *                            ����������ȷ��DMA��ͨ��, ��������ʹ��! 
 * @param       par         : �����ַ
 * @param       mar         : �洢����ַ
 * @retval      ��
 */
void dma_basic_config(DMA_Channel_TypeDef *dmax_chy,  uint32_t par, uint32_t mar)
{
    if (dmax_chy > DMA1_Channel7)   /* ����DMA1_Channel7, ��ΪDMA2��ͨ���� */
    {
        RCC->AHBENR |= 1 << 1;  /* ����DMA2ʱ�� */
    }
    else
    {
        RCC->AHBENR |= 1 << 0;  /* ����DMA1ʱ�� */
    }

    delay_ms(5);                /* �ȴ�DMAʱ���ȶ� */

    dmax_chy->CPAR = par;       /* DMA �����ַ */
    dmax_chy->CMAR = mar;       /* DMA �洢����ַ */
    dmax_chy->CNDTR = 0;        /* DMA ���䳤������, ������dma_enable�������� */
}


/**
 * @brief       ����һ��DMA����
 * @param       dmax_chy    : DMA��ͨ��, DMA1_Channel1 ~ DMA1_Channel7, DMA2_Channel1 ~ DMA2_Channel5
 *                            ĳ�������Ӧ�ĸ�DMA, �ĸ�ͨ��, ��ο�<<STM32���Ĳο��ֲ� V10>> 10.3.7��
 *                            ����������ȷ��DMA��ͨ��, ��������ʹ��! 
 * @param       cndtr       : ���ݴ�����
 * @retval      ��
 */
void dma_enable(DMA_Channel_TypeDef *dmax_chy, uint16_t cndtr)
{
    dmax_chy->CCR &= ~(1 << 0); /* �ر�DMA���� */

    while (dmax_chy->CCR & (1 << 0));   /* ȷ��DMA���Ա����� */

    dmax_chy->CNDTR = cndtr;    /* DMA���������� */
    dmax_chy->CCR |= 1 << 0;    /* ����DMA���� */
}



























