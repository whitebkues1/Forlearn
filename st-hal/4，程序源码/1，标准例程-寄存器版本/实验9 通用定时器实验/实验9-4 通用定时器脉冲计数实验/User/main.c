/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ͨ�ö�ʱ��������� ʵ��
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
#include "./BSP/TIMER/gtim.h"

int main(void)
{
    uint32_t curcnt = 0;
    uint32_t oldcnt = 0;
    uint8_t key = 0;
    uint8_t t = 0;

    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */
    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                 /* ��ʼ��LED */
    key_init();                 /* ��ʼ������ */
    gtim_timx_cnt_chy_init(0);  /* ��ʱ��������ʼ��, ����Ƶ */
    gtim_timx_cnt_chy_restart();/* �������� */

    while (1)
    {
        key = key_scan(0);      /* ɨ�谴�� */

        if (key == KEY0_PRES)   /* KEY0��������,�������� */
        {
            gtim_timx_cnt_chy_restart();        /* ������������ */
        }

        curcnt = gtim_timx_cnt_chy_get_count(); /* ��ȡ����ֵ */

        if (oldcnt != curcnt)
        {
            oldcnt = curcnt;
            printf("CNT:%d\r\n", oldcnt);       /* ��ӡ������� */
        }

        t++;

        if (t > 20)         /* 200ms����һ�� */
        {
            t = 0;
            LED0_TOGGLE();  /* LED0��˸ ,��ʾ�������� */
        }

        delay_ms(10);
    }
}
















