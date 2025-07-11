/**
 ****************************************************************************************************
 * @file        tpad.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-04-21
 * @brief       电容触摸按键(TPAD) 驱动代码
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
 * V1.0 20200421
 * 第一次发布
 *
 * V1.1 20221101
 * 1, 修改tpad_scan, 针对综合实验, 加强抗干扰特性 
 * 2, 修改tpad_reset, 提前放电, 增强抗干扰特性
 *
 ****************************************************************************************************
 */

#include "./BSP/TPAD/tpad.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"


/* 空载的时候(没有手按下),计数器需要的时间
 * 这个值应该在每次开机的时候被初始化一次
 */
volatile uint16_t g_tpad_default_val = 0;   /* 空载的时候(没有手按下),计数器需要的时间 */

/**
 * @brief       初始化触摸按键
 * @param       psc     : 分频系数(值越小, 越灵敏, 最小值为: 1)
 * @retval      0, 初始化成功; 1, 初始化失败;
 */
uint8_t tpad_init(uint16_t psc)
{
    uint16_t buf[10];
    uint16_t temp;
    uint8_t j, i;
    tpad_timx_cap_init(TPAD_ARR_MAX_VAL, psc - 1);  /* 以72 / (psc - 1) Mhz的频率计数 */

    for (i = 0; i < 10; i++)    /* 连续读取10次 */
    {
        buf[i] = tpad_get_val();
        delay_ms(10);
    }

    for (i = 0; i < 9; i++)     /* 排序 */
    {
        for (j = i + 1; j < 10; j++)
        {
            if (buf[i] > buf[j])/* 升序排列 */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }

    temp = 0;

    for (i = 2; i < 8; i++)     /* 取中间的6个数据进行平均 */
    {
        temp += buf[i];
    }
    
    g_tpad_default_val = temp / 6;
    printf("g_tpad_default_val:%d\r\n", g_tpad_default_val);

    if (g_tpad_default_val > TPAD_ARR_MAX_VAL / 2)
    {
        return 1;   /* 初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常! */
    }

    return 0;
}

/**
 * @brief       复位TPAD
 *   @note      我们将TPAD按键看做是一个电容, 当手指按下/不按下时容值有变化
 *              该函数将GPIO设置成推挽输出, 然后输出0, 进行放电, 然后再设置
 *              GPIO为浮空输入, 等待外部大电阻慢慢充电
 * @param       无
 * @retval      无
 */
static void tpad_reset(void)
{
    sys_gpio_set(TPAD_GPIO_PORT, TPAD_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* TPAD引脚推挽输出 */

    sys_gpio_pin_set(TPAD_GPIO_PORT, TPAD_GPIO_PIN, 0); /* TPAD引脚输出0, 放电 */
    
    delay_ms(1);
    
    TPAD_TIMX_CAP->SR = 0;      /* 清除标记 */
    TPAD_TIMX_CAP->CNT = 0;     /* 归零 */

    sys_gpio_set(TPAD_GPIO_PORT, TPAD_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_NONE);  /* TPAD引脚浮空输入 */
}

/**
 * @brief       得到定时器捕获值
 *   @note      如果超时, 则直接返回定时器的计数值
 *              我们定义超时时间为: TPAD_ARR_MAX_VAL - 500
 * @param       无
 * @retval      捕获值/计数值（超时的情况下返回）
 */
static uint16_t tpad_get_val(void)
{
    tpad_reset();

    while ((TPAD_TIMX_CAP->SR & (1 << TPAD_TIMX_CAP_CHY)) == 0) /* 等待通道CHY捕获上升沿 */
    {
        if (TPAD_TIMX_CAP->CNT > TPAD_ARR_MAX_VAL - 500)
        {
            return TPAD_TIMX_CAP->CNT;  /* 超时了,直接返回CNT的值 */
        }
    }
    
    /* 检测完了, 直接把引脚 恢复低电平 */
    sys_gpio_set(TPAD_GPIO_PORT, TPAD_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* TPAD引脚推挽输出 */

    sys_gpio_pin_set(TPAD_GPIO_PORT, TPAD_GPIO_PIN, 0); /* TPAD引脚输出0, 放电 */

    return TPAD_TIMX_CAP_CHY_CCRX;      /* 返回捕获/比较值 */
}

/**
 * @brief       读取n次, 取最大值
 * @param       n       ：连续获取的次数
 * @retval      n次读数里面读到的最大读数值
 */
static uint16_t tpad_get_maxval(uint8_t n)
{
    uint16_t temp = 0;
    uint16_t maxval = 0;

    while (n--)
    {
        temp = tpad_get_val();  /* 得到一次值 */

        if (temp > maxval)maxval = temp;
    }

    return maxval;
}

/**
 * @brief       扫描触摸按键
 * @param       mode    ：扫描模式
 *   @arg       0, 不支持连续触发(按下一次必须松开才能按下一次);
 *   @arg       1, 支持连续触发(可以一直按下)
 * @retval      0, 没有按下; 1, 有按下;
 */
uint8_t tpad_scan(uint8_t mode)
{
    #define TPAD_CHECK_NUM      3   /* 连续TPAD_CHECK_NUM次有效, 才认为有效 */
    #define TPAD_CHECK_TIME     10  /* 每次间隔时间为TPAD_CHECK_TIME ms */
    uint8_t i = 0;

    static uint8_t keyen = 0;   /* 0, 可以开始检测;  >0, 还不能开始检测; */
    uint8_t res = 0;
    uint8_t sample = 3;         /* 默认采样次数为3次 */
    uint16_t rval;
    

    if (mode)
    {
        sample = 3;     /* 支持连按的时候，设置采样次数为3次 */
        keyen = 0;      /* 支持连按, 每次调用该函数都可以检测 */
    }

    while(i < TPAD_CHECK_NUM)
    {
        rval = tpad_get_maxval(sample);
        
        if(rval > (g_tpad_default_val *2))
        {
            //printf("tr:%d\r\n", rval);  /* 输出计数值, 调试的时候才用到 */
            delay_ms(TPAD_CHECK_TIME);
            i++;
        }
        else
        {
            break;
        }
    }

    if(i == TPAD_CHECK_NUM) /* 连续3次检测有效 */
    {
        printf("r:%d,%d\r\n", rval, keyen);   /* 输出计数值, 调试的时候才用到 */
        
        if(!keyen)          /* keyen == 0, 则可以继续返回有效状态了 */
        {
            res = 1;
        }

        keyen = 3;          /* 3次无效后, 才能再次检测 */
    }
    else if (keyen)         /* 检测到一次无效 */
    {
        keyen--;  
    }
    return res;
}

/**
 * @brief       定时器输入捕获设置
 * @param       arr     ：自动重装值
 * @param       psc     ：时钟预分频数
 * @retval      无
 */
static void tpad_timx_cap_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = TPAD_TIMX_CAP_CHY;
    TPAD_GPIO_CLK_ENABLE();         /* TPAD引脚 时钟使能 */
    TPAD_TIMX_CAP_CHY_CLK_ENABLE(); /* 定时器 时钟使能 */

    sys_gpio_set(TPAD_GPIO_PORT, TPAD_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_NONE);  /* TPAD引脚浮空输入 */

    TPAD_TIMX_CAP->ARR = arr;       /* 设定计数器自动重装值 */
    TPAD_TIMX_CAP->PSC = psc;       /* 设置预分频器 */

    if (chy <= 2)
    {
        TPAD_TIMX_CAP->CCMR1 |= 1 << 8 * (chy - 1);         /* CCyS[1:0]   = 01 选择输入端 IC1/2映射到TI1/2上 */
        TPAD_TIMX_CAP->CCMR1 |= 0 << (2 + 8 * (chy - 1));   /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        TPAD_TIMX_CAP->CCMR1 |= 0 << (4 + 8 * (chy - 1));   /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }
    else if (chy <= 4)
    {
        TPAD_TIMX_CAP->CCMR2 |= 1 << 8 * (chy - 3);         /* CCyS[1:0]   = 01 选择输入端 IC3/4映射到TI3/4上 */
        TPAD_TIMX_CAP->CCMR2 |= 0 << (2 + 8 * (chy - 3));   /* ICyPSC[1:0] = 00 输入捕获不分频,全捕获 */
        TPAD_TIMX_CAP->CCMR2 |= 0 << (4 + 8 * (chy - 3));   /* ICyF[3:0]   = 00 输入端滤波 不滤波 */
    }

    TPAD_TIMX_CAP->CCER |= 1 << (4 * (chy - 1));        /* CCyE = 1 输入捕获使能 */
    TPAD_TIMX_CAP->CCER |= 0 << (1 + 4 * (chy - 1));    /* CCyP = 0 捕获上升沿 ,注意:CCyNP使用默认值0 */
    
    TPAD_TIMX_CAP->CR1  |= 1 << 0;  /* 使能定时器TIMX */
}













