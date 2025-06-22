/**
 ****************************************************************************************************
 * @file        timer.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-28
 * @brief       ͨ�ö�ʱ��(�����ۺϲ���ʵ��) ��������
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
 * V1.0 20221028
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/timer.h"
#include "./BSP/USART3/usart3.h"
#include "./BSP/LED/led.h"
#include "ucos_ii.h"

volatile uint8_t framecnt;   /* ֡������ */

/**
 * @brief       ��ʱ��3�жϷ������
 * @param       ��
 * @retval      ��
 */
void TIM3_IRQHandler(void)
{
    OSIntEnter();

    if (TIM3->SR & 0X01) /* �Ǹ����ж� */
    {
        if (framecnt)printf("frame:%d\r\n", framecnt); /* ��ӡ֡�� */

        framecnt = 0;
    }

    TIM3->SR &= ~(1 << 0);      /* ����жϱ�־λ */
    OSIntExit();
}

/**
 * @brief       ������ʱ��3�жϳ�ʼ��
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void tim3_int_init(uint16_t arr, uint16_t psc)
{
    RCC->APB1ENR |= 1 << 1;     /* TIM3ʱ��ʹ�� */
    TIM3->ARR = arr;            /* �趨�������Զ���װֵ */
    TIM3->PSC = psc;            /* Ԥ��Ƶ�� */
    TIM3->DIER |= 1 << 0;       /* ��������ж� */
    TIM3->CR1 |= 0x01;          /* ʹ�ܶ�ʱ�� */
    
    sys_nvic_init(1, 3, TIM3_IRQn, 2);  /* ��ռ1�������ȼ�3����2 */
}

//extern void nes_vs10xx_feeddata(void);

/**
 * @brief       ��ʱ��6�жϷ������
 * @param       ��
 * @retval      ��
 */
void TIM6_IRQHandler(void)
{
    OSIntEnter();

    if (TIM6->SR & 0X01) /* �Ǹ����ж� */
    {
//        nes_vs10xx_feeddata();  /* ������� */
    }

    TIM6->SR &= ~(1 << 0);      /* ����жϱ�־λ */
    OSIntExit();
}

/**
 * @brief       ������ʱ��6�жϳ�ʼ��
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void tim6_int_init(uint16_t arr, uint16_t psc)
{
    RCC->APB1ENR |= 1 << 4;     /* TIM6ʱ��ʹ�� */
    TIM6->ARR = arr;            /* �趨�������Զ���װֵ */
    TIM6->PSC = psc;            /* Ԥ��Ƶ�� */
    TIM6->DIER |= 1 << 0;       /* ��������ж� */
    TIM6->CR1 |= 0x01;          /* ʹ�ܶ�ʱ�� */
    
    sys_nvic_init(1, 3, TIM6_IRQn, 2);  /* ��ռ1�������ȼ�3����2 */
}

/**
 * @brief       ��ʱ��7�жϷ������
 * @param       ��
 * @retval      ��
 */
void TIM7_IRQHandler(void)
{
    OSIntEnter();

    if (TIM7->SR & 0X01) /* �Ǹ����ж� */
    {
        g_usart3_rx_sta |= 1 << 15; /* ��ǽ������ */
        TIM7->SR &= ~(1 << 0);      /* ����жϱ�־λ */
        TIM7->CR1 &= ~(1 << 0);     /* �رն�ʱ��7 */
    }

    OSIntExit();
}

/**
 * @brief       ������ʱ��7�жϳ�ʼ��
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void tim7_int_init(uint16_t arr, uint16_t psc)
{
    RCC->APB1ENR |= 1 << 5;     /* TIM7ʱ��ʹ�� */
    TIM7->ARR = arr;            /* �趨�������Զ���װֵ */
    TIM7->PSC = psc;            /* Ԥ��Ƶ�� */
    TIM7->DIER |= 1 << 0;       /* ��������ж� */
    TIM7->CR1 |= 0x01;          /* ʹ�ܶ�ʱ�� */
    
    sys_nvic_init(0, 1, TIM7_IRQn, 2);  /* ��ռ0�������ȼ�1����2 */
}


/**
 * @brief       ��ʱ��8 CH2 PWM��� ��ʼ��������ʹ��PWMģʽ1��
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void tim8_ch2n_pwm_init(uint16_t arr, uint16_t psc)
{
    RCC->APB2ENR |= 1 << 13;    /* TIM8ʱ��ʹ�� */
    RCC->APB2ENR |= 1 << 3;     /* ʹ��PORTBʱ�� */

    sys_gpio_set(GPIOB, SYS_GPIO_PIN0,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* PB0 ����ģʽ���� */

    TIM8->ARR = arr;        /* �趨�������Զ���װֵ */
    TIM8->PSC = psc;        /* ����Ԥ��Ƶ��  */
    TIM8->BDTR |= 1 << 15;  /* ʹ��MOEλ(��TIM1/8 �д˼Ĵ���,��������MOE�������PWM), ����ͨ�ö�ʱ��, ���
                                     * �Ĵ�������Ч��, ��������/�����ò���Ӱ����, Ϊ�˼�������ͳһ�ĳ�����MOEλ
                                     */
 
    TIM8->CCMR1 |= 7 << 12; /* CH2 PWMģʽ2 */
    TIM8->CCMR1 |= 1 << 11; /* CH2 Ԥװ��ʹ�� */
    
    TIM8->CCER |= 1 << 6;   /* OC2�������ʹ�� */
    TIM8->CCER |= 1 << 7;   /* OC2N�͵�ƽ��Ч */
    TIM8->CR1 |= 1 << 7;    /* ARPEʹ�� */
    TIM8->CR1 |= 1 << 0;    /* ʹ�ܶ�ʱ��TIMX */
}








