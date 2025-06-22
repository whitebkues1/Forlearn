/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       DAC ��������
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
 * V1.0 20200424
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       DAC��ʼ������
 *   @note      ������֧��DAC1_OUT1/2ͨ����ʼ��
 *              DAC������ʱ������APB1, ʱ��Ƶ��=36Mhz=27.8ns
 *              DAC�����buffer�رյ�ʱ��, �������ʱ��: tSETTLING = 3us (F103�����ֲ���д)
 *              ���DAC���������ٶ�ԼΪ:333Khz, ��10����Ϊһ������, ��������33Khz���ҵĲ���
 *
 * @param       outx: Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @retval      ��
 */
void dac_init(uint8_t outx)
{
    uint8_t offset = (outx - 1) * 16;   /* OUT1 / OUT2��DAC�Ĵ��������ƫ����(OUT1 = 0, OUT2 = 16) */

    RCC->APB2ENR |= 1 << 2;;        /* ʹ��DAC OUT1/2��IO��ʱ��(����PA��,PA4/PA5) */
    RCC->APB1ENR |= 1 << 29;        /* ʹ��DAC1��ʱ�� */

    sys_gpio_set(GPIOA, 1 << (4 + outx),/* STM32��Ƭ��, ����PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* DAC����ģʽ����,ģ������ */

    DAC1->CR = 0;                   /* DR�Ĵ������� */
    DAC1->CR |= 1 << (1 + offset);  /* BOFF1/2=1,�ر�DACͨ��1/2������� */
    DAC1->CR |= 0 << (2 + offset);  /* TEN1/2=0,��ʹ�ô������� */
    DAC1->CR |= 0 << (3 + offset);  /* TSEL1/2[3:0]=0,������� */
    DAC1->CR |= 0 << (6 + offset);  /* WAVE1/2[1:0]=0,��ʹ�ò��η��� */
    DAC1->CR |= 0 << (12 + offset); /* DMAEN1/2=0,DAC1/2 DMA��ʹ�� */
    DAC1->CR |= 0 << (14 + offset); /* CEN1/2=0,DAC1/2��������ͨģʽ */

    DAC1->CR |= 1 << (0 + offset);  /* ʹ��DAC1_OUT1/2 */
    DAC1->DHR12R1 = 0;  /* ͨ��1��� 0 */
    DAC1->DHR12R2 = 0;  /* ͨ��2��� 0 */
}

/**
 * @brief       ����ͨ��1/2�����ѹ
 * @param       outx: 1,ͨ��1; 2,ͨ��2
 * @param       vol : 0~3300,����0~3.3V
 * @retval      ��
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* ���ֵ���ڵ���4096, ��ȡ4095 */

    if (outx == 1)   /* ͨ��1 */
    {
        DAC1->DHR12R1 = temp;
    }
    else            /* ͨ��2 */
    {
        DAC1->DHR12R2 = temp;
    }
}





















































