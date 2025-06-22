/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2020-04-21
 * @brief       �߼���ʱ�� ��������
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
 * V1.0 20200421
 * ��һ�η���
 * V1.1 20200421
 * 1, ����atim_timx_comp_pwm_init����, ʵ������Ƚ�ģʽPWM�������
 * V1.2 20200421
 * 1, ����atim_timx_cplm_pwm_init����
 * 2, ����atim_timx_cplm_pwm_set����
 * ���atim_timx_cplm_pwm_init AC6��__nop()���Ż������⣬
 * ʵ��BTDR֧��12��13λͬʱд�룬�ֿ�д������Ҫ����ʱ�ģ���ҿ���ѡ���ʺ��Լ��ķ���
 * 
 ****************************************************************************************************
 */

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"


/* g_npwm_remain��ʾ��ǰ��ʣ�¶��ٸ�����Ҫ����
 * ÿ����෢��256������
 */
static uint32_t g_npwm_remain = 0;

/**
 * @brief       �߼���ʱ��TIMX NPWM�жϷ�����
 * @param       ��
 * @retval      ��
 */
void ATIM_TIMX_NPWM_IRQHandler(void)
{
    uint16_t npwm = 0;

    if (ATIM_TIMX_NPWM->SR & 0X0001)    /* ����ж�, ��RCR=0 */
    {
        if (g_npwm_remain > 256)        /* ���д���256��������Ҫ���� */
        {
            g_npwm_remain = g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)   /* ����λ��������256��������Ҫ���� */
        {
            npwm = g_npwm_remain % 256;
            g_npwm_remain = 0;          /* û�������� */
        }

        if (npwm)   /* ������Ҫ���� */
        {
            ATIM_TIMX_NPWM->RCR = npwm - 1; /* �����ظ������Ĵ���ֵΪnpwm-1, ��npwm������ */
            ATIM_TIMX_NPWM->EGR |= 1 << 0;  /* ����һ�θ����¼�,�Ը���RCR�Ĵ��� */
            ATIM_TIMX_NPWM->CR1 |= 1 << 0;  /* ʹ�ܶ�ʱ��TIMX */
        } 
        else
        {
            ATIM_TIMX_NPWM->CR1 &= ~(1 << 0);   /* �رն�ʱ��TIMX */
        }
    }

    ATIM_TIMX_NPWM->SR &= ~(1 << 0);    /* ����жϱ�־λ */
}

/**
 * @brief       �߼���ʱ��TIMX ͨ��Y ���ָ������PWM ��ʼ������
 * @note
 *              �߼���ʱ����ʱ������APB2, ��PCLK2 = 72Mhz, ��������PPRE2����Ƶ, ���
 *              �߼���ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = ATIM_TIMX_NPWM_CHY;
    ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE();   /* TIMX ͨ��IO��ʱ��ʹ�� */
    ATIM_TIMX_NPWM_CHY_CLK_ENABLE();        /* TIMX ʱ��ʹ�� */

    sys_gpio_set(ATIM_TIMX_NPWM_CHY_GPIO_PORT, ATIM_TIMX_NPWM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX PWM CHY ����ģʽ���� */

    ATIM_TIMX_NPWM->ARR = arr;       /* �趨�������Զ���װֵ */
    ATIM_TIMX_NPWM->PSC = psc;       /* ����Ԥ��Ƶ��  */
    ATIM_TIMX_NPWM->BDTR |= 1 << 15; /* ʹ��MOEλ, �߼���ʱ����������MOE�������PWM */

    if (chy <= 2)
    {
        ATIM_TIMX_NPWM->CCMR1 |= 6 << (4 + 8 * (chy - 1));  /* CH1/2 PWMģʽ1 */
        ATIM_TIMX_NPWM->CCMR1 |= 1 << (3 + 8 * (chy - 1));  /* CH1/2 Ԥװ��ʹ�� */
    }
    else if (chy <= 4)
    {
        ATIM_TIMX_NPWM->CCMR2 |= 6 << (4 + 8 * (chy - 3));  /* CH3/4 PWMģʽ1 */
        ATIM_TIMX_NPWM->CCMR2 |= 1 << (3 + 8 * (chy - 3));  /* CH3/4 Ԥװ��ʹ�� */
    }

    ATIM_TIMX_NPWM->CCER |= 1 << (4 * (chy - 1));       /* OCy ���ʹ�� */
    ATIM_TIMX_NPWM->CCER |= 0 << (1 + 4 * (chy - 1));   /* OCy �ߵ�ƽ��Ч */

    ATIM_TIMX_NPWM->CR1 |= 1 << 7;  /* ARPEʹ�� */
    ATIM_TIMX_NPWM->DIER |= 1 << 0; /* ��������ж� */
    ATIM_TIMX_NPWM->CR1 |= 1 << 0;  /* ʹ�ܶ�ʱ��TIMX */

    sys_nvic_init(1, 3, ATIM_TIMX_NPWM_IRQn, 2);/* ��ռ1�������ȼ�3����2 */
}

/**
 * @brief       �߼���ʱ��TIMX NPWM����PWM����
 * @param       rcr: PWM�ĸ���, 1~2^32�η���
 * @retval      ��
 */
