/**
 ****************************************************************************************************
 * @file        rtc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-22
 * @brief       RTC ��������
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
 * V1.0 20200422
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/RTC/rtc.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

#include "calendar.h"


//_calendar_obj calendar;     /* ʱ��ṹ�� */

/**
 * @brief       RTCд�������SRAM
 * @param       bkrx : �����Ĵ������,��Χ:0~41
 * @param       data : Ҫд�������,16λ����
 * @retval      ��
 */
void rtc_write_bkr(uint32_t bkrx, uint16_t data)
{
    uint32_t temp = 0;
    PWR->CR |= 1 << 8;  /* ȡ��������д���� */
    temp = BKP_BASE + 4 + bkrx * 4;
    (*(uint16_t *)temp) = data;
}

/**
 * @brief       RTC��ȡ������SRAM
 * @param       bkrx : �����Ĵ������,��Χ:0~41
 * @retval      ��ȡ����ֵ
 */
uint16_t rtc_read_bkr(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = BKP_BASE + 4 + bkrx * 4;
    return (*(uint16_t *)temp); /* ���ض�ȡ����ֵ */
}

/**
 * @brief       RTC��ʼ��
 *   @note
 *              Ĭ�ϳ���ʹ��LSE,��LSE����ʧ�ܺ�,�л�ΪLSI.
 *              ͨ��BKP�Ĵ���0��ֵ,�����ж�RTCʹ�õ���LSE/LSI:
 *              ��BKP0==0X5050ʱ,ʹ�õ���LSE
 *              ��BKP0==0X5051ʱ,ʹ�õ���LSI
 *              ע��:�л�LSI/LSE������ʱ��/���ڶ�ʧ,�л�������������.
 *
 * @param       ��
 * @retval      0,�ɹ�
 *              1,�����ʼ��ģʽʧ��
 */
uint8_t rtc_init(void)
{
    /* ����ǲ��ǵ�һ������ʱ�� */
    uint16_t bkpflag = 0;
    uint16_t retry = 200;
    uint32_t tempreg = 0;
    uint32_t clockfreq = 0;

    RCC->APB1ENR |= 1 << 28;        /* ʹ�ܵ�Դʱ�� */
    RCC->APB1ENR |= 1 << 27;        /* ʹ�ܱ���ʱ�� */
    PWR->CR |= 1 << 8;              /* ȡ��������д���� */

    bkpflag = rtc_read_bkr(0);      /* ��ȡBKP0��ֵ */

    if (bkpflag != 0X5050)          /* ֮ǰʹ�õĲ���LSE */
    {
        RCC->BDCR |= 1 << 16;       /* ����������λ */   
        RCC->BDCR &= ~(1 << 16);    /* ����������λ���� */
        RCC->BDCR |= 1 << 0;        /* �����ⲿ�������� */
        
        while (retry && ((RCC->BDCR & 0X02) == 0))  /* �ȴ�LSE׼���� */
        {
            retry--;
            delay_ms(5);
        }
        
        tempreg = RCC->BDCR;        /* ��ȡBDCR��ֵ */
        tempreg &= ~(3 << 8);       /* ����8/9λ */

        if (retry == 0)             /* ����LSEʧ��? */
        {
            RCC->CSR |= 1 << 0;         /* LSIʹ�� */
        
            while (!(RCC->CSR & 0x02)); /* �ȴ�LSI���� */
            
            tempreg |= 1 << 9;          /* LSE����ʧ��,����LSI. */
            clockfreq = 40000 - 1;      /* LSIƵ��Լ40Khz(�ο�F103�����ֲ�˵��) */
            rtc_write_bkr(0, 0X5051);   /* ����Ѿ���ʼ������,ʹ��LSI */
        }
        else
        {
            tempreg |= 1 << 8;          /* ѡ��LSE,��ΪRTCʱ�� */
            clockfreq = 32768 - 1;      /* LSEƵ��Ϊ32.769Khz */
            rtc_write_bkr(0, 0X5050);   /* ����Ѿ���ʼ������,ʹ��LSE */
        }

        tempreg |= 1 << 15;             /* ʹ��RTCʱ�� */
        RCC->BDCR = tempreg;            /* ��������BDCR�Ĵ��� */

        while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ���������� */

        while (!(RTC->CRL & (1 << 3))); /* �ȴ�RTC�Ĵ���ͬ�� */

        RTC->CRH |= 1 << 0;             /* SECF = 1, �������ж� */
        RTC->CRH |= 1 << 1;             /* ALRF = 1, ���������ж� */

        while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ���������� */

        RTC->CRL |= 1 << 4;             /* �������� */
        RTC->PRLH = 0X0000;
        RTC->PRLL = clockfreq;          /* ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��32767 */
        RTC->CRL &= ~(1 << 4);          /* ���ø��� */

        while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ���������� */

        if (bkpflag != 0X5051)          /* BKP0�����ݼȲ���0X5050,Ҳ����0X5051,˵���ǵ�һ������,��Ҫ����ʱ������. */
        {
            rtc_set_time(2020, 4, 22, 6, 59, 55);   /* ����ʱ�� */
        }
    }
    else     /* ϵͳ������ʱ */
    {
        retry = 30;     /* ���⿨�� */
        
        while ((!(RTC->CRL & (1 << 3)) && retry))   /* �ȴ�RTC�Ĵ���ͬ�� */
        {
            delay_ms(5);
            retry--;
        }

        retry = 100;    /* ���LSI/LSE�Ƿ��������� */
        
        tempreg = RTC->DIVL;            /* ��ȡDIVL�Ĵ�����ֵ */
        while (retry)
        {
            delay_ms(5);
            retry--;
            
            if (tempreg != RTC->DIVL)   /* �Ա�DIVL��tempreg, ����в���, ���˳� */
            {
                break;                  /* DIVL != tempreg, ˵��RTC�ڼ���, ˵������û���� */
            }
        }
        if (retry == 0)
        {
            rtc_write_bkr(0, 0XFFFF);   /* ��Ǵ����ֵ */
            RCC->BDCR = 1 << 16;        /* ��λBDCR */
            delay_ms(10);
            RCC->BDCR = 0;              /* ������λ */
            return 1;                   /* ��ʼ��ʧ�� */
        }
        else
        {
            RTC->CRH |= 0X01;           /* �������ж� */

            while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ���������� */
        }
    }

    sys_nvic_init(0, 0, RTC_IRQn, 2);   /* ���ȼ����� */
    rtc_get_time();  /* ����ʱ�� */
    return 0;
}

