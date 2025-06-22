/**
 ****************************************************************************************************
 * @file        wdg.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ���Ź� ��������
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
 *
 ****************************************************************************************************
 */

#include "./BSP/WDG/wdg.h"

/**
 * @brief       ��ʼ���������Ź� 
 * @param       prer: ��Ƶ��:0~7(ֻ�е�3λ��Ч!)
 *   @arg       ��Ƶ���� = 4 * 2^prer. �����ֵֻ����256!
 * @param       rlr: �Զ���װ��ֵ,0~0XFFF. 
 * @note        ʱ�����(���):Tout=((4 * 2^prer) * rlr) / 40 (ms). 
 * @retval      ��
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    IWDG->KR = 0X5555;  /* ʹ�ܶ�IWDG->PR��IWDG->RLR��д */
    IWDG->PR = prer;    /* ���÷�Ƶϵ�� */
    IWDG->RLR = rlr;    /* �Ӽ��ؼĴ��� IWDG->RLR */
    IWDG->KR = 0XAAAA;  /* reload */
    IWDG->KR = 0XCCCC;  /* ʹ�ܿ��Ź� */
}

/**
 * @brief       ι�������Ź�
 * @param       ��
 * @retval      ��
 */
void iwdg_feed(void)
{
    IWDG->KR = 0XAAAA;  /* reload */
}








