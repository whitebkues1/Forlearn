/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-20
 * @brief       ͨ�ö�ʱ�� ��������
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
 * V1.0 20200420
 * ��һ�η���
 * V1.1 20200420
 * ����gtim_timx_pwm_chy_init����
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/**
 * @brief       ͨ�ö�ʱ��TIMX�жϷ�����
 * @param       ��
 * @retval      ��
 */
void GTIM_TIMX_INT_IRQHandler(void)
{ 
    if (GTIM_TIMX_INT->SR & 0X0001)   /* ����ж� */
    {
        LED1_TOGGLE();
    }

    GTIM_TIMX_INT->SR &= ~(1 << 0); /* ����жϱ�־λ */
} 

/**
 * @brief       ͨ�ö�ʱ��TIMX��ʱ�жϳ�ʼ������
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();
    GTIM_TIMX_INT->ARR = arr;           /* �趨�������Զ���װֵ */
    GTIM_TIMX_INT->PSC = psc;           /* ����Ԥ��Ƶ��  */
    GTIM_TIMX_INT->DIER |= 1 << 0;      /* ��������ж� */
    GTIM_TIMX_INT->CR1 |= 1 << 0;       /* ʹ�ܶ�ʱ��TIMX */
    sys_nvic_init(1, 3, GTIM_TIMX_INT_IRQn, 2); /* ��ռ1�������ȼ�3����2 */
}

/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y PWM��� ��ʼ��������ʹ��PWMģʽ1��
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_PWM_CHY;
    GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();    /* TIMX ͨ�� IO��ʱ��ʹ�� */
    GTIM_TIMX_PWM_CHY_CLK_ENABLE();         /* TIMX ʱ��ʹ�� */

    sys_gpio_set(GTIM_TIMX_PWM_CHY_GPIO_PORT, GTIM_TIMX_PWM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX PWM CHY ����ģʽ���� */

    GTIM_TIMX_PWM_CHY_GPIO_REMAP();         /* IO��REMAP����, �Ǳ���! */

    GTIM_TIMX_PWM->ARR = arr;       /* �趨�������Զ���װֵ */
    GTIM_TIMX_PWM->PSC = psc;       /* ����Ԥ��Ƶ��  */
    GTIM_TIMX_PWM->BDTR |= 1 << 15; /* ʹ��MOEλ(��TIM1/8 �д˼Ĵ���,��������MOE�������PWM), ����ͨ�ö�ʱ��, ���
                                     * �Ĵ�������Ч��, ��������/�����ò���Ӱ����, Ϊ�˼�������ͳһ�ĳ�����MOEλ
                                     */

    if (chy <= 2)
    {
        GTIM_TIMX_PWM->CCMR1 |= 6 << (4 + 8 * (chy - 1));   /* CH1/2 PWMģʽ1 */
        GTIM_TIMX_PWM->CCMR1 |= 1 << (3 + 8 * (chy - 1));   /* CH1/2 Ԥװ��ʹ�� */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_PWM->CCMR2 |= 6 << (4 + 8 * (chy - 3));   /* CH3/4 PWMģʽ1 */
        GTIM_TIMX_PWM->CCMR2 |= 1 << (3 + 8 * (chy - 3));   /* CH3/4 Ԥװ��ʹ�� */
    }

    GTIM_TIMX_PWM->CCER |= 1 << (4 * (chy - 1));        /* OCy ���ʹ�� */
    GTIM_TIMX_PWM->CCER |= 1 << (1 + 4 * (chy - 1));    /* OCy �͵�ƽ��Ч */
    GTIM_TIMX_PWM->CR1 |= 1 << 7;   /* ARPEʹ�� */
    GTIM_TIMX_PWM->CR1 |= 1 << 0;   /* ʹ�ܶ�ʱ��TIMX */
}









