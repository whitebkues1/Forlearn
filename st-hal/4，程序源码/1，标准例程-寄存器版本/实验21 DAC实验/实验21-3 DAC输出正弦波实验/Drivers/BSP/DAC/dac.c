/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
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
 * V1.2 20200424
 * 1, 支持DMA DAC输出任意波形
 * 2, 新增dac_dma_wave_init和dac_dma_wave_enable函数
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       DAC初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=36Mhz=27.8ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 3us (F103数据手册有写)
 *              因此DAC输出的最高速度约为:333Khz, 以10个点为一个周期, 最大能输出33Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint8_t outx)
{
    uint8_t offset = (outx - 1) * 16;   /* OUT1 / OUT2在DAC寄存器里面的偏移量(OUT1 = 0, OUT2 = 16) */

    RCC->APB2ENR |= 1 << 2;;        /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    RCC->APB1ENR |= 1 << 29;        /* 使能DAC1的时钟 */

    sys_gpio_set(GPIOA, 1 << (4 + outx),/* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
                 SYS_GPIO_MODE_AIN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* DAC引脚模式设置,模拟输入 */

    DAC1->CR = 0;                   /* DR寄存器清零 */
    DAC1->CR |= 1 << (1 + offset);  /* BOFF1/2=1,关闭DAC通道1/2输出缓冲 */
    DAC1->CR |= 0 << (2 + offset);  /* TEN1/2=0,不使用触发功能 */
    DAC1->CR |= 0 << (3 + offset);  /* TSEL1/2[3:0]=0,软件触发 */
    DAC1->CR |= 0 << (6 + offset);  /* WAVE1/2[1:0]=0,不使用波形发生 */
    DAC1->CR |= 0 << (12 + offset); /* DMAEN1/2=0,DAC1/2 DMA不使能 */
    DAC1->CR |= 0 << (14 + offset); /* CEN1/2=0,DAC1/2工作在普通模式 */

    DAC1->CR |= 1 << (0 + offset);  /* 使能DAC1_OUT1/2 */
    DAC1->DHR12R1 = 0;  /* 通道1输出 0 */
    DAC1->DHR12R2 = 0;  /* 通道2输出 0 */
}

/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* 如果值大于等于4096, 则取4095 */

    if (outx == 1)   /* 通道1 */
    {
        DAC1->DHR12R1 = temp;
    }
    else            /* 通道2 */
    {
        DAC1->DHR12R2 = temp;
    }
}

/**
 * @brief       设置DAC_OUT1输出三角波
 *   @note      输出频率 ≈ 1000 / (dt * samples) Khz, 不过在dt较小的时候,比如小于5us时, 由于delay_us
 *              本身就不准了(调用函数,计算等都需要时间,延时很小的时候,这些时间会影响到延时), 频率会偏小.
 * 
 * @param       maxval : 最大值(0 < maxval < 4096), (maxval + 1)必须大于等于samples/2
 * @param       dt     : 每个采样点的延时时间(单位: us)
 * @param       samples: 采样点的个数, samples必须小于等于(maxval + 1) * 2 , 且maxval不能等于0
 * @param       n      : 输出波形个数,0~65535
 *
 * @retval      无
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval;           /* 递增量 */
    
    if((maxval + 1) <= samples)return ; /* 数据不合法 */
        
    incval = (maxval + 1) / (samples / 2);  /* 计算递增量 */
    
    for(j = 0; j < n; j++)
    { 
        DAC1->DHR12R1 = 0;  /* 先输出0 */
        for(i = 0; i < (samples / 2); i++)  /* 输出上升沿 */
        { 
             DAC1->DHR12R1 +=  incval;
             delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  /* 输出下降沿 */
        {
             DAC1->DHR12R1 -=  incval;
             delay_us(dt);
        }
    }
}

