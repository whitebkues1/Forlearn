/**
 ****************************************************************************************************
 * @file        gsm.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-31
 * @brief       GSMģ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20221031
 * ��һ�η���
 * ����4G CAT1 GM196Hģ��, 2G GSMҲͨ��
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "./BSP/GSM/gsm.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./MALLOC/malloc.h"
#include "./TEXT/text.h"
#include "./BSP/USART3/usart3.h"
#include "./FATFS/source/ff.h"
#include "ucos_ii.h"


__gsmdev gsmdev;    /* gsm������ */


/**
 * @brief       gsm���������,�����յ���Ӧ��
 * @param       str     : �ڴ���Ӧ����
 * @retval      0, û�еõ��ڴ���Ӧ����
 *              ����, �ڴ�Ӧ������λ��(str��λ��)
 */
uint8_t *gsm_check_cmd(uint8_t *str)
{
    char *strx = 0;

    if (g_usart3_rx_sta & 0X8000) /* ���յ�һ�������� */
    {
        g_usart3_rx_buf[g_usart3_rx_sta & 0X7FFF] = 0;  /* ��ӽ����� */
        strx = strstr((const char *)g_usart3_rx_buf, (const char *)str);
    }

    return (uint8_t *)strx;
}

/**
 * @brief       ��gsm��������
 * @param       cmd     : ���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
 * @param       cmd     : �ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
 * @param       waittime: �ȴ�ʱ��(��λ:10ms)
 * @retval      0,���ͳɹ�(�õ����ڴ���Ӧ����)
 *              1,�յ���Ԥ�ڽ��
 *              2,û�յ��κλظ�
 */
uint8_t gsm_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t waittime)
{
    uint8_t res = 0;
    g_usart3_rx_sta = 0;
    gsmdev.cmdon = 1;   /* ����ָ��ȴ�״̬ */

    if ((uint32_t)cmd <= 0XFF)
    {
        while ((USART3->SR & 0X40) == 0);  /* �ȴ���һ�����ݷ������ */

        USART3->DR = (uint32_t)cmd;
    }
    else u3_printf("%s\r\n", cmd);  /* �������� */

    if (ack && waittime)            /* ��Ҫ�ȴ�Ӧ�� */
    {
        while (--waittime)          /* �ȴ�����ʱ */
        {
            delay_ms(10);

            if (g_usart3_rx_sta & 0X8000)     /* �Ƿ���յ��ڴ���Ӧ���� */
            {
                if (gsm_check_cmd(ack))res = 0; /* �յ��ڴ��Ľ���� */
                else res = 1; /* �����ڴ��Ľ�� */

                break;
            }
        }

        if (waittime == 0)res = 2;
    }

    return res;
}

/**
 * @brief       �������ʱ����,��gsm_send_cmd�ɶ�ʹ��/�����gsm_send_cmd�����.
 * @param       ��
 * @retval      ��
 */
void gsm_cmd_over(void)
{
    g_usart3_rx_sta = 0;
    gsmdev.cmdon = 0; /* �˳�ָ��ȴ�״̬ */
}

/**
 * @brief       ��1���ַ�ת��Ϊ16��������
 * @param       chr     : �ַ�,0~9/A~F/a~F
 * @retval      chr��Ӧ��16������ֵ
 */
uint8_t gsm_chr2hex(uint8_t chr)
{
    if (chr >= '0' && chr <= '9')return chr - '0';

    if (chr >= 'A' && chr <= 'F')return (chr - 'A' + 10);

    if (chr >= 'a' && chr <= 'f')return (chr - 'a' + 10);

    return 0;
}

/**
 * @brief       ��1��16��������ת��Ϊ�ַ�
 * @param       hex     : 16��������,0~15;
 * @retval      �ַ�
 */
uint8_t gsm_hex2chr(uint8_t hex)
{
    if (hex <= 9)return hex + '0';

    if (hex >= 10 && hex <= 15)return (hex - 10 + 'A');

    return '0';
}

/**
 * @brief       unicode gbk ת������
 * @param       src     : �����ַ���
 * @param       dst     : ���(uni2gbkʱΪgbk����,gbk2uniʱ,Ϊunicode�ַ���)
 * @param       mode    : 0,unicode��gbkת��;
 *                        1,gbk��unicodeת��;
 * @retval      ��
 */
