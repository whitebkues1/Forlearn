/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       SD�� ʵ��
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
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./MALLOC/malloc.h"
#include "./BSP/SDIO/sdio_sdcard.h"


/**
 * @brief       ͨ�����ڴ�ӡSD�������Ϣ
 * @param       ��
 * @retval      ��
 */
void show_sdcard_info(void)
{
    switch (g_sd_card_info.CardType)
    {
        case STD_CAPACITY_SD_CARD_V1_1:
            printf("Card Type:SDSC V1.1\r\n");
            break;

        case STD_CAPACITY_SD_CARD_V2_0:
            printf("Card Type:SDSC V2.0\r\n");
            break;

        case HIGH_CAPACITY_SD_CARD:
            printf("Card Type:SDHC V2.0\r\n");
            break;

        case MULTIMEDIA_CARD:
            printf("Card Type:MMC Card\r\n");
            break;
    }

    printf("Card ManufacturerID:%d\r\n", g_sd_card_info.SD_cid.ManufacturerID);         /* ������ID */
    printf("Card RCA:%d\r\n", g_sd_card_info.RCA);                                      /* ����Ե�ַ */
    printf("Card Capacity:%d MB\r\n", (uint32_t)(g_sd_card_info.CardCapacity >> 20));   /* ��ʾ���� */
    printf("Card BlockSize:%d\r\n\r\n", g_sd_card_info.CardBlockSize);                  /* ��ʾ���С */
}

/**
 * @brief       ����SD���Ķ�ȡ
 *   @note      ��secaddr��ַ��ʼ,��ȡseccnt������������
 * @param       secaddr : ������ַ
 * @param       seccnt  : ������
 * @retval      ��
 */
void sd_test_read(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t *buf;
    uint8_t sta = 0;
    buf = mymalloc(SRAMIN, seccnt * 512);       /* �����ڴ�,��SRAM�����ڴ� */
    sta = sd_read_disk(buf, secaddr, seccnt);   /* ��ȡsecaddr������ʼ������ */

    if (sta == 0)
    {
        lcd_show_string(30, 170, 200, 16, 16, "USART1 Sending Data...", BLUE);
        printf("SECTOR %d DATA:\r\n", secaddr);

        for (i = 0; i < seccnt * 512; i++)
        {
            printf("%x ", buf[i]);  /* ��ӡsecaddr��ʼ���������� */
        }
        
        printf("\r\nDATA ENDED\r\n");
        lcd_show_string(30, 170, 200, 16, 16, "USART1 Send Data Over!", BLUE);
    }
    else
    {
        printf("err:%d\r\n", sta);
        lcd_show_string(30, 170, 200, 16, 16, "SD read Failure!      ", BLUE);
    }

    myfree(SRAMIN, buf);    /* �ͷ��ڴ� */
}

/**
 * @brief       ����SD����д��
 *   @note      ��secaddr��ַ��ʼ,д��seccnt������������
 *              ����!! ���дȫ��0XFF������,���������SD��.
 *
 * @param       secaddr : ������ַ
 * @param       seccnt  : ������
 * @retval      ��
 */
void sd_test_write(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t *buf;
    uint8_t sta = 0;
    buf = mymalloc(SRAMIN, seccnt * 512);       /* ��SRAM�����ڴ� */

    for (i = 0; i < seccnt * 512; i++)          /* ��ʼ��д�������,��3�ı���. */
    {
        buf[i] = i * 3;
    }

    sta = sd_write_disk(buf, secaddr, seccnt);  /* ��secaddr������ʼд��seccnt���������� */

    if (sta == 0)
    {
        printf("Write over!\r\n");
    }
    else printf("err:%d\r\n", sta);

    myfree(SRAMIN, buf);    /* �ͷ��ڴ� */
}

int main(void)
{
    uint8_t key;
    uint8_t t = 0;

    sys_stm32_clock_init(9);    /* ����ʱ��, 72Mhz */
    delay_init(72);             /* ��ʱ��ʼ�� */
    usart_init(72, 115200);     /* ���ڳ�ʼ��Ϊ115200 */
    usmart_dev.init(72);        /* ��ʼ��USMART */
    led_init();                 /* ��ʼ��LED */
    lcd_init();                 /* ��ʼ��LCD */
    key_init();                 /* ��ʼ������ */
    my_mem_init(SRAMIN);        /* ��ʼ���ڲ�SRAM�ڴ�� */

    lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30,  70, 200, 16, 16, "SD TEST", RED);
    lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Read Sector 0", RED);

    while (sd_init())    /* ��ⲻ��SD�� */
    {
        lcd_show_string(30, 130, 200, 16, 16, "SD Card Error!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        delay_ms(500);
        LED0_TOGGLE();  /* �����˸ */
    }
        
    /* ��ӡSD�������Ϣ */
    show_sdcard_info(); 
    
    /* ���SD���ɹ� */
    lcd_show_string(30, 130, 200, 16, 16, "SD Card OK    ", BLUE);
    lcd_show_string(30, 150, 200, 16, 16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 150, g_sd_card_info.CardCapacity >> 20, 5, 16, BLUE); /* ��ʾSD������ */

    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)       /* KEY0������ */
        {
            sd_test_read(0,1);  /* ��0������ȡ1*512�ֽڵ����� */
        }

        t++;
        delay_ms(10);

        if (t == 20)
        {
            LED0_TOGGLE();  /* �����˸ */
            t = 0;
        }
    }
}


































