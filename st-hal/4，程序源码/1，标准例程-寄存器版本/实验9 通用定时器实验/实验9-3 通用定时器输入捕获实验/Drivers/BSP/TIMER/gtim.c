/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2020-04-20
 * @brief       通用定时器 驱动代码
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
 * V1.0 20200420
 * 第一次发布
 * V1.1 20200420
 * 新增gtim_timx_pwm_chy_init函数
 * V1.2 20200420
 * 1,新增gtim_timx_cap_chy_init函数 
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/**
 * @brief       通用定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_INT_IRQHandler(void)
{ 
    if (GTIM_TIMX_INT->SR & 0X0001)   /* 溢出中断 */
    {
        LED1_TOGGLE();
    }

    GTIM_TIMX_INT->SR &= ~(1 << 0); /* 清除中断标志位 */
} 

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();
    GTIM_TIMX_INT->ARR = arr;           /* 设定计数器自动重装值 */
    GTIM_TIMX_INT->PSC = psc;           /* 设置预分频器  */
    GTIM_TIMX_INT->DIER |= 1 << 0;      /* 允许更新中断 */
    GTIM_TIMX_INT->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, GTIM_TIMX_INT_IRQn, 2); /* 抢占1，子优先级3，组2 */
}

/**
 * @brief       通用定时器TIMX 通道Y PWM输出 初始化函数（使用PWM模式1）
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_PWM_CHY;
    GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();    /* TIMX 通道 IO口时钟使能 */
    GTIM_TIMX_PWM_CHY_CLK_ENABLE();         /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_PWM_CHY_GPIO_PORT, GTIM_TIMX_PWM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX PWM CHY 引脚模式设置 */

    GTIM_TIMX_PWM_CHY_GPIO_REMAP();         /* IO口REMAP设置, 非必需! */

    GTIM_TIMX_PWM->ARR = arr;       /* 设定计数器自动重装值 */
    GTIM_TIMX_PWM->PSC = psc;       /* 设置预分频器  */
    GTIM_TIMX_PWM->BDTR |= 1 << 15; /* 使能MOE位(仅TIM1/8 有此寄存器,必须设置MOE才能输出PWM), 其他通用定时器, 这个
                                     * 寄存器是无效的, 所以设置/不设置并不影响结果, 为了兼容这里统一改成设置MOE位
                                     */

    if (chy <= 2)
    {
        GTIM_TIMX_PWM->CCMR1 |= 6 << (4 + 8 * (chy - 1));   /* CH1/2 PWM模式1 */
        GTIM_TIMX_PWM->CCMR1 |= 1 << (3 + 8 * (chy - 1));   /* CH1/2 预装载使能 */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_PWM->CCMR2 |= 6 << (4 + 8 * (chy - 3));   /* CH3/4 PWM模式1 */
        GTIM_TIMX_PWM->CCMR2 |= 1 << (3 + 8 * (chy - 3));   /* CH3/4 预装载使能 */
    }

    GTIM_TIMX_PWM->CCER |= 1 << (4 * (chy - 1));        /* OCy 输出使能 */
    GTIM_TIMX_PWM->CCER |= 1 << (1 + 4 * (chy - 1));    /* OCy 低电平有效 */
    GTIM_TIMX_PWM->CR1 |= 1 << 7;   /* ARPE使能 */
    GTIM_TIMX_PWM->CR1 |= 1 << 0;   /* 使能定时器TIMX */
}

/* 输入捕获状态(g_timxchy_cap_sta)
 * [7]  :0,没有成功的捕获;1,成功捕获到一次.
 * [6]  :0,还没捕获到高电平;1,已经捕获到高电平了.
 * [5:0]:捕获高电平后溢出的次数,最多溢出63次,所以最长捕获值 = 63*65536 + 65535 = 4194303
 *       注意:为了通用,我们默认ARR和CCRy都是16位寄存器,对于32位的定时器(如:TIM5),也只按16位使用
 *       按1us的计数频率,最长溢出时间为:4194303 us, 约4.19秒
 */
uint8_t g_timxchy_cap_sta = 0;  /* 输入捕获状态 */
uint32_t g_timxchy_cap_val =0 ; /* 输入捕获值 */