/**
 * @brief       RTCʱ���ж�
 *   @note      �����ж� / �����ж� ����ͬһ���жϷ�����
 *              ����RTC_CRL�Ĵ����� SECF �� LARF λ�������ĸ��ж�
 * @param       ��
 * @retval      ��
 */
void RTC_IRQHandler(void)
{
    OSIntEnter();
    rtc_get_time();             /* ����ʱ�� */

    if (RTC->CRL & (1 << 0))    /* SECF = 1, �����ж� */
    {
        RTC->CRL &= ~(1 << 0);  /* SECF = 0, �������ж� */
        //printf("sec:%d\r\n", calendar.sec);   /* ��ӡ���� */
    }

    if (RTC->CRL & (1 << 1))    /* ALRF = 1, �����ж� */
    {
        RTC->CRL &= ~(1 << 1);  /* ALRF = 0, �������ж� */

        /* �������ʱ�� */
        printf("Alarm Time:%d-%d-%d %d:%d:%d\n", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);
        alarm.ringsta |= 1 << 7;/* �������� */
    }

    RTC->CRL &= ~(1 << 2);      /* OWF = 0, �������жϱ�־ */

    while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ����������, ���ȴ�RTOFF == 1 */
    OSIntExit();
}

/**
 * @brief       �ж�����Ƿ�������
 *   @note      �·�������:
 *              �·�   1  2  3  4  5  6  7  8  9  10 11 12
 *              ����   31 29 31 30 31 30 31 31 30 31 30 31
 *              ������ 31 28 31 30 31 30 31 31 30 31 30 31
 * @param       year : ���
 * @retval      0, ������; 1, ������;
 */
