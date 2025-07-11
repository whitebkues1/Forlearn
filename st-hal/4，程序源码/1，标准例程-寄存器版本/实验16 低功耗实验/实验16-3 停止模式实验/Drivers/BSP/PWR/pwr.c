/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
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
 * V1.2 20200423
 * 1, 支持进入停止模式及唤醒
 * 2, 新增pwr_enter_stop函数
 *
 ****************************************************************************************************
 */

#include "./BSP/PWR/pwr.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/**
 * @brief       初始化PVD电压监视器
 * @param       pls: 电压等级
 *   @arg       000,2.2V;   001,2.3V
 *   @arg       010,2.4V;   011,2.5V;
 *   @arg       100,2.6V;   101,2.7V;
 *   @arg       110,2.8V;   111,2.9V;
 * @retval      无
 */
void pwr_pvd_init(uint8_t pls)
{
    RCC->APB1ENR |= 1 << 28;    /* 使能电源时钟 */
    PWR->CR &= ~(3 << 5);       /* PLS[2:0]清零 */
    PWR->CR |=  pls << 5;       /* PLS[2:0] = pls,注意不要超范围! */
    PWR->CR |= 1 << 4;          /* PVDE = 1,使能PVD检测 */

    EXTI->IMR |= 1 << 16;       /* 开启line16 上的中断(PVD & AVD) */
    EXTI->FTSR |= 1 << 16;      /* line16 上事件下降沿触发 */
    EXTI->RTSR |= 1 << 16;      /* line16 上事件上升降沿触发 */

    sys_nvic_init(3, 3, PVD_IRQn, 2);   /* 组2，最低优先级 */
}

/**
 * @brief       PVD中断服务函数
 * @param       无
 * @retval      无
 */
void PVD_IRQHandler(void)
{
    if (PWR->CSR & (1 << 2))   /* 电压比PLS所选电压还低 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED); /* LCD显示电压低 */
        LED1(0);                                                        /* 点亮绿灯, 表明电压低了 */
    }
    else
    {
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);/* LCD显示电压正常 */
        LED1(1);                                                        /* 灭掉绿灯 */
    }

    EXTI->PR |= 1 << 16;   /* 清除line16的中断标志 */
}

/**
 * @brief       WK_UP按键 外部中断服务程序
 * @param       无
 * @retval      无
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    EXTI->PR = PWR_WKUP_GPIO_PIN;   /* 清除WKUP所在中断线 的中断标志位 */
}

/**
 * @brief       低功耗模式下的按键初始化(用于唤醒睡眠模式/停止模式)
 * @param       无
 * @retval      无
 */
void pwr_wkup_key_init(void)
{
    PWR_WKUP_GPIO_CLK_ENABLE();     /* WKUP时钟使能 */

    sys_gpio_set(PWR_WKUP_GPIO_PORT, PWR_WKUP_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);    /* WKUP引脚模式设置,下拉输入 */

    sys_nvic_ex_config(PWR_WKUP_GPIO_PORT, PWR_WKUP_GPIO_PIN, SYS_GPIO_RTIR);   /* WKUP配置为上升沿触发中断 */ 
    sys_nvic_init( 2, 2, PWR_WKUP_INT_IRQn, 2); /* 抢占2，子优先级2，组2 */
}

/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
    EXTI->PR = PWR_WKUP_GPIO_PIN;   /* 清除WKUP上的中断标志位 */
    sys_wfi_set();      /* 执行WFI指令, 进入睡眠模式 */
}

/**
 * @brief       进入停止模式
 * @param       无
 * @retval      无
 */
void pwr_enter_stop(void)
{
    RCC->APB1ENR |= 1 << 28;        /* 使能电源时钟 */
    EXTI->PR = PWR_WKUP_GPIO_PIN;   /* 清除WKUP上的中断标志位 */

    PWR->CR |= 1 << 0;              /* LPDS=1, 停止模式下电压调节器处于低功耗模式 */
    PWR->CR &= ~(1 << 1);           /* PDDS=0, CPU深度睡眠时进入停止模式 */
    SCB->SCR |= 1 << 2;             /* 使能SLEEPDEEP位 */

    sys_wfi_set();                  /* 执行WFI指令, 进入停止模式 */
    
    SCB->SCR &= ~(1 << 2);          /* 关闭SLEEPDEEP位 */
}












