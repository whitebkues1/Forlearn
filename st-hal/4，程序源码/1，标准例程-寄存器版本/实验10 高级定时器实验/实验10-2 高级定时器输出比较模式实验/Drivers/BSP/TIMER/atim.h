/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-21
 * @brief       �߼���ʱ�� ��������
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
 * V1.0 20200421
 * ��һ�η���
 * V1.1 20200421
 * 1, ����atim_timx_comp_pwm_init����, ʵ������Ƚ�ģʽPWM�������
 *
 ****************************************************************************************************
 */

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* �߼���ʱ�� ���� */

 /* TIMX ���ָ������PWM ���� 
 * ���������PWMͨ��PC6(TIM8_CH1)���, �����öŰ�������PC6��PE5, Ȼ���ڳ������潫PE5���óɸ�������
 * �Ϳ��� ����TIM8_CH1����LED1(GREEN)������, ����һ�α�ʾһ��PWM��
 * Ĭ��ʹ�õ���TIM8_CH1.
 * ע��: ͨ���޸��⼸���궨��, ����֧��TIM1/TIM8��ʱ��, ����һ��IO�����ָ��������PWM
 */
#define ATIM_TIMX_NPWM_CHY_GPIO_PORT            GPIOC
#define ATIM_TIMX_NPWM_CHY_GPIO_PIN             SYS_GPIO_PIN6
#define ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC��ʱ��ʹ�� */

#define ATIM_TIMX_NPWM                          TIM8
#define ATIM_TIMX_NPWM_IRQn                     TIM8_UP_IRQn
#define ATIM_TIMX_NPWM_IRQHandler               TIM8_UP_IRQHandler
#define ATIM_TIMX_NPWM_CHY                      1                           /* ͨ��Y,  1<= Y <=4 */
#define ATIM_TIMX_NPWM_CHY_CCRX                 TIM8->CCR1                  /* ͨ��Y������ȽϼĴ��� */
#define ATIM_TIMX_NPWM_CHY_CLK_ENABLE()         do{ RCC->APB2ENR |= 1 << 13; }while(0)  /* TIM8 ʱ��ʹ�� */


/* TIMX ����Ƚ�ģʽ ���� 
 * ����ͨ��TIM8������Ƚ�ģʽ,����PC6,PC7,PC8,PC9���4·PWM,ռ�ձ�50%,����ÿһ·PWM֮�����λ��Ϊ25%
 * �޸�CCRx�����޸���λ.
 * Ĭ�������TIM1
 * ע��: ͨ���޸���Щ�궨��,����֧��TIM1/TIM8����һ����ʱ��,����һ��IO��ʹ������Ƚ�ģʽ,���PWM
 */
#define ATIM_TIMX_COMP_CH1_GPIO_PORT            GPIOC
#define ATIM_TIMX_COMP_CH1_GPIO_PIN             SYS_GPIO_PIN6
#define ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC��ʱ��ʹ�� */

#define ATIM_TIMX_COMP_CH2_GPIO_PORT            GPIOC
#define ATIM_TIMX_COMP_CH2_GPIO_PIN             SYS_GPIO_PIN7
#define ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC��ʱ��ʹ�� */

#define ATIM_TIMX_COMP_CH3_GPIO_PORT            GPIOC
#define ATIM_TIMX_COMP_CH3_GPIO_PIN             SYS_GPIO_PIN8
#define ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC��ʱ��ʹ�� */

#define ATIM_TIMX_COMP_CH4_GPIO_PORT            GPIOC
#define ATIM_TIMX_COMP_CH4_GPIO_PIN             SYS_GPIO_PIN9
#define ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE()    do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC��ʱ��ʹ�� */

#define ATIM_TIMX_COMP                          TIM8            
#define ATIM_TIMX_COMP_CH1_CCRX                 ATIM_TIMX_COMP->CCR1        /* ͨ��1������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH2_CCRX                 ATIM_TIMX_COMP->CCR2        /* ͨ��2������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH3_CCRX                 ATIM_TIMX_COMP->CCR3        /* ͨ��3������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH4_CCRX                 ATIM_TIMX_COMP->CCR4        /* ͨ��4������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CLK_ENABLE()             do{ RCC->APB2ENR |= 1 << 13; }while(0)  /* TIM8 ʱ��ʹ�� */
 
/******************************************************************************************/

void atim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* �߼���ʱ�� PWM��ʼ������ */
void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc);   /* �߼���ʱ�� ���ָ������PWM��ʼ������ */
void atim_timx_npwm_chy_set(uint32_t npwm);                 /* �߼���ʱ�� �������PWM�ĸ��� */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc);   /* �߼���ʱ�� ����Ƚ�ģʽ���PWM ��ʼ������ */

#endif

















