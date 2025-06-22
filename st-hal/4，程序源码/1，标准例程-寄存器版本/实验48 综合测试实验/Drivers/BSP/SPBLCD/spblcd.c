/**
 ****************************************************************************************************
 * @file        spblcd.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-26
 * @brief       SPB效果实现 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20221026
 * 第一次发布
 * V1.1 20221217
 * 修改SPILCD_END_ADDR的值, 之前的设置地址不对,可能会损坏文件系统/文件
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
 * @brief       从lcd拷贝到spi flash
 * @param       offset  : 存储地址偏移量
 * @param       width   : 宽度
 * @param       height  : 高度
 * @retval      0,成功; 其他,错误代码;
 */
uint8_t slcd_frame_lcd2spi(uint32_t offset, uint16_t width, uint16_t height)
{
    uint16_t *pbuf;
    uint32_t startx;        /* 起始地址 */
    uint16_t i, j, k;
    uint32_t woffset = 0;   /* 写地址偏移 */
    
    uint8_t *p;
    uint8_t temp;

    startx = SPILCD_BASE + offset * 2;  /* 因为offset是以像素为单位的,而写入是以字节为单位的，所以得*2 */
    pbuf = mymalloc(SRAMIN, 4096);      /* 申请内存 */

    if (!pbuf)return 1;

    woffset = 0;
    k = 0;

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            pbuf[k++] = lcd_read_point(i, j);    /* 读取点 */

            if (k == 2048)
            {
                p = (uint8_t *)pbuf;

                for (k = 0; k < 2048; k++)      /* 调转高低字节 */
                {
                    temp = p[2 * k];
                    p[2 * k] = p[2 * k + 1];
                    p[2 * k + 1] = temp;
                }
                
                norflash_write((uint8_t *)pbuf, startx + woffset, 4096); /* 写入一个扇区 */
                woffset += 4096;
                k = 0;
            }
        }
    }

    if (k)  /* 还有一点数据要写入 */
    {
        p = (uint8_t *)pbuf;

        for (i = 0; i < k; i++) /* 调转高低字节 */
        {
            temp = p[2 * i];
            p[2 * i] = p[2 * i + 1];
            p[2 * i + 1] = temp;
        }

        norflash_write((uint8_t *)pbuf, startx + woffset, k * 2);    /* 写入最后一次数据 */
    }

    myfree(SRAMIN, pbuf);   /* 释放内存 */
    return 0;
}

/**
 * @brief       SPI2模式设置
 * @param       mode    : 0, 8位普通模式; 1, 16位DMA模式;
 * @param       无
 * @retval      无
 */
void slcd_spi2_mode(uint8_t mode)
{
    uint16_t tempreg = 0;

    if (mode == 0)  /* 8位普通模式,重新配置SPI */
    {
        RCC->APB1RSTR |= 1 << 14;   /* 复位SPI2 */
        RCC->APB1RSTR &= ~(1 << 14);/* 停止复位SPI2 */
        tempreg |= 0 << 10;         /* 全双工模式 */
        tempreg |= 1 << 9;          /* 软件nss管理 */
        tempreg |= 1 << 8;
        tempreg |= 1 << 2;          /* SPI主机 */
        tempreg |= 0 << 11;         /* 8位数据格式 */
        tempreg |= 1 << 1;          /* 空闲模式下SCK为1 CPOL=1 */
        tempreg |= 1 << 0;          /* 数据采样从第2个时间边沿开始,CPHA=1 */
        tempreg |= 0 << 3;          /* 2分频,设置为42Mhz频率 */
        tempreg |= 0 << 7;          /* MSB First */
        tempreg |= 1 << 6;          /* SPI启动 */
        SPI2->CR1 = tempreg;        /* 设置CR1 */
        SPI2->I2SCFGR &= ~(1 << 11);/* 选择SPI模式 */
    }
    else    /* 16位DMA模式 */
    {
        SPI2->CR1 |= 1 << 10;       /* 仅接收模式 */
        SPI2->CR1 |= 1 << 11;       /* 16bit数据格式 */
        SPI2->CR2 |= 1 << 0;        /* 使用DMA接收 */
    }
}

/**
 * @brief       QSPI --> LCD_RAM dma配置
 *  @note       PAR和NDTR在后面使能的时候再设置
 * @param       无
 * @retval      无
 */
