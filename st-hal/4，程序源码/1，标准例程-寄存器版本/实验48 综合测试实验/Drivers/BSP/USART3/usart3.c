/**
 ****************************************************************************************************
 * @file        usart3.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-29
 * @brief       ����3 ��������
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
 * V1.0 20221029
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/USART3/usart3.h"
#include "./BSP/TIMER/timer.h"
#include "./MALLOC/malloc.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "ucos_ii.h"


/* ���ڽ��ջ����� */
uint8_t g_usart3_rx_buf[USART3_MAX_RECV_LEN];       /* ���ջ���,���USART3_MAX_RECV_LEN���ֽ� */


/**
 * ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
 * ���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
 * �κ�����,���ʾ�˴ν������.
 * ���յ�������״̬
 * [15]:0,û�н��յ�����;1,���յ���һ������.
 * [14:0]:���յ������ݳ���
 */
volatile uint16_t g_usart3_rx_sta = 0;

/**
 * @brief       ����3�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART3_IRQHandler(void)
{
    uint8_t res;
    OSIntEnter();

    if (USART3->SR & (1 << 5))  /* ���յ����� */
    {
        res = USART3->DR;

        if ((g_usart3_rx_sta & (1 << 15)) == 0) /* �������һ������,��û�б�����,���ٽ����������� */
        {
            if (g_usart3_rx_sta < USART3_MAX_RECV_LEN)      /* �����Խ������� */
            {
                TIM7->CNT = 0;                  /* ��������� */

                if (g_usart3_rx_sta == 0)       /* ʹ�ܶ�ʱ��7���ж� */
                {
                    TIM7->CR1 |= 1 << 0;        /* ʹ�ܶ�ʱ��7 */
                }

                g_usart3_rx_buf[g_usart3_rx_sta++] = res;   /* ��¼���յ���ֵ */
            }
            else
            {
                g_usart3_rx_sta |= 1 << 15;     /* ǿ�Ʊ�ǽ������ */
            }
        }
    }

    OSIntExit();
}

/**
 * @brief       ����3��ʼ������
 * @param       sclk: ����X��ʱ��ԴƵ��(��λ: MHz)
 *              ����1 ��ʱ��Դ����: PCLK2 = 72Mhz
 *              ����2 - 5 ��ʱ��Դ����: PCLK1 = 36Mhz
 * @note        ע��: ����������ȷ��sclk, ���򴮿ڲ����ʾͻ������쳣.
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @retval      ��
 */
void usart3_init(uint32_t sclk, uint32_t baudrate)
{
    USART3_TX_GPIO_CLK_ENABLE();    /* ʹ�ܴ���TX��ʱ�� */
    USART3_RX_GPIO_CLK_ENABLE();    /* ʹ�ܴ���RX��ʱ�� */
    
    sys_gpio_set(USART3_TX_GPIO_PORT, USART3_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* ����TX�� ģʽ���� */

    sys_gpio_set(USART3_RX_GPIO_PORT, USART3_RX_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* ����RX�� �������ó�����ģʽ */

    RCC->APB1ENR |= 1 << 18;        /* ʹ�ܴ���3ʱ�� */
    RCC->APB1RSTR |= 1 << 18;       /* ��λ����3 */
    RCC->APB1RSTR &= ~(1 << 18);    /* ֹͣ��λ */
    
    /* ���������� */
    USART3->BRR = (sclk * 1000000) / (baudrate); /*  ���������� */
    USART3->CR1 |= 0X200C;          /* 1λֹͣ,��У��λ */
    
    /* ʹ�ܽ����ж� */
    USART3->CR1 |= 1 << 5;          /* ���ջ������ǿ��ж�ʹ�� */
    
    sys_nvic_init(0, 1, USART3_IRQn, 2); /* ��2 */
    
    tim7_int_init(99, 7199);        /* 10ms�ж� */
    TIM7->CR1 &= ~(1 << 0);         /* �رն�ʱ��7 */
    g_usart3_rx_sta = 0;            /* ���� */
}

/**
 * @brief       ����3,printf ����
 * @param       fmt     : ��ʽ������
 * @param       ...     : �ɱ����
 * @retval      ��
 */
void u3_printf(char *fmt, ...)
{
    uint16_t i, j;
    uint8_t *pbuf;
    va_list ap;
    
    pbuf = mymalloc(SRAMIN, USART3_MAX_SEND_LEN);   /* �����ڴ� */

    if (!pbuf)  /* �ڴ�����ʧ�� */
    {
        printf("u3 malloc error\r\n");
        return ;
    }

    va_start(ap, fmt);
    vsprintf((char *)pbuf, fmt, ap);
    va_end(ap);
    i = strlen((const char *)pbuf); /* �˴η������ݵĳ��� */

    for (j = 0; j < i; j++)         /* ѭ���������� */
    {
        while ((USART3->SR & 0X40) == 0);   /* ѭ������,ֱ��������� */

        USART3->DR = pbuf[j];
    }

    myfree(SRAMIN, pbuf);           /* �ͷ��ڴ� */
}



































