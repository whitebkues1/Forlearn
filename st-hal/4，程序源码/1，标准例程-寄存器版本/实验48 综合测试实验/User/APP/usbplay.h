/**
 ****************************************************************************************************
 * @file        usbplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-31
 * @brief       APP-USB连接 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.1 20221031
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __USBPLAY_H
#define __USBPLAY_H

#include "os.h"
#include "common.h"


void usb_port_set(uint8_t enable);
uint8_t usb_play(void);

#endif