static uint8_t rtc_is_leap_year(uint16_t year)
{
    /* �������: ��������겻���İ������� */
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       ����ʱ��, ����������ʱ����
 *   @note      ��1970��1��1��Ϊ��׼, �����ۼ�ʱ��
 *              �Ϸ���ݷ�ΧΪ: 1970 ~ 2105��
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @param       hour  : Сʱ
 * @param       min   : ����
 * @param       sec   : ����
 * @retval      0, �ɹ�; 1, ʧ��;
 */
uint8_t rtc_set_time(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = rtc_date2sec(syear, smon, sday, hour, min, sec); /* ��������ʱ����ת������������ */

    /* ����ʱ�� */
    RCC->APB1ENR |= 1 << 28;    /* ʹ�ܵ�Դʱ�� */
    RCC->APB1ENR |= 1 << 27;    /* ʹ�ܱ���ʱ�� */
    PWR->CR |= 1 << 8;          /* ȡ��������д���� */
    /* ���������Ǳ����! */
    
    RTC->CRL |= 1 << 4;         /* �������� */
    RTC->CNTL = seccount & 0xffff;
    RTC->CNTH = seccount >> 16;
    RTC->CRL &= ~(1 << 4);      /* ���ø��� */

    while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ���������� */

    rtc_get_time();             /* ������֮�����һ������ */
    return 0;
}

/**
 * @brief       ��������, ���嵽������ʱ����
 *   @note      ��1970��1��1��Ϊ��׼, �����ۼ�ʱ��
 *              �Ϸ���ݷ�ΧΪ: 1970 ~ 2105��
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @param       hour  : Сʱ
 * @param       min   : ����
 * @param       sec   : ����
 * @retval      0, �ɹ�; 1, ʧ��;
 */
uint8_t rtc_set_alarm(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t seccount = 0;

    seccount = rtc_date2sec(syear, smon, sday, hour, min, sec); /* ��������ʱ����ת������������ */

    /* ����ʱ�� */
    RCC->APB1ENR |= 1 << 28;    /* ʹ�ܵ�Դʱ�� */
    RCC->APB1ENR |= 1 << 27;    /* ʹ�ܱ���ʱ�� */
    PWR->CR |= 1 << 8;          /* ȡ��������д���� */
    /* ���������Ǳ����! */
    
    RTC->CRL |= 1 << 4;         /* �������� */
    RTC->ALRL = seccount & 0xffff;
    RTC->ALRH = seccount >> 16;
    RTC->CRL &= ~(1 << 4);      /* ���ø��� */

    while (!(RTC->CRL & (1 << 5))); /* �ȴ�RTC�Ĵ���������� */

    return 0;
}

/**
 * @brief       �õ���ǰ��ʱ��
 *   @note      �ú�����ֱ�ӷ���ʱ��, ʱ�����ݱ�����calendar�ṹ������
 * @param       ��
 * @retval      ��
 */
void rtc_get_time(void)
{
    static uint16_t daycnt = 0;
    uint32_t seccount = 0;
    uint32_t temp = 0;
    uint16_t temp1 = 0;
    const uint8_t month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};   /* ƽ����·����ڱ� */

    seccount = RTC->CNTH;       /* �õ��������е�ֵ(������) */
    seccount <<= 16;
    seccount += RTC->CNTL;

    temp = seccount / 86400;    /* �õ�����(��������Ӧ��) */

    if (daycnt != temp)         /* ����һ���� */
    {
        daycnt = temp;
        temp1 = 1970;           /* ��1970�꿪ʼ */

        while (temp >= 365)
        {
            if (rtc_is_leap_year(temp1))    /* ������ */
            {
                if (temp >= 366)
                {
                    temp -= 366;    /* ����������� */
                }
                else
                {
                    break;
                }
            }
            else
            {
                temp -= 365;    /* ƽ�� */
            }
            
            temp1++;
        }

        calendar.year = temp1;  /* �õ���� */
        temp1 = 0;

        while (temp >= 28)      /* ������һ���� */
        {
            if (rtc_is_leap_year(calendar.year) && temp1 == 1)  /* �����ǲ�������/2�·� */
            {
                if (temp >= 29)
                {
                    temp -= 29; /* ����������� */
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (temp >= month_table[temp1])
                {
                    temp -= month_table[temp1]; /* ƽ�� */
                }
                else
                {
                    break;
                }
            }

            temp1++;
        }

        calendar.month = temp1 + 1;     /* �õ��·� */
        calendar.date = temp + 1;       /* �õ����� */
    }

    temp = seccount % 86400;            /* �õ������� */
    calendar.hour = temp / 3600;        /* Сʱ */
    calendar.min = (temp % 3600) / 60;  /* ���� */
    calendar.sec = (temp % 3600) % 60;  /* ���� */
    calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date); /* ��ȡ���� */
}

/**
 * @brief       ��������ʱ����ת����������
 *   @note      ���빫�����ڵõ�����(��ʼʱ��Ϊ: ��Ԫ0��3��1�տ�ʼ, ����������κ�����, �����Ի�ȡ��ȷ������)
 *              ʹ�� ��ķ����ɭ���㹫ʽ ����, ԭ��˵��������:
 *              https://www.cnblogs.com/fengbohello/p/3264300.html
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @retval      0, ������; 1 ~ 6: ����һ ~ ������
 */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}

/**
 * @brief       ��������ʱ����ת����������
 *   @note      ��1970��1��1��Ϊ��׼, 1970��1��1��, 0ʱ0��0��, ��ʾ��0����
 *              ����ʾ��2105��, ��Ϊuint32_t����ʾ136���������(����������)!
 *              ������ο�ֻlinux mktime����, ԭ��˵��������:
 *              http://www.openedv.com/thread-63389-1-1.html
 * @param       syear : ���
 * @param       smon  : �·�
 * @param       sday  : ����
 * @param       hour  : Сʱ
 * @param       min   : ����
 * @param       sec   : ����
 * @retval      ת�����������
 */
static long rtc_date2sec(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t Y, M, D, X, T;
    signed char monx = smon;/* ���·�ת���ɴ����ŵ�ֵ, ����������� */

    if (0 >=  (monx -= 2))  /* 1..12 -> 11,12,1..10 */
    { 
        monx += 12;         /* Puts Feb last since it has leap day */
        syear -= 1;
    }
    
    Y = (syear - 1) * 365 + syear / 4 - syear / 100 + syear / 400;  /* ��ԪԪ��1�����ڵ������� */
    M = 367 * monx / 12 - 30 + 59;
    D = sday - 1;
    X = Y + M + D - 719162;                         /* ��ȥ��ԪԪ�굽1970������� */
    T = ((X * 24 + hour) * 60 + min) * 60 + sec;    /* �������� */
    return T;
}















