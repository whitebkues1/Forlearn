/**
 ****************************************************************************************************
 * @file        dht11.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-26
 * @brief       DHT11数字温湿度传感器 驱动代码
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
 * V1.0 20200426
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __DHT11_H
#define __DHT11_H 

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* DHT11 引脚 定义 */

#define DHT11_DQ_GPIO_PORT                  GPIOG
#define DHT11_DQ_GPIO_PIN                   SYS_GPIO_PIN11
#define DHT11_DQ_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 8; }while(0)   /* PG口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define DHT11_DQ_OUT(x)         sys_gpio_pin_set(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, x)  /* 数据端口输出 */
#define DHT11_DQ_IN             sys_gpio_pin_get(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN)     /* 数据端口输入 */


uint8_t dht11_init(void);   /* 初始化DHT11 */
uint8_t dht11_check(void);  /* 检测是否存在DHT11 */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);   /* 读取温湿度 */

#endif















