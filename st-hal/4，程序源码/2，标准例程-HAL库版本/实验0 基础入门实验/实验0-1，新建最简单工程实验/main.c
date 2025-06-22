/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-12-29
 * @brief       ���STM32����, ���������ļ�(.s�ļ�), δʹ���κο��ļ�
 *              �ô���ʵ�ֹ��ܣ�ͨ��PB5����DS0��˸
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
 * V1.0 20201229
 * ��һ�η���
 *
 ****************************************************************************************************
 */


/* ���߻���ַ���� */
#define PERIPH_BASE         0x40000000                          /* �������ַ */

#define APB1PERIPH_BASE     PERIPH_BASE                         /* APB1���߻���ַ */
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000)          /* APB2���߻���ַ */
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x00020000)          /* AHB���߻���ַ */

/* �������ַ���� */
#define RCC_BASE            (AHBPERIPH_BASE + 0x00001000)       /* RCC����ַ */
#define GPIOB_BASE          (APB2PERIPH_BASE + 0x00000C00)      /* GPIOB����ַ */


/* ������ؼĴ���ӳ��(����) */
#define RCC_APB2ENR         *(volatile unsigned int *)(RCC_BASE + 0x18)     /* RCC_APB2ENR�Ĵ���ӳ�� */
    
#define GPIOB_CRL           *(volatile unsigned int *)(GPIOB_BASE + 0x00)   /* GPIOB_CRL�Ĵ���ӳ�� */
#define GPIOB_CRH           *(volatile unsigned int *)(GPIOB_BASE + 0x04)   /* GPIOB_CRH�Ĵ���ӳ�� */
#define GPIOB_IDR           *(volatile unsigned int *)(GPIOB_BASE + 0x08)   /* GPIOB_IDR�Ĵ���ӳ�� */
#define GPIOB_ODR           *(volatile unsigned int *)(GPIOB_BASE + 0x0C)   /* GPIOB_ODR�Ĵ���ӳ�� */


/* ��ʱ���� */
static void delay_x(volatile unsigned int t)
{
    while(t--);
}

/* main���� */
int main(void)
{
    /* δִ���κ�PLLʱ������, Ĭ��ʹ��HSI(8M)����, �൱�ڹ�������Ƶ8MhzƵ���� */

    RCC_APB2ENR |= 1 << 3;          /* GPIOB ʱ��ʹ�� */

    GPIOB_CRL &= ~(0X0FUL << 20);   /* MODE5[1:0], CNF5[1:0], ���� */
    GPIOB_CRL |= 0x03 << 20;        /* MODE5[1:0]=3, PB5���ģʽ, �ٶ�50Mhz */
    GPIOB_CRL |= 0x00 << 22;        /* CNF5[1:0] =0, PB5�������ģʽ */

    while(1)
    {
        GPIOB_ODR |= 1 << 5;        /* PB5 = 1, LED0�� */
        delay_x(500000);            /* ��ʱһ��ʱ�� */
        GPIOB_ODR &= ~(1UL << 5);   /* PB5 = 0, LED0�� */
        delay_x(500000);            /* ��ʱһ��ʱ�� */
    }
}
