void atim_timx_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0)return ;

    g_npwm_remain = npwm;               /* ����������� */
    ATIM_TIMX_NPWM->EGR |= 1 << 0;      /* ����һ�θ����¼�,���ж����洦��������� */
    ATIM_TIMX_NPWM->CR1 |= 1 << 0;      /* ʹ�ܶ�ʱ��TIMX */
}


/**
 * @brief       �߼���ʱ��TIMX ����Ƚ�ģʽ ��ʼ��������ʹ������Ƚ�ģʽ��
 * @note
 *              ���ø߼���ʱ��TIMX 4·����Ƚ�ģʽPWM���,ʵ��50%ռ�ձ�,��ͬ��λ����
 *              ע��,����������Ƚ�ģʽ,ÿ2���������ڲ������һ��PWM���,������Ƶ�ʼ���
 *              ����,���ǻ����Կ����ж����ж������޸�CCRx,�Ӷ�ʵ�ֲ�ͬƵ��/��ͬ��λ�Ŀ���
 *              �������ǲ��Ƽ���ôʹ��,��Ϊ����ܵ��·ǳ�Ƶ�����ж�,�Ӷ�ռ�ô���CPU��Դ
 *
 *              �߼���ʱ����ʱ������APB2, ��PCLK2 = 72Mhz, ��������PPRE2����Ƶ, ���
 *              �߼���ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_COMP_CH1_GPIO_CLK_ENABLE();   /* ͨ��1��ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_COMP_CH2_GPIO_CLK_ENABLE();   /* ͨ��2��ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();   /* ͨ��3��ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE();   /* ͨ��4��ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_COMP_CLK_ENABLE();            /* ʹ�ܶ�ʱ��ʱ�� */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH1_GPIO_PORT, ATIM_TIMX_COMP_CH1_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_CH1 ����ģʽ���� */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH2_GPIO_PORT, ATIM_TIMX_COMP_CH2_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_CH2 ����ģʽ���� */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH3_GPIO_PORT, ATIM_TIMX_COMP_CH3_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_CH3 ����ģʽ���� */
    
    sys_gpio_set(ATIM_TIMX_COMP_CH4_GPIO_PORT, ATIM_TIMX_COMP_CH4_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_CH4 ����ģʽ���� */

    ATIM_TIMX_COMP->ARR = arr;          /* �趨�������Զ���װֵ */
    ATIM_TIMX_COMP->PSC = psc;          /* ����Ԥ��Ƶ��  */
    ATIM_TIMX_NPWM->BDTR |= 1 << 15; /* ʹ��MOEλ, �߼���ʱ����������MOE�������PWM */

    ATIM_TIMX_COMP->CCMR1 |= 3 << 4;    /* ͨ��1 ����Ƚ�ģʽ ƥ��ʱ��ת */
    ATIM_TIMX_COMP->CCMR1 |= 1 << 3;    /* ͨ��1 Ԥװ��ʹ�� */

    ATIM_TIMX_COMP->CCMR1 |= 3 << 12;   /* ͨ��2 ����Ƚ�ģʽ ƥ��ʱ��ת */
    ATIM_TIMX_COMP->CCMR1 |= 1 << 11;   /* ͨ��2 Ԥװ��ʹ�� */

    ATIM_TIMX_COMP->CCMR2 |= 3 << 4;    /* ͨ��3 ����Ƚ�ģʽ ƥ��ʱ��ת */
    ATIM_TIMX_COMP->CCMR2 |= 1 << 3;    /* ͨ��3 Ԥװ��ʹ�� */

    ATIM_TIMX_COMP->CCMR2 |= 3 << 12;   /* ͨ��4 ����Ƚ�ģʽ ƥ��ʱ��ת */
    ATIM_TIMX_COMP->CCMR2 |= 1 << 11;   /* ͨ��4 Ԥװ��ʹ�� */

    ATIM_TIMX_COMP->CCER |= 1 << 0;     /* OC1 ���ʹ�� */ 
    ATIM_TIMX_COMP->CCER |= 1 << 4;     /* OC2 ���ʹ�� */ 
    ATIM_TIMX_COMP->CCER |= 1 << 8;     /* OC3 ���ʹ�� */ 
    ATIM_TIMX_COMP->CCER |= 1 << 12;    /* OC4 ���ʹ�� */ 

    ATIM_TIMX_COMP->CR1 |= 1 << 7;      /* ARPEʹ�� */ 
    ATIM_TIMX_COMP->CR1 |= 1 << 0;      /* ʹ�ܶ�ʱ��TIMX */ 
}

