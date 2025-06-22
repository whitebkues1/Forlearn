/**
 ****************************************************************************************************
 * @file        sram.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-27
 * @brief       �ⲿSRAM ��������
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
 * V1.0 20200427
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/SRAM/sram.h"
#include "./SYSTEM/usart/usart.h"


/**
 * @brief       ��ʼ�� �ⲿSRAM
 * @param       ��
 * @retval      ��
 */
void sram_init(void)
{
    SRAM_CS_GPIO_CLK_ENABLE();  /* SRAM_CS��ʱ��ʹ�� */
    SRAM_WR_GPIO_CLK_ENABLE();  /* SRAM_WR��ʱ��ʹ�� */
    SRAM_RD_GPIO_CLK_ENABLE();  /* SRAM_RD��ʱ��ʹ�� */

    RCC->APB2ENR |= 0XF << 5;   /* ʹ��PD,PE,PF,PGʱ�� */
    RCC->AHBENR |= 1 << 8;      /* ʹ��FSMCʱ�� */

    sys_gpio_set(SRAM_CS_GPIO_PORT, SRAM_CS_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SRAM_CS����ģʽ���� */

    sys_gpio_set(SRAM_WR_GPIO_PORT, SRAM_WR_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SRAM_WR����ģʽ���� */

    sys_gpio_set(SRAM_RD_GPIO_PORT, SRAM_RD_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SRAM_RD����ģʽ���� */

    /* SRAM_D0 ~ SRAM_D15 IO�ڳ�ʼ�� */
    sys_gpio_set(GPIOD, (3 << 0) | (7 << 8) | (3 << 14), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* PD0,1,8,9,10,14,15   AF OUT */
    sys_gpio_set(GPIOE, (0X1FF << 7), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);                      /* PE7~15  AF OUT */

    /* SRAM_A0 ~ SRAM_A18 IO�ڳ�ʼ��
     * ����ʹ�õ�SRAMоƬΪ: XM8A51216 / IS62WV51216, ����Ϊ 512K * 16bit, ��1M�ֽ�
     * �ܹ���Ҫ19�������� ������: 2^19 * 16bit
     */
    sys_gpio_set(GPIOF, (0X3F << 0) | (0XF << 12) , SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);        /* PF0~5, PF12~15  AF OUT */
    sys_gpio_set(GPIOG, (0X3F << 0), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);                       /* PG0~5  AF OUT */
    sys_gpio_set(GPIOD, (0X07 << 11), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);                      /* PD11, 12, 13  AF OUT */

    /* NBL0, NBL1, �ߵ��ֽ�ѡ�� IO��ʼ�� */
    sys_gpio_set(GPIOE, (0X03 << 0), SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);                       /* PE0, 1  AF OUT */

    /* FSMCʱ������HCLK, Ƶ��Ϊ72Mhz
     * �Ĵ�������
     * bank1��NE1~4,ÿһ����һ��BCR+TCR�������ܹ��˸��Ĵ�����
     * ��������ʹ��NE3 ��Ҳ�Ͷ�ӦBTCR[4], [5]
     */
    SRAM_FSMC_BCRX = 0X00000000;    /* BCR�Ĵ������� */
    SRAM_FSMC_BTRX = 0X00000000;    /* BTR�Ĵ������� */
    SRAM_FSMC_BWTRX = 0X00000000;   /* BWTR�Ĵ������� */

    /* ����BCR�Ĵ��� ʹ���첽ģʽ */
    SRAM_FSMC_BCRX |= 1 << 12;      /* �洢��дʹ�� */
    SRAM_FSMC_BCRX |= 0 << 14;      /* ��дʹ����ͬ��ʱ�� */
    SRAM_FSMC_BCRX |= 1 << 4;       /* �洢�����ݿ��Ϊ16bit */

    /* ����BTR�Ĵ���, ��ʱ����ƼĴ��� */
    SRAM_FSMC_BTRX |= 0 << 28;      /* ģʽA */
    SRAM_FSMC_BTRX |= 2 << 8;       /* ���ݱ���ʱ��(DATAST)Ϊ3��HCLK 1/72M * 3 = 41.7ns (��IS62WV51216, ����) */
    SRAM_FSMC_BTRX |= 0 << 0;       /* ��ַ����ʱ��(ADDSET)Ϊ1��HCLK 1/72M = 13.9ns (ʵ�� > 200ns) */

    /* ʹ��BANK1,����1 */
    SRAM_FSMC_BCRX |= 1 << 0;       /* ʹ��BANK1������1 */
    SRAM_FSMC_BCRX |= (uint32_t)1 << 31;    /* ʹ��FSMC */
}

/**
 * @brief       ��SRAMָ����ַд��ָ����������
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       datalen : Ҫд����ֽ���(���32bit)
 * @retval      ��
 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *(volatile uint8_t *)(SRAM_BASE_ADDR + addr) = *pbuf;
        addr++;
        pbuf++;
    }
}

/**
 * @brief       ��SRAMָ����ַ��ȡָ����������
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param       datalen : Ҫ��ȡ���ֽ���(���32bit)
 * @retval      ��
 */
void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen)
{
    for (; datalen != 0; datalen--)
    {
        *pbuf++ = *(volatile uint8_t *)(SRAM_BASE_ADDR + addr);
        addr++;
    }
}

/**
 * @brief       ���Ժ��� ��SRAMָ����ַд��1���ֽ�
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       data    : Ҫд����ֽ�
 * @retval      ��
 */
void sram_test_write(uint32_t addr, uint8_t data)
{
    sram_write(&data, addr, 1);     /* д��1���ֽ� */
}

/**
 * @brief       ���Ժ��� ��SRAMָ����ַ��ȡ1���ֽ�
 * @param       addr    : ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @retval      ��ȡ��������(1���ֽ�)
 */
uint8_t sram_test_read(uint32_t addr)
{
    uint8_t data;
    sram_read(&data, addr, 1);      /* ��ȡ1���ֽ� */
    return data;
}












