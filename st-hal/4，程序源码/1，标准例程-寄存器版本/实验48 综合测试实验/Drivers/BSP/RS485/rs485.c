/**
 ****************************************************************************************************
 * @file        rs485.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       RS485 ��������
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
 * V1.0 20200424
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/RS485/rs485.h"
#include "./SYSTEM/delay/delay.h"


#ifdef RS485_EN_RX      /* ���ʹ���˽��� */

uint8_t g_RS485_rx_buf[RS485_REC_LEN];  /* ���ջ���, ��� RS485_REC_LEN ���ֽ�. */
uint8_t g_RS485_rx_cnt = 0;             /* ���յ������ݳ��� */

void RS485_UX_IRQHandler(void)
{
    uint8_t res;

    if (RS485_UX->SR & (1 << 5))    /* ���յ����� */
    {
        res = RS485_UX->DR;

        if (g_RS485_rx_cnt < RS485_REC_LEN)         /* ������δ�� */
        {
            g_RS485_rx_buf[g_RS485_rx_cnt] = res;   /* ��¼���յ���ֵ */
            g_RS485_rx_cnt++;                       /* ������������1 */
        }
    }
}

#endif

/**
 * @brief       RS485��ʼ������
 *   @note      �ú�����Ҫ�ǳ�ʼ������
 * @param       sclk    : ����X��ʱ��ԴƵ��(��λ: MHz)
 *              ����1 ��ʱ��Դ����: PCLK2 = 72Mhz
 *              ����2 - 5 ��ʱ��Դ����: PCLK1 = 36Mhz
 * @note        ע��: ����������ȷ��sclk, ���򴮿ڲ����ʾͻ������쳣.
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @retval      ��
 */
void rs485_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;

    /* IO �� ʱ������ */
    RS485_RE_GPIO_CLK_ENABLE(); /* ʹ�� RS485_RE ��ʱ�� */
    RS485_TX_GPIO_CLK_ENABLE(); /* ʹ�� ����TX�� ʱ�� */
    RS485_RX_GPIO_CLK_ENABLE(); /* ʹ�� ����RX�� ʱ�� */
    RS485_UX_CLK_ENABLE();      /* ʹ�� ���� ʱ�� */

    sys_gpio_set(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);  /* RS485_RE �� ģʽ���� */

    sys_gpio_set(RS485_TX_GPIO_PORT, RS485_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* ����TX �� ģʽ���� */

    sys_gpio_set(RS485_RX_GPIO_PORT, RS485_RX_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* ����RX �� �������ó�����ģʽ */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;  /* �õ�BRR, ��������������� */
    
/* ���������� */
    RS485_UX->BRR = temp;       /* ���������� */
    RS485_UX->CR1 = 0;          /* ����CR1�Ĵ��� */
    RS485_UX->CR1 |= 0 << 12;   /* M = 0, 1����ʼλ, 8������λ, n��ֹͣλ(��USART_CR2 STOP[1:0]ָ��, Ĭ����0, ��ʾ1��ֹͣλ) */
    RS485_UX->CR1 |= 1 << 3;    /* TE = 1, ���ڷ���ʹ�� */
#if RS485_EN_RX  /* ���ʹ���˽��� */
    /* ʹ�ܽ����ж� */
    RS485_UX->CR1 |= 1 << 2;    /* RE = 1, ���ڽ���ʹ�� */
    RS485_UX->CR1 |= 1 << 5;    /* RXNEIE = 1, ���ջ������ǿ��ж�ʹ�� */
    sys_nvic_init(3, 3, RS485_UX_IRQn, 2); /* ��2��������ȼ� */
#endif
    RS485_UX->CR1 |= 1 << 13;   /* UE = 1, ����ʹ�� */

    RS485_RE(0);                /* Ĭ��Ϊ����ģʽ */
}

/**
 * @brief       RS485����len���ֽ�
 * @param       buf     : �������׵�ַ
 * @param       len     : ���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ���� RS485_REC_LEN ���ֽ�)
 * @retval      ��
 */
void rs485_send_data(uint8_t *buf, uint8_t len)
{
    uint8_t t;
    RS485_RE(1);    /* ���뷢��ģʽ */

    for (t = 0; t < len; t++)   /* ѭ���������� */
    {
        while ((RS485_UX->SR & 0X40) == 0); /* �ȴ����ͽ��� */

        RS485_UX->DR = buf[t];
    }

    while ((RS485_UX->SR & 0X40) == 0);     /* �ȴ����ͽ��� */

    g_RS485_rx_cnt = 0;
    RS485_RE(0);    /* �������ģʽ */
}

/**
 * @brief       RS485��ѯ���յ�������
 * @param       buf     : ���ջ������׵�ַ
 * @param       len     : ���յ������ݳ���
 *   @arg               0   , ��ʾû�н��յ��κ�����
 *   @arg               ����, ��ʾ���յ������ݳ���
 * @retval      ��
 */
void rs485_receive_data(uint8_t *buf, uint8_t *len)
{
    uint8_t rxlen = g_RS485_rx_cnt;
    uint8_t i = 0;
    *len = 0;       /* Ĭ��Ϊ0 */
    delay_ms(10);   /* �ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս��� */

    if (rxlen == g_RS485_rx_cnt && rxlen)   /* ���յ�������,�ҽ�������� */
    {
        for (i = 0; i < rxlen; i++)
        {
            buf[i] = g_RS485_rx_buf[i];
        }

        *len = g_RS485_rx_cnt;  /* ��¼�������ݳ��� */
        g_RS485_rx_cnt = 0;     /* ���� */
    }
}














