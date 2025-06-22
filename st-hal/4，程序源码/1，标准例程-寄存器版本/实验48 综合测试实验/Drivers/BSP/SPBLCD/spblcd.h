/**
 ****************************************************************************************************
 * @file        spblcd.h
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

#ifndef __SPBLCD_H
#define	__SPBLCD_H

#include "./BSP/LCD/lcd.h"
#include "./SYSTEM/delay/delay.h"

#include "./SYSTEM/sys/sys.h"



#define SLCD_DMA_MAX_TRANS  60*1024     /* DMAһ����ഫ��60K�ֽ� */

#define SPILCD_BASE         1024*1024*15.1  /**
                                            * SPB����,��SPI FLASH�ĵ�15.1M�ֽڿ�ʼ�洢,ռ�����Ϊ571.875KB�ֽ�
                                            * SPILCD_BASE�����Һ���ֱ��ʱ�־��
                                            * 0����ʾ240*320������1����ʾ320*480������2����ʾ480*800����������ֵ���Ƿ�
                                            * ����320*240����,���ռ��   224*600*2��262K�ֽ�
                                            * ����480*320����,���ռ��   364*800*2��568K�ֽ�
                                            * ����800*480����,���ռ��   610*480*2��571K�ֽ�
                                            */

#define SPILCD_END_ADDR     SPILCD_BASE + 600*1024  /* SPB�������ݽ���λ��,���ڴ��lcd����,������ڻ����洢���ݣ�571.8KB�� */

 
 
uint8_t slcd_frame_lcd2spi(uint32_t offset,uint16_t width,uint16_t height);

void slcd_spi2_mode(uint8_t mode);
void slcd_dma_init(void);
void slcd_dma_enable(uint32_t x);
void slcd_frame_show(uint16_t x);

#endif

