/**
 * @brief       �߼���ʱ��TIMX ������� ��ʼ��������ʹ��PWMģʽ1��
 * @note
 *              ���ø߼���ʱ��TIMX �������, һ·OCy һ·OCyN, ���ҿ�����������ʱ��
 *
 *              �߼���ʱ����ʱ������APB2, ��PCLK2 = 72Mhz, ��������PPRE2����Ƶ, ���
 *              �߼���ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = ATIM_TIMX_CPLM_CHY;
    
    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE();   /* ͨ��X��ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE();  /* ͨ��X����ͨ����ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();  /* ͨ��Xɲ�������ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_CPLM_CLK_ENABLE();            /* ʹ�ܶ�ʱ��ʱ�� */

    sys_gpio_set(ATIM_TIMX_CPLM_CHY_GPIO_PORT, ATIM_TIMX_CPLM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_CHX ����ģʽ���� */

    sys_gpio_set(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, ATIM_TIMX_CPLM_CHYN_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_CHXN ����ģʽ���� */

    sys_gpio_set(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, ATIM_TIMX_CPLM_BKIN_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX_BKIN ����ģʽ���� */

    ATIM_TIMX_CPLM_CHYN_GPIO_REMAP();   /* IO��REMAP����, ������Ҫ����, �Ǳ���! */

    ATIM_TIMX_CPLM->ARR = arr;          /* �趨�������Զ���װֵ */
    ATIM_TIMX_CPLM->PSC = psc;          /* ����Ԥ��Ƶ��  */

    if (chy <= 2)
    {
        ATIM_TIMX_CPLM->CCMR1 |= 6 << (4 + 8 * (chy - 1));  /* CH1/2 PWMģʽ1 */
        ATIM_TIMX_CPLM->CCMR1 |= 1 << (3 + 8 * (chy - 1));  /* CH1/2 Ԥװ��ʹ�� */
    }
    else if (chy <= 4)
    {
        ATIM_TIMX_CPLM->CCMR2 |= 6 << (4 + 8 * (chy - 3));  /* CH3/4 PWMģʽ1 */
        ATIM_TIMX_CPLM->CCMR2 |= 1 << (3 + 8 * (chy - 3));  /* CH3/4 Ԥװ��ʹ�� */
    }

    ATIM_TIMX_CPLM->CCER |= 1 << (4 * (chy - 1));       /* OCy ���ʹ�� */
    ATIM_TIMX_CPLM->CCER |= 1 << (1 + 4 * (chy - 1));   /* OCy �͵�ƽ��Ч */
    ATIM_TIMX_CPLM->CCER |= 1 << (2 + 4 * (chy - 1));   /* OCyN ���ʹ�� */
    ATIM_TIMX_CPLM->CCER |= 1 << (3 + 4 * (chy - 1));   /* OCyN �͵�ƽ��Ч */
     
    ATIM_TIMX_CPLM->BDTR |= 1 << 13;    /* BKP = 1, BKIN�ߵ�ƽ��Ч */
    __nop();
    __nop();
    __nop();                            /* д��BTDR��Ҫ�ȴ�����һ��APBʱ�ӣ�ʵ��AC6��ֻ��һ��nop�ᱻ�Ż�����������3��nop���� */
    ATIM_TIMX_CPLM->BDTR |= 1 << 12;    /* BKE = 1, ʹ��BKIN���, ������治Ҫ�ӵȴ�, ��Ϊ������û�в���BDTR�Ĵ����� */
    
    ATIM_TIMX_CPLM->CR1 |= 1 << 7;      /* ARPEʹ�� */
    ATIM_TIMX_CPLM->CR1 |= 2 << 8;      /* CKD[1:0] = 10, tDTS = 4 * tCK_INT = Ft / 4 = 18Mhz*/
    ATIM_TIMX_CPLM->CR1 |= 1 << 0;      /* ʹ�ܶ�ʱ��TIMX */
    ATIM_TIMX_CPLM->BDTR |= 1 << 14;    /* AOE=1,����MOE�ܱ������¼���λ,MOEλ������ݴ�λ�Զ�����ֵ���������仯��Ҫ������ã�LOCKλ���ú��λ�����޸� */    
}

/**
 * @brief       ��ʱ��TIMX ��������Ƚ�ֵ & ����ʱ��
 * @param       ccr: ����Ƚ�ֵ
 * @param       dtg: ����ʱ��
 *   @arg       dtg[7:5]=0xxʱ, ����ʱ�� = dtg[7:0] * tDTS
 *   @arg       dtg[7:5]=10xʱ, ����ʱ�� = (64 + dtg[6:0]) * 2  * tDTS
 *   @arg       dtg[7:5]=110ʱ, ����ʱ�� = (32 + dtg[5:0]) * 8  * tDTS
 *   @arg       dtg[7:5]=111ʱ, ����ʱ�� = (32 + dtg[5:0]) * 16 * tDTS
 *   @note      tDTS = 1 / (Ft /  CKD[1:0]) = 1 / 18M = 55.56ns
 * @retval      ��
 */
void atim_timx_cplm_pwm_set(uint16_t ccr, uint8_t dtg)
{ 
    ATIM_TIMX_CPLM->BDTR &= ~(0XFF << 0);   /* DTG[7:0]���� */
    ATIM_TIMX_CPLM->BDTR |= dtg << 0;       /* DTG[7:0] = dtg, ��������ʱ�� */
    ATIM_TIMX_CPLM->BDTR |= 1 << 15;        /* MOE=1,ʹ������� */    
    ATIM_TIMX_CPLM_CHY_CCRY = ccr;          /* ���ñȽϼĴ��� */
}






















