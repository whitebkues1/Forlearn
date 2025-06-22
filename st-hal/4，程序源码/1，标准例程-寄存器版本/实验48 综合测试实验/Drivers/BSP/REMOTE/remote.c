/**
 ****************************************************************************************************
 * @file        remote.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-25
 * @brief       ����ң�ؽ��� ��������
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
 * V1.0 20200425
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/REMOTE/remote.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"


/**
 * @brief       ����ң�س�ʼ��
 *   @note      ����IO�Լ���ʱ�������벶��
 * @param       ��
 * @retval      ��
 */
void remote_init(void)
{
    uint8_t chy = REMOTE_IN_TIMX_CHY;
    
    REMOTE_IN_GPIO_CLK_ENABLE();            /* �����������GPIOʱ��ʹ�� */
    REMOTE_IN_TIMX_CHY_CLK_ENABLE();        /* ��ʱ��ʱ��ʹ�� */

    sys_gpio_set(REMOTE_IN_GPIO_PORT, REMOTE_IN_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);    /* IOģʽ����, �������ó��������� */

    REMOTE_IN_TIMX->ARR = 10000;            /* �趨�������Զ���װֵ ���10ms��� */
    REMOTE_IN_TIMX->PSC = 72 - 1;           /* Ԥ��Ƶ��,1M�ļ���Ƶ��, 1us��1. */

    if (chy <= 2)
    {
        REMOTE_IN_TIMX->CCMR1 |= 1 << 8 * (chy - 1);        /* CCyS[1:0]   = 01 ѡ������� IC1/2ӳ�䵽TI1/2�� */
        REMOTE_IN_TIMX->CCMR1 |= 0 << (2 + 8 * (chy - 1));  /* ICyPSC[1:0] = 00 ���벶�񲻷�Ƶ,ȫ���� */
        REMOTE_IN_TIMX->CCMR1 |= 0 << (4 + 8 * (chy - 1));  /* ICyF[3:0]   = 00 ������˲� ���˲� */
    }
    else if (chy <= 4)
    {
        REMOTE_IN_TIMX->CCMR2 |= 1 << 8 * (chy - 3);        /* CCyS[1:0]   = 01 ѡ������� IC3/4ӳ�䵽TI3/4�� */
        REMOTE_IN_TIMX->CCMR2 |= 0 << (2 + 8 * (chy - 3));  /* ICyPSC[1:0] = 00 ���벶�񲻷�Ƶ,ȫ���� */
        REMOTE_IN_TIMX->CCMR2 |= 0 << (4 + 8 * (chy - 3));  /* ICyF[3:0]   = 00 ������˲� ���˲� */
    }

    REMOTE_IN_TIMX->CCER |= 1 << (4 * (chy - 1));       /* CCyE = 1 ���벶��ʹ�� */
    REMOTE_IN_TIMX->CCER |= 0 << (1 + 4 * (chy - 1));   /* CCyP = 0 ���������� ,ע��:CCyNPʹ��Ĭ��ֵ0 */

    REMOTE_IN_TIMX->EGR  |= 1 << 0;         /* ������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч */
    REMOTE_IN_TIMX->DIER |= 1 << chy;       /* ����chy�����ж� */
    REMOTE_IN_TIMX->DIER |= 1 << 0;         /* ��������ж� */
    REMOTE_IN_TIMX->CR1  |= 1 << 0;         /* ʹ�ܶ�ʱ��TIMX */
    
    sys_nvic_init(1, 2, REMOTE_IN_TIMX_IRQn, 2);    /* ��ռ1�������ȼ�2����2 */
}


/* ң��������״̬
 * [7]  : �յ����������־
 * [6]  : �õ���һ��������������Ϣ
 * [5]  : ����
 * [4]  : ����������Ƿ��Ѿ�������
 * [3:0]: �����ʱ��
 */
uint8_t g_remote_sta = 0;
uint32_t g_remote_data = 0; /* ������յ������� */
uint8_t  g_remote_cnt = 0;  /* �������µĴ��� */

/**
 * @brief       ��ʱ��X�жϷ�����
 * @param       ��
 * @retval      ��
 */
