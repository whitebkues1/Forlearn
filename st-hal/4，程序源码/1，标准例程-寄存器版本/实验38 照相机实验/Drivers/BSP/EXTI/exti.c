/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-26
 * @brief       �ⲿ�ж� ��������
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
 * V1.0 20200419
 * ��һ�η���
 * V1.1 20200426
 * 1, ����exti8_init��������Ӧ�жϷ�����
 * 2, ֧�ֶ�OV7725 VSYNC�жϵĴ���
 * 3, ���� g_ov7725_vsta ȫ�ֱ���
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/OV7725/ov7725.h"

/**
 * @brief       KEY0 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY0_INT_IRQHandler(void)
{
    delay_ms(20);  /* ���� */
    EXTI->PR = KEY0_INT_GPIO_PIN;   /* ���KEY0�����ж��� ���жϱ�־λ */

    if (KEY0 == 0)
    {
        LED0_TOGGLE();  /* LED0 ״̬ȡ�� */
        LED1_TOGGLE();  /* LED1 ״̬ȡ�� */
    }
}

/**
 * @brief       KEY1 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY1_INT_IRQHandler(void)
{
    delay_ms(20);  /* ���� */
    EXTI->PR = KEY1_INT_GPIO_PIN;   /* ���KEY1�����ж��� ���жϱ�־λ */

    if (KEY1 == 0)
    {
        LED0_TOGGLE();  /* LED0 ״̬ȡ�� */
    }
}


/**
 * @brief       WK_UP �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void WKUP_INT_IRQHandler(void)
{
    delay_ms(20);  /* ���� */
    EXTI->PR = WKUP_INT_GPIO_PIN;   /* ���WKUP�����ж��� ���жϱ�־λ */

    if (WK_UP == 1)
    {
        BEEP_TOGGLE();  /* LED2 ״̬ȡ�� */
    }
}

/**
 * @brief       �ⲿ�жϳ�ʼ������
 * @param       ��
 * @retval      ��
 */
void extix_init(void)
{
    key_init();
    sys_nvic_ex_config(KEY0_INT_GPIO_PORT, KEY0_INT_GPIO_PIN, SYS_GPIO_FTIR);   /* KEY0����Ϊ�½��ش����ж� */
    sys_nvic_ex_config(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN, SYS_GPIO_FTIR);   /* KEY1����Ϊ�½��ش����ж� */
    sys_nvic_ex_config(WKUP_INT_GPIO_PORT, WKUP_INT_GPIO_PIN, SYS_GPIO_RTIR);   /* WKUP����Ϊ�����ش����ж� */

    sys_nvic_init( 0, 2, KEY0_INT_IRQn, 2); /* ��ռ0�������ȼ�2����2 */
    sys_nvic_init( 1, 2, KEY1_INT_IRQn, 2); /* ��ռ1�������ȼ�2����2 */
    sys_nvic_init( 2, 2, WKUP_INT_IRQn, 2); /* ��ռ2�������ȼ�2����2 */
}

/* OV7725 ֡�жϱ�־
 * 0, ��ʾ��һ֡�����Ѿ���������, �µ�֡�жϵ���, ������FIFOд����
 * 1, ��ʾ��һ֡���ݻ�û�д�����, �µ�֡�жϵ���, ������FIFOд����
 */
uint8_t g_ov7725_vsta = 0;

/**
 * @brief       OV7725 VSYNC �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void OV7725_VSYNC_INT_IRQHandler(void)
{
    if (EXTI->PR & OV7725_VSYNC_GPIO_PIN)   /* ��OV7725_VSYNC_GPIO_PIN�ߵ��ж� */
    {
        if (g_ov7725_vsta == 0) /* ��һ֡�����Ѿ�������? */
        {
            OV7725_WRST(0);     /* ��λдָ�� */
            OV7725_WRST(1);     /* ������λ */
            OV7725_WEN(1);      /* ����д��FIFO */
            g_ov7725_vsta = 1;  /* ���֡�ж� */
        }
        else
        {
            OV7725_WEN(0);      /* ��ֹд��FIFO */
        }
    }

    EXTI->PR = OV7725_VSYNC_GPIO_PIN;   /* ���OV7725_VSYNC_GPIO_PIN�ϵ��жϱ�־λ */
}

/**
 * @brief       OV7725 VSYNC�ⲿ�жϳ�ʼ������
 * @param       ��
 * @retval      ��
 */
void exti_ov7725_vsync_init(void)
{
    sys_nvic_ex_config(OV7725_VSYNC_GPIO_PORT, OV7725_VSYNC_GPIO_PIN, SYS_GPIO_RTIR);   /* OV7725 VSYNC�� �����ش����ж� */
    sys_nvic_init( 0, 0, OV7725_VSYNC_INT_IRQn, 2);                                     /* ��ռ0�������ȼ�0����2 */
}