void slcd_dma_init(void)
{
    RCC->AHBENR |= 1 << 0;  /* 开启DMA1时钟 */
    delay_ms(5);            /* 等待DMA时钟稳定 */
    DMA1_Channel4->CPAR = (uint32_t)&SPI2->DR;      /* 外设地址为:SPI1->DR */
    DMA1_Channel4->CMAR = (uint32_t)&LCD->LCD_RAM;  /* 目标地址为LCD_RAM */
    DMA1_Channel4->CNDTR = 0;           /* DMA1,传输数据量,暂时设置为0 */
    DMA1_Channel4->CCR = 0X00000000;    /* 复位 */
    DMA1_Channel4->CCR |= 0 << 4;       /* 从外设读 */
    DMA1_Channel4->CCR |= 0 << 5;       /* 普通模式 */
    DMA1_Channel4->CCR |= 0 << 6;       /* 外设地址非增量模式 */
    DMA1_Channel4->CCR |= 0 << 7;       /* 存储器非增量模式 */
    DMA1_Channel4->CCR |= 1 << 8;       /* 外设数据宽度为16位 */
    DMA1_Channel4->CCR |= 1 << 10;      /* 存储器数据宽度16位 */
    DMA1_Channel4->CCR |= 1 << 12;      /* 中等优先级 */
    DMA1_Channel4->CCR |= 0 << 14;      /* 非存储器到存储器模式 */
}

/**
 * @brief       开启一次SPI到LCD的DMA的传输
 * @param       x       : 起始传输地址编号
 * @retval      无
 */
void slcd_dma_enable(uint32_t x)
{
    uint32_t lcdsize = spbdev.spbwidth * spbdev.spbheight;
    uint32_t dmatransfered = 0;
    
    uint32_t addr = SPILCD_BASE + (x * spbdev.spbheight) * 2;

    NORFLASH_CS(0);                                 /* 使能片选 */
    spi2_read_write_byte(FLASH_FastReadData);       /* 发送快速读取命令 */
    spi2_read_write_byte((uint8_t)((addr) >> 16));  /* 发送24bit地址 */
    spi2_read_write_byte((uint8_t)((addr) >> 8));
    spi2_read_write_byte((uint8_t)addr);
    spi2_read_write_byte(0XFF);                     /* 8 dummy clock */
    
    slcd_spi2_mode(1);  /* 设置为16位宽, 方便DMA传输 */
    
    while (lcdsize)
    {
        DMA1_Channel4->CCR &= ~(1 << 0);    /* 关闭DMA传输 */
        DMA1->IFCR |= 1 << 13;              /* 清除上次的传输完成标记 */

        if (lcdsize > SLCD_DMA_MAX_TRANS)
        {
            lcdsize -= SLCD_DMA_MAX_TRANS;
            DMA1_Channel4->CNDTR = SLCD_DMA_MAX_TRANS;  /* 设置传输长度 */
        }
        else
        {
            DMA1_Channel4->CNDTR = lcdsize;     /* 设置传输长度 */
            lcdsize = 0;
        }

        dmatransfered += SLCD_DMA_MAX_TRANS;
        DMA1_Channel4->CCR |= 1 << 0;           /* 开启DMA RX传输 */

        while ((DMA1->ISR & (1 << 13)) == 0);   /* 等待传输完成 */
    }

    DMA1_Channel4->CCR &= ~(1 << 0);            /* 关闭DMA传输 */
    
    slcd_spi2_mode(0);  /* 恢复8位模式 */ 
    NORFLASH_CS(1);     /* 禁止片选 */
}

/**
 * @brief       显示一帧,即启动一次spi到lcd的显示.
 * @param       x       : 坐标偏移量
 * @retval      无
 */
void slcd_frame_show(uint16_t x)
{
    OS_CPU_SR cpu_sr = 0;
    lcd_scan_dir(U2D_L2R);  /* 设置扫描方向 */

    if (lcddev.id == 0X9341 || lcddev.id == 0X7789 || lcddev.id == 0X5310 || lcddev.id == 0X7796 || lcddev.id == 0X5510 || lcddev.id == 0X9806)
    {
        lcd_set_window(spbdev.stabarheight, 0, spbdev.spbheight, spbdev.spbwidth);
        lcd_set_cursor(spbdev.stabarheight, 0); /* 设置光标位置 */
    }
    else
    {
        lcd_set_window(0, spbdev.stabarheight, spbdev.spbwidth, spbdev.spbheight);

        if (lcddev.id != 0X1963)lcd_set_cursor(0, spbdev.stabarheight); /* 设置光标位置 */
    }

    lcd_write_ram_prepare();    /* 开始写入GRAM */
    
    OS_ENTER_CRITICAL();        /* 进入临界区(无法被中断打断) */
    slcd_dma_enable(x);         /* 启动一次SPI到LCD的dma传输 */
    OS_EXIT_CRITICAL();         /* 退出临界区(可以被中断打断) */
    
    lcd_scan_dir(DFT_SCAN_DIR); /* 恢复默认方向 */
    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* 恢复默认窗口大小 */
} 



