/**
 ****************************************************************************************************
 * @file        beep.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-18
 * @brief       ������ ��������
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
 * V1.0 20200418
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/BEEP/beep.h"

/**
 * @brief       ��ʼ��BEEP���IO��, ��ʹ��ʱ��
 * @param       ��
 * @retval      ��
 */
void beep_init(void)
{
    BEEP_GPIO_CLK_ENABLE(); /* BEEPʱ��ʹ�� */

    sys_gpio_set(BEEP_GPIO_PORT, BEEP_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* BEEP����ģʽ���� */

    BEEP(0);    /* �رշ����� */
}






