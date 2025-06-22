/**
 ****************************************************************************************************
 * @file        spblcd.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-26
 * @brief       SPBЧ��ʵ�� ��������
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
 * V1.0 20221026
 * ��һ�η���
 * V1.1 20221217
 * �޸�SPILCD_END_ADDR��ֵ, ֮ǰ�����õ�ַ����,���ܻ����ļ�ϵͳ/�ļ�
 *
 ****************************************************************************************************
 */

#include "./BSP/SPBLCD/spblcd.h"
#include "./MALLOC/malloc.h"
#include "./BSP/DMA/dma.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/SPI/spi.h"
#include "spb.h"
#include "ucos_ii.h"


/**
 * @brief       ��lcd������spi flash
 * @param       offset  : �洢��ַƫ����
 * @param       width   : ���
 * @param       height  : �߶�
 * @retval      0,�ɹ�; ����,�������;
 */
uint8_t slcd_frame_lcd2spi(uint32_t offset, uint16_t width, uint16_t height)
{
    uint16_t *pbuf;
    uint32_t startx;        /* ��ʼ��ַ */
    uint16_t i, j, k;
    uint32_t woffset = 0;   /* д��ַƫ�� */
    
    uint8_t *p;
    uint8_t temp;

    startx = SPILCD_BASE + offset * 2;  /* ��Ϊoffset��������Ϊ��λ��,��д�������ֽ�Ϊ��λ�ģ����Ե�*2 */
    pbuf = mymalloc(SRAMIN, 4096);      /* �����ڴ� */

    if (!pbuf)return 1;

    woffset = 0;
    k = 0;

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            pbuf[k++] = lcd_read_point(i, j);    /* ��ȡ�� */

            if (k == 2048)
            {
                p = (uint8_t *)pbuf;

                for (k = 0; k < 2048; k++)      /* ��ת�ߵ��ֽ� */
                {
                    temp = p[2 * k];
                    p[2 * k] = p[2 * k + 1];
                    p[2 * k + 1] = temp;
                }
                
                norflash_write((uint8_t *)pbuf, startx + woffset, 4096); /* д��һ������ */
                woffset += 4096;
                k = 0;
            }
        }
    }

    if (k)  /* ����һ������Ҫд�� */
    {
        p = (uint8_t *)pbuf;

        for (i = 0; i < k; i++) /* ��ת�ߵ��ֽ� */
        {
            temp = p[2 * i];
            p[2 * i] = p[2 * i + 1];
            p[2 * i + 1] = temp;
        }

        norflash_write((uint8_t *)pbuf, startx + woffset, k * 2);    /* д�����һ������ */
    }

    myfree(SRAMIN, pbuf);   /* �ͷ��ڴ� */
    return 0;
}

/**
 * @brief       SPI2ģʽ����
 * @param       mode    : 0, 8λ��ͨģʽ; 1, 16λDMAģʽ;
 * @param       ��
 * @retval      ��
 */
void slcd_spi2_mode(uint8_t mode)
{
    uint16_t tempreg = 0;

    if (mode == 0)  /* 8λ��ͨģʽ,��������SPI */
    {
        RCC->APB1RSTR |= 1 << 14;   /* ��λSPI2 */
        RCC->APB1RSTR &= ~(1 << 14);/* ֹͣ��λSPI2 */
        tempreg |= 0 << 10;         /* ȫ˫��ģʽ */
        tempreg |= 1 << 9;          /* ���nss���� */
        tempreg |= 1 << 8;
        tempreg |= 1 << 2;          /* SPI���� */
        tempreg |= 0 << 11;         /* 8λ���ݸ�ʽ */
        tempreg |= 1 << 1;          /* ����ģʽ��SCKΪ1 CPOL=1 */
        tempreg |= 1 << 0;          /* ���ݲ����ӵ�2��ʱ����ؿ�ʼ,CPHA=1 */
        tempreg |= 0 << 3;          /* 2��Ƶ,����Ϊ42MhzƵ�� */
        tempreg |= 0 << 7;          /* MSB First */
        tempreg |= 1 << 6;          /* SPI���� */
        SPI2->CR1 = tempreg;        /* ����CR1 */
        SPI2->I2SCFGR &= ~(1 << 11);/* ѡ��SPIģʽ */
    }
    else    /* 16λDMAģʽ */
    {
        SPI2->CR1 |= 1 << 10;       /* ������ģʽ */
        SPI2->CR1 |= 1 << 11;       /* 16bit���ݸ�ʽ */
        SPI2->CR2 |= 1 << 0;        /* ʹ��DMA���� */
    }
}

/**
 * @brief       QSPI --> LCD_RAM dma����
 *  @note       PAR��NDTR�ں���ʹ�ܵ�ʱ��������
 * @param       ��
 * @retval      ��
 */
