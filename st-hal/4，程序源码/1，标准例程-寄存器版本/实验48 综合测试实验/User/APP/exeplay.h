/**
 ****************************************************************************************************
 * @file        exeplay.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-11-16
 * @brief       APP-运行器 代码
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
 * V1.1 20221116
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __EXEPLAY_H
#define __EXEPLAY_H

#include "common.h"


/** 
 * APP相关信息设置
 * 思路:把BIN文件先存放到外部SRAM,然后设置标志位,产生一次软复位.软复位之后,系统判断标志位,
 * 如果需要运行app,则先把标志位清空,然后复制外部SRAM的APP代码到内部sram,最后跳转到app的起始
 * 地址,开始运行app代码.
 * 注意:
 * 这里默认设置APP的尺寸最大为EXEPLAY_APP_SIZE字节.
 * 实际APP大小假设为APP_SIZE，那么：
 * SRAM的 0X20010000 - APP_SIZE ~ 0X20010000, 这个区间为APP的SRAM区
 * SRAM的 0X20001000 ~ 0X20001000 + APP_SIZE, 这个区间为APP的CODE区
 * 实际上APP_SIZE必须小于EXEPLAY_APP_SIZE
 */

#define EXEPLAY_APP_SIZE    55*1024             /* app代码的最大尺寸.这里为60K字节（还包括了SRAM，实际上不可能运行55K的代码）. */
#define EXEPLAY_APP_BASE    0x20001000          /* app执行代码的目的地址,也就是将要运行的代码存放的地址 */
#define	EXEPLAY_SRC_BASE    1024*1024*15.9      /* app执行代码的源地址,也就是软复位之前,app代码存放的地址 */


typedef  void (*dummyfun)(void);    /* 定义一个函数类型 */
extern dummyfun jump2app;           /* 假函数,让PC指针跑到新的main函数去 */
void exeplay_write_appmask(uint16_t val);
void exeplay_app_check(void);
uint8_t exe_play(void);

#endif
































