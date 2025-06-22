/**
 ****************************************************************************************************
 * @file        timer.h
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

#ifndef __TIMER_H
#define __TIMER_H


#include "./SYSTEM/sys/sys.h"


/* LCD PWM��������  */
#define LCD_BLPWM_VAL       TIM8->CCR2 


void tim8_ch2n_pwm_init(uint16_t arr, uint16_t psc);    /* ��ʱ��8 CH2N PWM��ʼ������ */
void tim7_int_init(uint16_t arr, uint16_t psc);         /* ��ʱ��7 ��ʱ�ж����� */
void tim3_int_init(uint16_t arr, uint16_t psc);         /* ��ʱ��3 ��ʱ�ж����� */
void tim6_int_init(uint16_t arr, uint16_t psc);         /* ��ʱ��6 ��ʱ�ж����� */

#endif

