/**
 * @brief       DAC DMA输出波形初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=36Mhz=27.8ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 4us (F103数据手册有写)
 *              因此DAC输出的最高速度约为:250Khz, 以10个点为一个周期, 最大能输出25Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void dac_dma_wave_init(uint8_t outx, uint32_t par, uint32_t mar)
{
    DMA_Channel_TypeDef *dmax_chy;
    uint8_t offset = (outx - 1) * 16;   /* OUT1 / OUT2在DAC寄存器里面的偏移量(OUT1 = 0, OUT2 = 16) */

    if (outx == 1)
    {
        dmax_chy = DMA2_Channel3;       /* OUT1对应DMA2_Channel3 */
    }
    else
    {
        dmax_chy = DMA2_Channel4;       /* OUT2对应DMA2_Channel4 */

    }
    
    RCC->APB1ENR |= 1 << 5;             /* TIM7时钟使能 */ 
    
    dac_init(outx);                     /* 初始化DAC 对应通道 */
    /* 有差异部分 单独设置 */
    DAC1->CR |= 1 << (12 + offset);     /* DMAEN1/2 = 1,通道1/2 DMA使能 */
    DAC1->CR |= 2 << (3 + offset);      /* TSEL1/2[2:0] = 2, TIM7_TRGO触发 */
    DAC1->CR |= 1 << (2 + offset);      /* TEN1/2 = 1,触发使能 */

    /* 使能DMA时钟, 并设置CPAR和CMAR寄存器 */
    dma_basic_config(dmax_chy, par, mar);

    dmax_chy->CCR = 0;          /* 复位CCR寄存器 */
    dmax_chy->CCR |= 1 << 4;    /* DIR = 1 , 存储器到外设模式 */
    dmax_chy->CCR |= 1 << 5;    /* CIRC = 1, 循环模式 */
    dmax_chy->CCR |= 0 << 6;    /* PINC = 0, 外设地址非增量模式 */
    dmax_chy->CCR |= 1 << 7;    /* MINC = 1, 存储器增量模式 */
    dmax_chy->CCR |= 1 << 8;    /* PSIZE[1:0] = 1, 外设数据宽度为: 16位 */
    dmax_chy->CCR |= 1 << 10;   /* MSIZE[1:0] = 1, 存储器数据宽度: 16位 */
    dmax_chy->CCR |= 1 << 12;   /* PL[1:0] =  1, 中等优先级 */
    dmax_chy->CCR |= 0 << 14;   /* MEM2MEM = 0 , 非存储器到存储器模式 */
}

/**
 * @brief       DAC DMA使能波形输出
 *   @note      TIM7的输入时钟频率(f)来自APB1, f = 36M * 2 = 72Mhz.
 *              DAC触发频率 ftrgo = f / ((psc + 1) * (arr + 1))
 *              波形频率 = ftrgo / ndtr; 
 *
 * @param       outx        : 要初始化的通道. 1,通道1; 2,通道2
 * @param       ndtr        : DMA通道单次传输数据量
 * @param       arr         : TIM7的自动重装载值
 * @param       psc         : TIM7的分频系数
 * @retval      无
 */
void dac_dma_wave_enable(uint8_t outx, uint16_t cndtr, uint16_t arr, uint16_t psc)
{
    TIM7->CR1 &= ~(1 << 0);         /* 关闭定时器TIMX,停止触发 */
    if (outx == 1)
    {
        dma_enable(DMA2_Channel3, cndtr);   /* 重新启动DMA2_Channel3传输 */
    }
    else
    {
        dma_enable(DMA2_Channel4, cndtr);   /* 重新启动DMA2_Channel4传输 */
    }

    TIM7->ARR = arr;                /* 设定计数器自动重装值 */
    TIM7->PSC = psc;                /* 设置预分频器  */
    TIM7->CR2 |= 2 << 4;            /* 定时器7的更新事件触发DAC转换 */ 
    TIM7->CR1 |= 1 << 0;            /* 使能定时器TIMX */ 
}






















