/**
 * @brief       通用定时器TIMX 输入捕获 中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_CAP_IRQHandler(void)
{
    uint16_t tsr = GTIM_TIMX_CAP->SR;       /* 获取中断状态 */
    uint8_t chy = GTIM_TIMX_CAP_CHY;        /* 需要捕获的通道 */

    if ((g_timxchy_cap_sta & 0X80) == 0)    /* 还未成功捕获 */
    {
        if (tsr & (1 << 0)) /* 溢出中断 */
        {
            if (g_timxchy_cap_sta & 0X40)   /* 已经捕获到高电平了 */
            {
                if ((g_timxchy_cap_sta & 0X3F) == 0X3F) /* 高电平太长了 */
                {
                    g_timxchy_cap_sta |= 0X80;  /* 标记成功捕获了一次 */
                    g_timxchy_cap_val = 0XFFFF;
                }
                else    /* 还可以累加高电平长度 */
                {
                    g_timxchy_cap_sta++;   
                }
            }
        }

        if (tsr & (1 << chy))   /* 通道y 发生了捕获事件 */
        {
            if (g_timxchy_cap_sta & 0X40)   /* 捕获到一个下降沿 */
            {
                g_timxchy_cap_sta |= 0X80;  /* 标记成功捕获到一次高电平脉宽 */
                g_timxchy_cap_val = GTIM_TIMX_CAP_CHY_CCRX; /* 获取当前的捕获值. */
                
                GTIM_TIMX_CAP->CCER &= ~(1 << (1 + 4 * (chy - 1)));/* CCyP = 0 设置为上升沿捕获 */ 
            }
            else    /* 还未开始,第一次捕获上升沿 */
            {
                g_timxchy_cap_val = 0;
                g_timxchy_cap_sta = 0X40;   /* 标记捕获到了上升沿 */
                GTIM_TIMX_CAP->CNT = 0;     /* 计数器清空 */ 
                GTIM_TIMX_CAP->CCER |= 1 << (1 + 4 * (chy - 1));    /* CCyP = 1 设置为下降沿捕获 */ 
            }
        }
    }

    GTIM_TIMX_CAP->SR = 0;     /* 清除所有中断标志位 */
}

/**
 * @brief       通用定时器TIMX 通道Y 输入捕获 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_CAP_CHY;
    GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();   /* TIMX 通道IO口时钟使能 */
    GTIM_TIMX_CAP_CHY_CLK_ENABLE();        /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_CAP_CHY_GPIO_PORT, GTIM_TIMX_CAP_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PD);    /* TIMX PWM CHY 复用功能 下拉 */

    GTIM_TIMX_CAP->ARR = arr;       /* 设定计数器自动重装值 */
    GTIM_TIMX_CAP->PSC = psc;       /* 设置预分频器  */

    if (chy <= 2)
    {
        GTIM_TIMX_CAP->CCMR1 |= 1 << 8 * (chy - 1);        /* CCyS[1:0]   = 01 选择输入端 IC1/2映射到TI1/2上 */
        GTIM_TIMX_CAP->CCMR1 |= 0 << (2 + 8 * (chy - 1));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        GTIM_TIMX_CAP->CCMR1 |= 0 << (4 + 8 * (chy - 1));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_CAP->CCMR2 |= 1 << 8 * (chy - 3);        /* CCyS[1:0]   = 01 选择输入端 IC3/4映射到TI3/4上 */
        GTIM_TIMX_CAP->CCMR2 |= 0 << (2 + 8 * (chy - 3));  /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        GTIM_TIMX_CAP->CCMR2 |= 0 << (4 + 8 * (chy - 3));  /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }

    GTIM_TIMX_CAP->CCER |= 1 << (4 * (chy - 1));       /* CCyE = 1 输入捕获使能 */
    GTIM_TIMX_CAP->CCER |= 0 << (1 + 4 * (chy - 1));   /* CCyP = 0 捕获上升沿 ,注意:CCyNP使用默认值0 */

    GTIM_TIMX_CAP->EGR  |= 1 << 0;  /* 软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效 */
    GTIM_TIMX_CAP->DIER |= 1 << 1;  /* 允许捕获中断 */
    GTIM_TIMX_CAP->DIER |= 1 << 0;  /* 允许更新中断 */
    GTIM_TIMX_CAP->CR1  |= 1 << 0;  /* 使能定时器TIMX */

    sys_nvic_init(1, 3, GTIM_TIMX_CAP_IRQn, 2);/* 抢占1，子优先级3，组2 */
}