void gsm_unigbk_exchange(uint8_t *src, uint8_t *dst, uint8_t mode)
{
    OS_CPU_SR cpu_sr = 0;
    uint16_t temp;
    uint8_t buf[2];
    OS_ENTER_CRITICAL();/* �����ٽ���(�޷����жϴ��) */

    if (mode) /* gbk 2 unicode */
    {
        while (*src != 0)
        {
            if (*src < 0X81)    /* �Ǻ��� */
            {
                temp = (uint16_t)ff_oem2uni((WCHAR) * src, 0);
                src++;
            }
            else    /* ����,ռ2���ֽ� */
            {
                buf[1] = *src++;
                buf[0] = *src++;
                temp = (uint16_t)ff_oem2uni((WCHAR) * (uint16_t *)buf, 0);
            }

            *dst++ = gsm_hex2chr((temp >> 12) & 0X0F);
            *dst++ = gsm_hex2chr((temp >> 8) & 0X0F);
            *dst++ = gsm_hex2chr((temp >> 4) & 0X0F);
            *dst++ = gsm_hex2chr(temp & 0X0F);
        }
    }
    else    /* unicode 2 gbk */
    {
        while (*src != 0)
        {
            buf[1] = gsm_chr2hex(*src++) * 16;
            buf[1] += gsm_chr2hex(*src++);
            buf[0] = gsm_chr2hex(*src++) * 16;
            buf[0] += gsm_chr2hex(*src++);
            temp = (uint16_t)ff_uni2oem((WCHAR) * (uint16_t *)buf, 0);

            if (temp < 0X80)
            {
                *dst = temp;
                dst++;
            }
            else
            {
                *(uint16_t *)dst = swap16(temp);
                dst += 2;
            }
        }
    }

    *dst = 0; /* ��ӽ����� */
    OS_EXIT_CRITICAL();	/* �˳��ٽ���(���Ա��жϴ��) */
}

extern void phone_incall_task_creat(void);

/**
 * @brief       �绰����/��⵽���� ���
 * @param       ��
 * @retval      ��
 */
void gsm_cmsgin_check(void)
{
    uint8_t *p1, *p2;

    if (gsmdev.cmdon == 0 && gsmdev.mode == 0) /* ��ָ��ȴ�״̬,.����/����ģʽ,�ż������ */
    {
        if (g_usart3_rx_sta & 0X8000) /* �յ������� */
        {
            if (gsm_check_cmd("+CLIP:")) /* ���յ�����? */
            {
                p1 = gsm_check_cmd("+CLIP:");
                p1 += 8;
                p2 = (uint8_t *)strstr((const char *)p1, "\"");
                p2[0] = 0; /* ��ӽ����� */
                strcpy((char *)gsmdev.incallnum, (char *)p1); /* �������� */
                gsmdev.mode = 3;            /* ��������� */
                phone_incall_task_creat();  /* ������������ */
            }
            
            g_usart3_rx_sta = 0;
            printf("rev:%s\r\n", g_usart3_rx_buf);
        }
    }
}

/**
 * @brief       gsm״̬���
 * @param       ��
 * @retval      ��
 */
void gsm_status_check(void)
{
    uint8_t *p1;

    if (gsmdev.cmdon == 0 && gsmdev.mode == 0 && g_usart3_rx_sta == 0) /* ��ָ��ȴ�״̬.����/����ģʽ/��û���յ��κ�����,�������ѯ */
    {
        if (gsm_send_cmd("AT+CSQ", "OK", 25) == 0) /* ��ѯ�ź�����,˳����GSMģ��״̬ */
        {
            p1 = (uint8_t *)strstr((const char *)(g_usart3_rx_buf), ":");
            p1 += 2;
            gsmdev.csq = (p1[0] - '0') * 10 + p1[1] - '0'; /* �ź����� */

            if (gsmdev.csq > 30)gsmdev.csq = 30;

            gsmdev.status |= 1 << 7;    /* ��ѯGSMģ���Ƿ���λ? */
        }
        else
        {
            gsmdev.csq = 0;
            gsmdev.status = 0;          /* ���²��� */
        }

        if ((gsmdev.status & 0XC0) == 0X80) /* CPIN״̬,δ��ȡ? */
        {
            gsm_send_cmd("ATE0", "OK", 100); /* ������(����ر�,����������ݿ����쳣) */

            if (gsm_send_cmd("AT+CPIN?", "OK", 25) == 0)gsmdev.status |= 1 << 6; /* SIM����λ */
            else gsmdev.status &= ~(1 << 6); /* SIM������ */
        }

        if ((gsmdev.status & 0XE0) == 0XC0) /* ��Ӫ������,δ��ȡ? */
        {
            if (gsm_send_cmd("AT+COPS?", "OK", 25) == 0) /* ��ѯ��Ӫ������ */
            {
                p1 = (uint8_t *)strstr((const char *)(g_usart3_rx_buf), "MOBILE"); /* ����MOBILE,�����ǲ����й��ƶ�? */

                if (p1)gsmdev.status &= ~(1 << 4); /* �й��ƶ� */
                else
                {
                    p1 = (uint8_t *)strstr((const char *)(g_usart3_rx_buf), "UNICOM"); /* ����UNICOM,�����ǲ����й���ͨ? */

                    if (p1)gsmdev.status |= 1 << 4;	/* �й���ͨ */
                }

                if (p1)
                {
                    gsmdev.status |= 1 << 5;    /* �õ���Ӫ�������� */
                    /* phone����ͨ������ */
                    gsm_send_cmd("AT+CLIP=1", "OK", 100);   /* ����������ʾ */
                    gsm_send_cmd("AT+COLP=1", "OK", 100);   /* ���ñ��к�����ʾ */
                }
            }
            else gsmdev.status &= ~(1 << 5);    /* δ�����Ӫ������ */
        }

        gsm_cmd_over();                         /* ������� */
    }
}























