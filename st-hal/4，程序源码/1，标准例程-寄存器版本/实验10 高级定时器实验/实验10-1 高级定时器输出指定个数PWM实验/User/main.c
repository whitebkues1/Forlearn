/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-21
 * @brief       �߼���ʱ�����ָ������PWM ʵ��
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
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/TIMER/atim.h"

int main(void)
{
    uint8_t key = 0;
    uint8_t t = 0;

    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */
    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                 /* ��ʼ��LED */
    key_init();                 /* ��ʼ������ */

    /* �� LED1 ��������Ϊ����ģʽ, �����PC6��ͻ */
    sys_gpio_set(LED1_GPIO_PORT, LED1_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    atim_timx_npwm_chy_init(5000 - 1, 7200 - 1);    /* 10Khz�ļ���Ƶ��,2hz��PWMƵ��. */

    ATIM_TIMX_NPWM_CHY_CCRX = 2500; /* ����PWMռ�ձ�,50%,�������Կ���ÿһ��PWM����,LED2(BLUE)
                                     * ��һ��ʱ��������,һ��ʱ�������,LED2����һ��,��ʾһ��PWM��
                                     */
    atim_timx_npwm_chy_set(5);  /* ���5��PWM��(����LED2(BLUE)��˸5��) */

    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)   /* KEY0���� */
        {
            atim_timx_npwm_chy_set(5);  /* ���5��PWM��(����TIM8_CH1, ��PC6���5������) */
        }

        t++;
        delay_ms(10);
        
        if (t > 50)             /* ����LED0��˸, ��ʾ��������״̬ */
        {
            t = 0;
            LED0_TOGGLE();
        }
    }
}

















