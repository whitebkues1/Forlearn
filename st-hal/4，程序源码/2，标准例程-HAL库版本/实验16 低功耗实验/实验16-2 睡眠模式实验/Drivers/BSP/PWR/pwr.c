/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-23
 * @brief       低功耗模式 驱动代码
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
 * V1.0 20200423
 * 第一次发布
 * V1.1 20200423
 * 1, 支持进入睡眠模式及唤醒
 * 2, 新增pwr_wkup_key_init和pwr_enter_sleep函数
 *
 ****************************************************************************************************
 */

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/**
 * @brief       初始化PVD电压监视器
 * @param       pls: 电压等级(PWR_PVD_detection_level)
 *   @arg       PWR_PVDLEVEL_0,2.2V;
 *   @arg       PWR_PVDLEVEL_1,2.3V;
 *   @arg       PWR_PVDLEVEL_2,2.4V;
 *   @arg       PWR_PVDLEVEL_3,2.5V;
 *   @arg       PWR_PVDLEVEL_4,2.6V;
 *   @arg       PWR_PVDLEVEL_5,2.7V;
 *   @arg       PWR_PVDLEVEL_6,2.8V;
 *   @arg       PWR_PVDLEVEL_7,2.9V;
 * @retval      无
 */
void pwr_pvd_init(uint32_t pls)
{
    PWR_PVDTypeDef pwr_pvd = {0};

    __HAL_RCC_PWR_CLK_ENABLE();                      /* 使能PWR时钟 */
    
    pwr_pvd.PVDLevel = pls;                          /* 检测电压级别 */
    pwr_pvd.Mode = PWR_PVD_MODE_IT_RISING_FALLING;   /* 使用中断线的上升沿和下降沿双边缘触发 */
    HAL_PWR_ConfigPVD(&pwr_pvd);

    HAL_NVIC_SetPriority(PVD_IRQn, 3 ,3);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
    HAL_PWR_EnablePVD();                             /* 使能PVD检测 */
}

/**
 * @brief       PVD中断服务函数
 * @param       无
 * @retval      无
 */
void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

/**
 * @brief       PVD中断服务回调函数
 * @param       无
 * @retval      无
 */
void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) /* 电压比PLS所选电压还低 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED); /* LCD显示电压低 */
        LED1(0);                                                        /* 点亮绿灯, 表明电压低了 */
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE); /* LCD显示电压正常 */
        LED1(1);                                                         /* 灭掉绿灯 */
    }
}

/***********************************睡眠模式实验程序*******************************************/

/**
 * @brief       低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式/待机模式)
 * @param       无
 * @retval      无
 */
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    PWR_WKUP_GPIO_CLK_ENABLE();                           /* WKUP时钟使能 */

    gpio_init_struct.Pin = PWR_WKUP_GPIO_PIN;             /* WKUP引脚 */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;          /* 中断,上升沿 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                /* 下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;        /* 高速 */
    HAL_GPIO_Init(PWR_WKUP_GPIO_PORT, &gpio_init_struct); /* WKUP引脚初始化 */

    HAL_NVIC_SetPriority(PWR_WKUP_INT_IRQn, 2, 2);        /* 抢占优先级2，子优先级2 */
    HAL_NVIC_EnableIRQ(PWR_WKUP_INT_IRQn);
}

/**
 * @brief       WK_UP按键 外部中断服务程序
 * @param       无
 * @retval      无
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(PWR_WKUP_GPIO_PIN);
}

/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
    HAL_SuspendTick();  /* 暂停滴答时钟，防止通过滴答时钟中断唤醒 */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); /* 执行WFI指令, 进入睡眠模式 */
}

/**
 * @brief       外部中断回调函数
 * @param       GPIO_Pin:中断线引脚
 * @note        此函数会被PWR_WKUP_INT_IRQHandler()调用
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == PWR_WKUP_GPIO_PIN)
    {
        /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
    }
}