void slcd_dma_init(void)
{
    RCC->AHBENR |= 1 << 0;  /* ����DMA1ʱ�� */
    delay_ms(5);            /* �ȴ�DMAʱ���ȶ� */
    DMA1_Channel4->CPAR = (uint32_t)&SPI2->DR;      /* �����ַΪ:SPI1->DR */
    DMA1_Channel4->CMAR = (uint32_t)&LCD->LCD_RAM;  /* Ŀ���ַΪLCD_RAM */
    DMA1_Channel4->CNDTR = 0;           /* DMA1,����������,��ʱ����Ϊ0 */
    DMA1_Channel4->CCR = 0X00000000;    /* ��λ */
    DMA1_Channel4->CCR |= 0 << 4;       /* ������� */
    DMA1_Channel4->CCR |= 0 << 5;       /* ��ͨģʽ */
    DMA1_Channel4->CCR |= 0 << 6;       /* �����ַ������ģʽ */
    DMA1_Channel4->CCR |= 0 << 7;       /* �洢��������ģʽ */
    DMA1_Channel4->CCR |= 1 << 8;       /* �������ݿ��Ϊ16λ */
    DMA1_Channel4->CCR |= 1 << 10;      /* �洢�����ݿ��16λ */
    DMA1_Channel4->CCR |= 1 << 12;      /* �е����ȼ� */
    DMA1_Channel4->CCR |= 0 << 14;      /* �Ǵ洢�����洢��ģʽ */
}

/**
 * @brief       ����һ��SPI��LCD��DMA�Ĵ���
 * @param       x       : ��ʼ�����ַ���
 * @retval      ��
 */
void slcd_dma_enable(uint32_t x)
{
    uint32_t lcdsize = spbdev.spbwidth * spbdev.spbheight;
    uint32_t dmatransfered = 0;
    
    uint32_t addr = SPILCD_BASE + (x * spbdev.spbheight) * 2;

    NORFLASH_CS(0);                                 /* ʹ��Ƭѡ */
    spi2_read_write_byte(FLASH_FastReadData);       /* ���Ϳ��ٶ�ȡ���� */
    spi2_read_write_byte((uint8_t)((addr) >> 16));  /* ����24bit��ַ */
    spi2_read_write_byte((uint8_t)((addr) >> 8));
    spi2_read_write_byte((uint8_t)addr);
    spi2_read_write_byte(0XFF);                     /* 8 dummy clock */
    
    slcd_spi2_mode(1);  /* ����Ϊ16λ��, ����DMA���� */
    
    while (lcdsize)
    {
        DMA1_Channel4->CCR &= ~(1 << 0);    /* �ر�DMA���� */
        DMA1->IFCR |= 1 << 13;              /* ����ϴεĴ�����ɱ�� */

        if (lcdsize > SLCD_DMA_MAX_TRANS)
        {
            lcdsize -= SLCD_DMA_MAX_TRANS;
            DMA1_Channel4->CNDTR = SLCD_DMA_MAX_TRANS;  /* ���ô��䳤�� */
        }
        else
        {
            DMA1_Channel4->CNDTR = lcdsize;     /* ���ô��䳤�� */
            lcdsize = 0;
        }

        dmatransfered += SLCD_DMA_MAX_TRANS;
        DMA1_Channel4->CCR |= 1 << 0;           /* ����DMA RX���� */

        while ((DMA1->ISR & (1 << 13)) == 0);   /* �ȴ�������� */
    }

    DMA1_Channel4->CCR &= ~(1 << 0);            /* �ر�DMA���� */
    
    slcd_spi2_mode(0);  /* �ָ�8λģʽ */ 
    NORFLASH_CS(1);     /* ��ֹƬѡ */
}

/**
 * @brief       ��ʾһ֡,������һ��spi��lcd����ʾ.
 * @param       x       : ����ƫ����
 * @retval      ��
 */
void slcd_frame_show(uint16_t x)
{
    OS_CPU_SR cpu_sr = 0;
    lcd_scan_dir(U2D_L2R);  /* ����ɨ�跽�� */

    if (lcddev.id == 0X9341 || lcddev.id == 0X7789 || lcddev.id == 0X5310 || lcddev.id == 0X7796 || lcddev.id == 0X5510 || lcddev.id == 0X9806)
    {
        lcd_set_window(spbdev.stabarheight, 0, spbdev.spbheight, spbdev.spbwidth);
        lcd_set_cursor(spbdev.stabarheight, 0); /* ���ù��λ�� */
    }
    else
    {
        lcd_set_window(0, spbdev.stabarheight, spbdev.spbwidth, spbdev.spbheight);

        if (lcddev.id != 0X1963)lcd_set_cursor(0, spbdev.stabarheight); /* ���ù��λ�� */
    }

    lcd_write_ram_prepare();    /* ��ʼд��GRAM */
    
    OS_ENTER_CRITICAL();        /* �����ٽ���(�޷����жϴ��) */
    slcd_dma_enable(x);         /* ����һ��SPI��LCD��dma���� */
    OS_EXIT_CRITICAL();         /* �˳��ٽ���(���Ա��жϴ��) */
    
    lcd_scan_dir(DFT_SCAN_DIR); /* �ָ�Ĭ�Ϸ��� */
    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* �ָ�Ĭ�ϴ��ڴ�С */
} 



