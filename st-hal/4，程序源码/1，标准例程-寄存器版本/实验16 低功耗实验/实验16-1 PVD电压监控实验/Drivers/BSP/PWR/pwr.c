/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       �͹���ģʽ ��������
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

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/**
 * @brief       ��ʼ��PVD��ѹ������
 * @param       pls: ��ѹ�ȼ�
 *   @arg       000,2.2V;   001,2.3V
 *   @arg       010,2.4V;   011,2.5V;
 *   @arg       100,2.6V;   101,2.7V;
 *   @arg       110,2.8V;   111,2.9V;
 * @retval      ��
 */
void pwr_pvd_init(uint8_t pls)
{
    RCC->APB1ENR |= 1 << 28;    /* ʹ�ܵ�Դʱ�� */
    PWR->CR &= ~(3 << 5);       /* PLS[2:0]���� */
    PWR->CR |=  pls << 5;       /* PLS[2:0] = pls,ע�ⲻҪ����Χ! */
    PWR->CR |= 1 << 4;          /* PVDE = 1,ʹ��PVD��� */

    EXTI->IMR |= 1 << 16;       /* ����line16 �ϵ��ж�(PVD & AVD) */
    EXTI->FTSR |= 1 << 16;      /* line16 ���¼��½��ش��� */
    EXTI->RTSR |= 1 << 16;      /* line16 ���¼��������ش��� */

    sys_nvic_init(3, 3, PVD_IRQn, 2);   /* ��2��������ȼ� */
}

/**
 * @brief       PVD�жϷ�����
 * @param       ��
 * @retval      ��
 */
void PVD_IRQHandler(void)
{
    if (PWR->CSR & (1 << 2))   /* ��ѹ��PLS��ѡ��ѹ���� */
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED); /* LCD��ʾ��ѹ�� */
        LED1(0);                                                        /* �����̵�, ������ѹ���� */
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);/* LCD��ʾ��ѹ���� */
        LED1(1);                                                        /* ����̵� */
    }

    EXTI->PR |= 1 << 16;   /* ���line16���жϱ�־ */
}












