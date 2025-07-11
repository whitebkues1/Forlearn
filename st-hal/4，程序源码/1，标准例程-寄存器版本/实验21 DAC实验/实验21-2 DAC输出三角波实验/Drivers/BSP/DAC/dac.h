/**
 ****************************************************************************************************
 * @file        dac.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-24
 * @brief       DAC 驱动代码
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
 * V1.0 20200424
 * 第一次发布
 * V1.1 20200424
 * 新增dac_triangular_wave函数
 *
 ****************************************************************************************************
 */

#ifndef __DAC_H
#define __DAC_H

#include "./SYSTEM/sys/sys.h"



void dac_init(uint8_t outx);    /* DAC通道1初始化 */ 
void dac_set_voltage(uint8_t outx, uint16_t vol);   /* 设置通道1/2输出电压 */ 
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n);   /* 输出三角波 */

#endif

