void REMOTE_IN_TIMX_IRQHandler(void)
{
    uint16_t dval;  /* �½���ʱ��������ֵ */
    uint16_t tsr = REMOTE_IN_TIMX->SR;  /* ��ȡ�ж�״̬ */
    uint8_t chy = REMOTE_IN_TIMX_CHY;   /* ��Ҫ�����ͨ�� */

    if (tsr & 0X01)             /* ������� */
    {
        if (g_remote_sta & 0x80)      /* �ϴ������ݱ����յ��� */
        {
            g_remote_sta &= ~0X10;    /* ȡ���������Ѿ��������� */

            if ((g_remote_sta & 0X0F) == 0X00)
            {
                g_remote_sta |= 1 << 6; /* ����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ� */
            }
            
            if ((g_remote_sta & 0X0F) < 14)
            {
                g_remote_sta++;
            }
            else
            {
                g_remote_sta &= ~(1 << 7);    /* ���������ʶ */
                g_remote_sta &= 0XF0; /* ��ռ����� */
            }
        }
    }

    if (tsr & (1 << chy))   /* ������(chyͨ��)�ж� */
    {
        if (RDATA)  /* �����ز��� */
        {
            REMOTE_IN_TIMX->CCER |= 1 << (1 + 4 * (chy - 1));   /* CCyP = 1 ����Ϊ�½��ز��� */  
            REMOTE_IN_TIMX->CNT = 0;        /* ��ն�ʱ��ֵ */
            g_remote_sta |= 0X10;           /* ����������Ѿ������� */
        }
        else    /* �½��ز��� */
        {
            dval = REMOTE_IN_TIMX_CCRY;     /* ��ȡCCRxҲ������CC1IF��־λ */ 
            REMOTE_IN_TIMX->CCER &= ~(1 << (1 + 4 * (chy - 1)));/* CCyP = 0 ����Ϊ�����ز��� */ 

            if (g_remote_sta & 0X10)        /* ���һ�θߵ�ƽ���� */
            {
                if (g_remote_sta & 0X80)    /* ���յ��������� */
                {

                    if (dval > 300 && dval < 800)   /* 560Ϊ��׼ֵ,560us */
                    {
                        g_remote_data >>= 1;            /* ����һλ */
                        g_remote_data &= ~(0x80000000); /* ���յ�0 */
                    }
                    else if (dval > 1400 && dval < 1800)    /* 1680Ϊ��׼ֵ,1680us */
                    {
                        g_remote_data >>= 1;            /* ����һλ */
                        g_remote_data |= 0x80000000;    /* ���յ�1 */
                    }
                    else if (dval > 2000 && dval < 3000)    /* �õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms */
                    {
                        g_remote_cnt++;         /* ������������1�� */
                        g_remote_sta &= 0XF0;   /* ��ռ�ʱ�� */
                    }
                }
                else if (dval > 4200 && dval < 4700)    /* 4500Ϊ��׼ֵ4.5ms */
                {
                    g_remote_sta |= 1 << 7; /* ��ǳɹ����յ��������� */
                    g_remote_cnt = 0;       /* ����������������� */
                }
            }

            g_remote_sta &= ~(1 << 4);
        }
    }

    REMOTE_IN_TIMX->SR = 0;     /* ����жϱ�־λ */
}

/**
 * @brief       ������ⰴ��(���ư���ɨ��)
 * @param       ��
 * @retval      0   , û���κΰ�������
 *              ����, ���µİ�����ֵ
 */
uint8_t remote_scan(void)
{
    uint8_t sta = 0;
    uint8_t t1, t2;

    if (g_remote_sta & (1 << 6))    /* �õ�һ��������������Ϣ�� */
    {
        t1 = g_remote_data;                 /* �õ���ַ�� */
        t2 = (g_remote_data >> 8) & 0xff;   /* �õ���ַ���� */

        if ((t1 == (uint8_t)~t2) && t1 == REMOTE_ID)    /* ����ң��ʶ����(ID)����ַ */
        {
            t1 = (g_remote_data >> 16) & 0xff;
            t2 = (g_remote_data >> 24) & 0xff;

            if (t1 == (uint8_t)~t2)
            {
                sta = t1;           /* ��ֵ��ȷ */
            }
        }

        if ((sta == 0) || ((g_remote_sta & 0X80) == 0)) /* �������ݴ���/ң���Ѿ�û�а����� */
        {
            g_remote_sta &= ~(1 << 6);  /* ������յ���Ч������ʶ */
            g_remote_cnt = 0;           /* ����������������� */
        }
    }

    return sta;
}
































