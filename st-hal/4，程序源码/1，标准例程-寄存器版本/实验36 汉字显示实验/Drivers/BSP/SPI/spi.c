/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI ��������
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

#include "./BSP/SPI/spi.h"


/**
 * @brief       SPI��ʼ������
 *   @note      ����ģʽ,8λ����,��ֹӲ��Ƭѡ
 * @param       ��
 * @retval      ��
 */
void spi2_init(void)
{
    uint32_t tempreg = 0;
    
    SPI2_SPI_CLK_ENABLE();          /* SPI2ʱ��ʹ�� */
    SPI2_SCK_GPIO_CLK_ENABLE();     /* SPI2_SCK��ʱ��ʹ�� */
    SPI2_MISO_GPIO_CLK_ENABLE();    /* SPI2_MISO��ʱ��ʹ�� */
    SPI2_MOSI_GPIO_CLK_ENABLE();    /* SPI2_MOSI��ʱ��ʹ�� */

    sys_gpio_set(SPI2_SCK_GPIO_PORT, SPI2_SCK_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SCK����ģʽ����(�������) */

    sys_gpio_set(SPI2_MISO_GPIO_PORT, SPI2_MISO_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* MISO����ģʽ����(�������) */

    sys_gpio_set(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* MOSI����ģʽ����(�������) */

    tempreg |= 0 << 11;             /* DFF = 0      , ʹ��8λ���ݸ�ʽ */
    tempreg |= 0 << 10;             /* RXONLY = 0   , ȫ˫��ģʽ */
    tempreg |= 1 << 9;              /* SSM = 1      , ���Ƭѡ(NSS)���� */
    tempreg |= 1 << 8;              /* SSI = 1      , ��ֹ������豸, �������� */
    tempreg |= 0 << 7;              /* LSBFRST = 0  , MSB�ȴ��� */
    tempreg |= 7 << 3;              /* BR[2:0] = 7  , Ĭ��ʹ��256��Ƶ, �ٶ���� */
    tempreg |= 1 << 2;              /* MSTR = 1     , ����ģʽ */
    tempreg |= 1 << 1;              /* CPOL = 0     , ����״̬��, SCK���ָߵ�ƽ */
    tempreg |= 1 << 0;              /* CPHA = 1     , ���ݲɼ��ӵڶ���ʱ�ӱ��ؿ�ʼ */

    SPI2_SPI->CR1 = tempreg;        /* ����CR1�Ĵ��� */
    SPI2_SPI->CR1 |= 1 << 6;        /* SPE = 1      , ʹ��SPI */

    spi2_read_write_byte(0xff);     /* ��������, ʵ���Ͼ��ǲ���8��ʱ������, �ﵽ���DR������, �Ǳ��� */
}

/**
 * @brief       SPI2�ٶ����ú���
 *   @note      SPI2ʱ��ѡ������APB1, ��PCLK1, Ϊ36Mhz
 *              SPI�ٶ� = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI2ʱ�ӷ�Ƶϵ��
 * @retval      ��
 */
void spi2_set_speed(uint8_t speed)
{
    speed &= 0X07;                  /* ���Ʒ�Χ */
    SPI2_SPI->CR1 &= ~(1 << 6);     /* SPE = 0          , SPI�豸ʧ�� */
    SPI2_SPI->CR1 &= ~(7 << 3);     /* BR[2:0] = 0      , ������ */
    SPI2_SPI->CR1 |= speed << 3;    /* BR[2:0] = speed  , ���÷�Ƶϵ�� */
    SPI2_SPI->CR1 |= 1 << 6;        /* SPE = 1          , SPIʹ�� */
}

/**
 * @brief       SPI2��дһ���ֽ�����
 * @param       txdata  : Ҫ���͵�����(1�ֽ�)
 * @retval      ���յ�������(1�ֽ�)
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    while ((SPI2_SPI->SR & 1 << 1) == 0);   /* �ȴ��������� */

    SPI2_SPI->DR = txdata;                  /* ����һ��byte */

    while ((SPI2_SPI->SR & 1 << 0) == 0);   /* �ȴ�������һ��byte */

    return SPI2_SPI->DR;                    /* �����յ������� */
}






