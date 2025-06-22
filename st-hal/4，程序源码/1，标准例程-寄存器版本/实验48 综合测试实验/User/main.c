/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-26
 * @brief       �ۺϲ��� ʵ��
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

#include "./BSP/USART3/usart3.h"
#include "./PICTURE/gif.h"
#include "os.h"
#include "spb.h"
#include "common.h"
#include "ebook.h"
#include "picviewer.h"
#include "settings.h"
#include "calendar.h"
#include "beepplay.h"
#include "ledplay.h"
#include "keyplay.h"
#include "usbplay.h"
#include "nesplay.h"
#include "notepad.h"
#include "paint.h"
#include "exeplay.h"
#include "camera.h"
#include "wirelessplay.h"
#include "calculator.h"
#include "vmeterplay.h"
#include "phoneplay.h"
#include "appplay.h"
#include "netplay.h"

#if !(__ARMCC_VERSION >= 6010050)   /* ����AC6����������ʹ��AC5������ʱ */
#define __ALIGNED_8     __align(8)  /* AC5ʹ����� */
#else                               /* ʹ��AC6������ʱ */
#define __ALIGNED_8     __ALIGNED(8) /* AC6ʹ����� */
#endif

/******************************************************************************************/
/* UCOSII�������� */

/* START ���� ���� */
#define START_TASK_PRIO                 10                  /* ��ʼ��������ȼ�����Ϊ��� */
#define START_STK_SIZE                  64                  /* ��ջ��С */

__ALIGNED_8 static OS_STK START_TASK_STK[START_STK_SIZE];   /* �����ջ,8�ֽڶ��� */
void start_task(void *pdata);                               /* ������ */


/* ���� ���� ���� */
#define USART_TASK_PRIO                 7                   /* �������ȼ� */
#define USART_STK_SIZE                  128                 /* ��ջ��С */

__ALIGNED_8 static OS_STK USART_TASK_STK[USART_STK_SIZE];   /* �����ջ,8�ֽڶ��� */
void usart_task(void *pdata);                               /* ������ */


/* �� ���� ���� */
#define MAIN_TASK_PRIO                  6                   /* �������ȼ� */
#define MAIN_STK_SIZE                   512                 /* ��ջ��С */

__ALIGNED_8 static OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];     /* �����ջ,8�ֽڶ��� */
void main_task(void *pdata);                                /* ������ */


/* ���� ���� ���� */
#define WATCH_TASK_PRIO                 3                   /* �������ȼ� */
#define WATCH_STK_SIZE                  256                 /* ��ջ��С */

__ALIGNED_8 static OS_STK WATCH_TASK_STK[WATCH_STK_SIZE];   /* �����ջ,8�ֽڶ��� */
void watch_task(void *pdata);                               /* ������ */

/******************************************************************************************/

/**
 * @brief       ��ʾ������Ϣ,ֹͣ����,������ʾ������Ϣ
 * @param       x, y : ����
 * @param       err  : ������Ϣ
 * @param       fsize: �����С
 * @retval      ��
 */
void system_error_show(uint16_t x, uint16_t y, uint8_t *err, uint8_t fsize)
{
    uint8_t ledr = 1;

    while (1)
    {
        lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, (char *)err, RED);
        delay_ms(400);
        lcd_fill(x, y, lcddev.width - 1, y + fsize, BLACK);
        delay_ms(100);
        LED0(ledr ^= 1);
    }
}

/**
 * @brief       ��ʾ������Ϣ, ��ʾ�Ժ�(2��), ��������
 * @param       x, y : ����
 * @param       fsize: �����С
 * @param       str  : �ַ���
 * @retval      ��
 */
void system_error_show_pass(uint16_t x, uint16_t y, uint8_t fsize, uint8_t *str)
{
    BEEP(1);
    lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, (char *)str, RED);
    delay_ms(2000);
    BEEP(0);
}

/**
 * @brief       ��������SPI FLASH(��������Դ��ɾ��),�Կ��ٸ���ϵͳ.
 * @param       x, y : ����
 * @param       fsize: �����С
 * @retval      0,û�в���; 1,������;
 */
uint8_t system_files_erase(uint16_t x, uint16_t y, uint8_t fsize)
{
    uint8_t ledr = 1;
    uint8_t key;
    uint8_t t = 0;
    uint16_t i = 0;

    lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, "Erase all system files?", RED);

    while (1)
    {
        t++;

        if (t == 20)lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "KEY0:NO / KEY1:YES", RED);

        if (t == 40)
        {
            gui_fill_rectangle(x, y + fsize, lcddev.width, fsize, BLACK); /* �����ʾ */
            t = 0;
            LED0(ledr ^= 1);
        }

        key = key_scan(0);

        if (key == KEY0_PRES)   /* ������,�û�ȡ���� */
        {
            gui_fill_rectangle(x, y, lcddev.width, fsize * 2, BLACK); /* �����ʾ */
            g_point_color = WHITE;
            LED0(1);
            return 0;
        }

        if (key == KEY1_PRES)   /* Ҫ����,Ҫ�������� */
        {
            LED0(1);
            lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "Erasing SPI FLASH...", RED);

            for (i = 200; i < 4096; i++)
            {
                fonts_progress_show(x + fsize * 22 / 2, y + fsize, fsize, 3895, i - 200, RED);  /* ��ʾ�ٷֱ� */
                norflash_erase_sector(i);   /* ����һ������Լ50ms��3895�������Ҫ��195�� */
            }

            lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "Erasing SPI FLASH OK      ", RED);
            delay_ms(600);
            return 1;
        }

        delay_ms(10);
    }
}

/**
 * @brief       �ֿ����ȷ����ʾ.
 * @param       x, y : ����
 * @param       fsize: �����С
 * @retval      0,����Ҫ����; 1,ȷ��Ҫ����;
 */
uint8_t system_font_update_confirm(uint16_t x, uint16_t y, uint8_t fsize)
{
    uint8_t ledr = 1;
    uint8_t key;
    uint8_t t = 0;
    uint8_t res = 0;
    g_point_color = RED;
    lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, "Update font?", RED);

    while (1)
    {
        t++;

        if (t == 20)lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "KEY0:NO / KEY1:YES", RED);

        if (t == 40)
        {
            gui_fill_rectangle(x, y + fsize, lcddev.width, fsize, BLACK); /* �����ʾ */
            t = 0;
            LED0(ledr ^= 1);
        }

        key = key_scan(0);

        if (key == KEY0_PRES)break; /* ������ */

        if (key == KEY1_PRES)
        {
            res = 1;    /* Ҫ���� */
            break;
        }

        delay_ms(10);
    }

    LED0(1);
    gui_fill_rectangle(x, y, lcddev.width, fsize * 2, BLACK); /* �����ʾ */
    g_point_color = WHITE;
    return res;
}

uint8_t g_tpad_failed_flag = 0;     /* TPADʧЧ��ǣ����ʧЧ����ʹ��WK_UP��Ϊ�˳����� */

/**
 * @brief       ϵͳ��ʼ��
 * @param       ��
 * @retval      ��
 */
void system_init(void)
{
    uint16_t okoffset = 162;
    uint16_t ypos = 0;
    uint16_t j = 0;
    uint16_t temp = 0;
    uint8_t res;
    uint32_t dtsize, dfsize;
    uint8_t *stastr = 0;
    uint8_t *version = 0;
    uint8_t verbuf[12];
    uint8_t fsize;
    uint8_t icowidth;

    sys_stm32_clock_init(9);            /* ����ʱ��, 72Mhz */
    usart_init(72, 115200);             /* ���ڳ�ʼ��Ϊ115200 */
    
    norflash_init();                    /* ��ʼ��SPI FLASH */
    exeplay_app_check();                /* ����Ƿ���Ҫ����APP���� */

    delay_init(72);                     /* ��ʱ��ʼ�� */
    usmart_dev.init(72);                /* ��ʼ��USMART */
    usart3_init(36, 115200);            /* ��ʼ������3������Ϊ115200 */
    lcd_init();                         /* ��ʼ��LCD */
    led_init();                         /* ��ʼ��LED */
    key_init();                         /* ��ʼ������ */ 
    beep_init();                        /* ��������ʼ�� */
    at24cxx_init();                     /* EEPROM��ʼ�� */
    adc_temperature_init();             /* ��ʼ���ڲ��¶ȴ����� */
    lsens_init();                       /* ��ʼ������������ */

    my_mem_init(SRAMIN);                /* ��ʼ���ڲ�SRAM�ڴ�� */

    tp_dev.init();
    gui_init();
    piclib_init();                      /* piclib��ʼ�� */
    slcd_dma_init();                    /* SLCD DMA��ʼ�� */
    exfuns_init();                      /* FATFS �����ڴ� */

    version = mymalloc(SRAMIN, 31);     /* ����31���ֽ��ڴ� */

REINIT: /* ���³�ʼ�� */

    lcd_clear(BLACK);                   /* ���� */
    g_point_color = WHITE;
    g_back_color = BLACK;
    j = 0;

    /* ��ʾ��Ȩ��Ϣ */
    ypos = 2;

    if (lcddev.width <= 272)
    {
        fsize = 12;
        icowidth = 24;
        okoffset = 190;
        app_show_mono_icos(5, ypos, icowidth, 24, (uint8_t *)APP_ALIENTEK_ICO2424, YELLOW, BLACK);
    }
    else if (lcddev.width == 320)
    {
        fsize = 16;
        icowidth = 32;
        okoffset = 250;
        app_show_mono_icos(5, ypos, icowidth, 32, (uint8_t *)APP_ALIENTEK_ICO3232, YELLOW, BLACK);
    }
    else if (lcddev.width >= 480)
    {
        fsize = 24;
        icowidth = 48;
        okoffset = 370;
        app_show_mono_icos(5, ypos, icowidth, 48, (uint8_t *)APP_ALIENTEK_ICO4848, YELLOW, BLACK);
    }

    lcd_show_string(icowidth + 5 * 2, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "ALIENTEK STM32F103", g_point_color);
    lcd_show_string(icowidth + 5 * 2, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "Copyright (C) 2022-2032", g_point_color);
    app_get_version(verbuf, HARDWARE_VERSION, 2);
    strcpy((char *)version, "HARDWARE:");
    strcat((char *)version, (const char *)verbuf);
    strcat((char *)version, ", SOFTWARE:");
    app_get_version(verbuf, SOFTWARE_VERSION, 3);
    strcat((char *)version, (const char *)verbuf);
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, (char *)version, g_point_color);
    sprintf((char *)verbuf, "LCD ID:%04X", lcddev.id);  /* LCD ID��ӡ��verbuf���� */
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, (char *)verbuf, g_point_color);  /* ��ʾLCD ID */

    /* ��ʼӲ������ʼ�� */
    LED0(0);
    LED1(0);    /* ͬʱ����2��LED */
    
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "CPU:STM32F103ZET6 72Mhz", g_point_color);
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "FLASH:512KB + 16MB SRAM:64KB", g_point_color);
    LED0(1);
    LED1(1);    /* ͬʱ�ر�2��LED */

    /* SPI FLASH��� */
    if (norflash_read_id() != BY25Q128 && norflash_read_id() != W25Q128 && norflash_read_id() != NM25Q128)  /* ��ȡQSPI FLASH ID */
    {
        system_error_show(5, ypos + fsize * j++, "SPI Flash Error!!", fsize);
    }
    else temp = 16 * 1024;  /* 16M�ֽڴ�С */

    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SPI Flash:     KB", g_point_color);
    lcd_show_xnum(5 + 10 * (fsize / 2), ypos + fsize * j, temp, 5, fsize, 0, g_point_color); /* ��ʾspi flash��С */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* ����Ƿ���Ҫ����SPI FLASH? */
    res = key_scan(1);

    if (res == WKUP_PRES)   /* ������ʱ�򣬰���WKUP�����������SPI FLASH�ֿ���ļ�ϵͳ���� */
    {
        res = system_files_erase(5, ypos + fsize * j, fsize);

        if (res)goto REINIT;
    }

    /* RTC��� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "RTC Check...", g_point_color);

    if (rtc_init())system_error_show_pass(5 + okoffset, ypos + fsize * j++, fsize, "ERROR"); /* RTC��� */
    else
    {
        calendar_get_time(&calendar);/* �õ���ǰʱ�� */
        calendar_get_date(&calendar);/* �õ���ǰ���� */
        lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);
    }

    /* ���SPI FLASH���ļ�ϵͳ */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "FATFS Check...", g_point_color); /* FATFS��� */
    f_mount(fs[0], "0:", 1);    /* ����SD�� */
    f_mount(fs[1], "1:", 1);    /* ����SPI FLASH */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* SD����� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SD Card:     MB", g_point_color); /* FATFS��� */
    temp = 0;

    do
    {
        temp++;
        res = exfuns_get_free("0:", &dtsize, &dfsize);  /* �õ�SD��ʣ�������������� */
        delay_ms(200);
    } while (res && temp < 5); /* �������5�� */

    if (res == 0)   /* �õ��������� */
    {
        gui_phy.memdevflag |= 1 << 0;	/* ����SD����λ */
        temp = dtsize >> 10; /* ��λת��ΪMB */
        stastr = "OK";
    }
    else
    {
        temp = 0; /* ������,��λΪ0 */
        stastr = "ERROR";
    }

    lcd_show_xnum(5 + 8 * (fsize / 2), ypos + fsize * j, temp, 5, fsize, 0, g_point_color);	/* ��ʾSD��������С */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, (char *)stastr, g_point_color);   /* SD��״̬ */

    /* 25Q128���,����������ļ�ϵͳ,���ȴ��� */
    temp = 0;

    do
    {
        temp++;
        res = exfuns_get_free("1:", &dtsize, &dfsize); /* �õ�FLASHʣ�������������� */
        delay_ms(200);
    } while (res && temp < 20); /* �������20�� */

    if (res == 0X0D)   /* �ļ�ϵͳ������ */
    {
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SPI Flash Disk Formatting...", g_point_color); /* ��ʽ��FLASH */
        res = f_mkfs("1:", 0, 0, FF_MAX_SS);    /* ��ʽ��SPI FLASH,1:,�̷�;0,�Զ�ѡ���ļ�ϵͳ���� */

        if (res == 0)
        {
            f_setlabel((const TCHAR *)"1:ALIENTEK");        /* ����Flash���̵�����Ϊ��ALIENTEK */
            lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color); /* ��־��ʽ���ɹ� */
            res = exfuns_get_free("1:", &dtsize, &dfsize);  /* ���»�ȡ���� */
        }
    }

    if (res == 0)   /* �õ�FLASH��ʣ�������������� */
    {
        gui_phy.memdevflag |= 1 << 1;   /* ����SPI FLASH��λ */
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SPI Flash Disk:     KB", g_point_color); /* FATFS��� */
        temp = dtsize;
    }
    else system_error_show(5, ypos + fsize * (j + 1), "Flash Fat Error!", fsize);   /* flash �ļ�ϵͳ���� */

    lcd_show_xnum(5 + 15 * (fsize / 2), ypos + fsize * j, temp, 5, fsize, 0, g_point_color);    /* ��ʾFLASH������С */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);	/* FLASH��״̬ */

    /* TPAD��� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "TPAD Check...", g_point_color);

    if (tpad_init(6))system_error_show(5, ypos + fsize * (j + 1), "TPAD Error!", fsize); /* ����������� */
    else lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* 24C02��� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "24C02 Check...", g_point_color);

    if (at24cxx_check())system_error_show(5, ypos + fsize * (j + 1), "24C02 Error!", fsize);    /* 24C02��� */
    else lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* �ֿ��� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "Font Check...", g_point_color);
    res = key_scan(1); /* ��ⰴ�� */

    if (res == KEY1_PRES)   /* KEY1���£����£�ȷ�� */
    {
        res = system_font_update_confirm(5, ypos + fsize * (j + 1), fsize);
    }
    else res = 0;

    if (fonts_init() || (res == 1))   /* �������,������岻����/ǿ�Ƹ���,������ֿ� */
    {
        res = 0; /* ������Ч */

        if (fonts_update_font(5, ypos + fsize * j, fsize, "0:", g_point_color) != 0)        /* ��SD������ */
        {

            if (fonts_update_font(5, ypos + fsize * j, fsize, "1:", g_point_color) != 0)    /* ��SPI FLASH���� */
            {
                system_error_show(5, ypos + fsize * (j + 1), "Font Error!", fsize);         /* ������� */
            }

        }

        lcd_fill(5, ypos + fsize * j, lcddev.width - 1, ypos + fsize * (j + 1), BLACK);     /* ����ɫ */
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "Font Check...", g_point_color);
    }

    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color); /* �ֿ���OK */

    /* ϵͳ�ļ���� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Files Check...", g_point_color);

    while (app_system_file_check("1"))      /* ϵͳ�ļ���� */
    {
        lcd_fill(5, ypos + fsize * j, lcddev.width - 1, ypos + fsize * (j + 1), BLACK); /* ����ɫ */
        lcd_show_string(5, ypos + fsize * j, (fsize / 2) * 8, fsize, fsize, "Updating", g_point_color); /* ��ʾupdating */
        app_boot_cpdmsg_set(5, ypos + fsize * j, fsize);    /* ���õ����� */
        temp = 0;

        if (app_system_file_check("0"))     /* ���SD��ϵͳ�ļ������� */
        {
            if (app_system_file_check("2"))res = 9; /* ���Ϊ�����õ��� */
            else res = 2;                   /* ���ΪU�� */
        }
        else res = 0;                       /* ���ΪSD�� */

        if (res == 0 || res == 2)           /* �����˲Ÿ��� */
        {
            sprintf((char *)verbuf, "%d:", res);

            if (app_system_update(app_boot_cpdmsg, verbuf))   /* ����? */
            {
                system_error_show(5, ypos + fsize * (j + 1), "SYSTEM File Error!", fsize);
            }
        }

        lcd_fill(5, ypos + fsize * j, lcddev.width - 1, ypos + fsize * (j + 1), BLACK); /* ����ɫ */
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Files Check...", g_point_color);

        if (app_system_file_check("1"))     /* ������һ�Σ��ټ�⣬������в�ȫ��˵��SD���ļ��Ͳ�ȫ�� */
        {
            system_error_show(5, ypos + fsize * (j + 1), "SYSTEM File Lost!", fsize);
        }
        else break;
    }

    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* ��������� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "Touch Check...", g_point_color);
    res = key_scan(1); /* ��ⰴ�� */

    if (tp_init() || (res == KEY0_PRES && (tp_dev.touchtype & 0X80) == 0))   /* �и���/������KEY0�Ҳ��ǵ�����,ִ��У׼ */
    {
        if (res == 1)tp_adjust();

        res = 0;        /* ������Ч */
        goto REINIT;    /* ���¿�ʼ��ʼ�� */
    }

    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color); /* ���������OK */

    /* ϵͳ�������� */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Parameter Load...", g_point_color);

    if (app_system_parameter_init())system_error_show(5, ypos + fsize * (j + 1), "Parameter Load Error!", fsize); /* �������� */
    else lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Starting...", g_point_color);

    /* �������̽�,��ʾ�������� */
    BEEP(1);
    delay_ms(100);
    BEEP(0);
    myfree(SRAMIN, version);
    delay_ms(1500);
}

int main(void)
{
    system_init();  /* ϵͳ��ʼ�� */
    OSInit();
    OSTaskCreateExt((void(*)(void *) )start_task,               /* ������ */
                    (void *          )0,                        /* ���ݸ��������Ĳ��� */
                    (OS_STK *        )&START_TASK_STK[START_STK_SIZE - 1], /* �����ջջ�� */
                    (INT8U          )START_TASK_PRIO,           /* �������ȼ� */
                    (INT16U         )START_TASK_PRIO,           /* ����ID����������Ϊ�����ȼ�һ�� */
                    (OS_STK *        )&START_TASK_STK[0],       /* �����ջջ�� */
                    (INT32U         )START_STK_SIZE,            /* �����ջ��С */
                    (void *          )0,                        /* �û�����Ĵ洢�� */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* ����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ */
    OSStart();
}

/**
 * @brief       ��ʼ����
 * @param       pdata : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pdata)
{
    uint32_t cnts;
    OS_CPU_SR cpu_sr = 0;
    pdata = pdata;
    
    
    /* �������õĽ���Ƶ������SysTick */
    cnts = (CPU_INT32U)((168 * 1000000) / OS_TICKS_PER_SEC);
    OS_CPU_SysTickInit(cnts);
    
    OSStatInit();                           /* ��ʼ��ͳ������.�������ʱ1�������� */
    app_srand(OSTime);

    OS_ENTER_CRITICAL();                    /* �����ٽ���(�޷����жϴ��) */
    OSTaskCreateExt((void(*)(void *) )main_task,                /* ������ */
                    (void *          )0,                        /* ���ݸ��������Ĳ��� */
                    (OS_STK *        )&MAIN_TASK_STK[MAIN_STK_SIZE - 1], /* �����ջջ�� */
                    (INT8U          )MAIN_TASK_PRIO,            /* �������ȼ� */
                    (INT16U         )MAIN_TASK_PRIO,            /* ����ID����������Ϊ�����ȼ�һ�� */
                    (OS_STK *        )&MAIN_TASK_STK[0],        /* �����ջջ�� */
                    (INT32U         )MAIN_STK_SIZE,             /* �����ջ��С */
                    (void *          )0,                        /* �û�����Ĵ洢�� */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* ����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ */

    OSTaskCreateExt((void(*)(void *) )usart_task,              	/* ������ */
                    (void *          )0,                        /* ���ݸ��������Ĳ��� */
                    (OS_STK *        )&USART_TASK_STK[USART_STK_SIZE - 1], /* �����ջջ�� */
                    (INT8U          )USART_TASK_PRIO,           /* �������ȼ� */
                    (INT16U         )USART_TASK_PRIO,           /* ����ID����������Ϊ�����ȼ�һ�� */
                    (OS_STK *        )&USART_TASK_STK[0],       /* �����ջջ�� */
                    (INT32U         )USART_STK_SIZE,            /* �����ջ��С */
                    (void *          )0,                        /* �û�����Ĵ洢�� */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* ����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ */

    OSTaskCreateExt((void(*)(void *) )watch_task,               /* ������ */
                    (void *          )0,                        /* ���ݸ��������Ĳ��� */
                    (OS_STK *        )&WATCH_TASK_STK[WATCH_STK_SIZE - 1], /* �����ջջ�� */
                    (INT8U          )WATCH_TASK_PRIO,           /* �������ȼ� */
                    (INT16U         )WATCH_TASK_PRIO,           /* ����ID����������Ϊ�����ȼ�һ�� */
                    (OS_STK *        )&WATCH_TASK_STK[0],       /* �����ջջ�� */
                    (INT32U         )WATCH_STK_SIZE,            /* �����ջ��С */
                    (void *          )0,                        /* �û�����Ĵ洢�� */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* ����ѡ��,Ϊ�˱���������������񶼱��渡��Ĵ�����ֵ */

    OSTaskSuspend(START_TASK_PRIO);     /* ������ʼ���� */
    OS_EXIT_CRITICAL();                 /* �˳��ٽ���(���Ա��жϴ��) */
}

/**
 * @brief       ������
 * @param       pdata : �������(δ�õ�)
 * @retval      ��
 */
void main_task(void *pdata)
{
    uint8_t selx;
    uint16_t tcnt = 0;
    
    spb_init(0);        /* ��ʼ��SPB���� */
    spb_load_mui();     /* ����SPB������ */
    slcd_frame_show(spbdev.spbahwidth); /* ��ʾ���� */

    while (1)
    {

        selx = spb_move_chk();
        system_task_return = 0;         /* ���˳���־ */

        switch (selx)   /* ������˫���¼� */
        {
            case 0:
                ebook_play();
                break;  /* ����ͼ�� */

            case 1:
                picviewer_play();
                break;  /* ������� */

            case 2:
                calendar_play();
                break;  /* ʱ�� */

            case 3:
                sysset_play();
                break;  /* ϵͳ���� */

            case 4:
                notepad_play();
                break;  /* ���±� */

            case 5:
                exe_play();
                break;  /* ������ */

            case 6:
                paint_play();
                break;  /* ��д���� */

            case 7:
                camera_play();
                break;  /* ����ͷ */

            case 8:
                usb_play();
                break;  /* USB���� */

            case 9:
                net_play();
                break;  /* ����ͨ�� */

            case 10:
                wireless_play();
                break;  /* ����ͨ�� */

            case 11:
                calc_play();
                break;  /* ������ */

            case 12:
                key_play((uint8_t *)APP_MFUNS_CAPTION_TBL[selx][gui_phy.language]);
                break;  /* �������� */

            case 13:
                led_play((uint8_t *)APP_MFUNS_CAPTION_TBL[selx][gui_phy.language]);
                break;  /* LED���� */

            case 14:
                beep_play((uint8_t *)APP_MFUNS_CAPTION_TBL[selx][gui_phy.language]);
                break;  /* ������ͨ�� */

            case SPB_ICOS_NUM:
                phone_play();
                break;  /* �绰���� */

            case SPB_ICOS_NUM+1:
                app_play();
                break;  /* APP */

            case SPB_ICOS_NUM+2:
                vmeter_play();
                break;  /* ��ѹ�� */
        }

        if (selx != 0XFF)spb_load_mui();    /* ��ʾ������ */

        delay_ms(1000 / OS_TICKS_PER_SEC);  /* ��ʱһ��ʱ�ӽ��� */
        tcnt++;

        if (tcnt == OS_TICKS_PER_SEC)       /* OS_TICKS_PER_SEC������Ϊ1���� */
        {
            tcnt = 0;
            spb_stabar_msg_show(0);/* ����״̬����Ϣ */
        }
    }
}

volatile uint8_t memshow_flag = 1;  /* Ĭ�ϴ�ӡmemʹ���� */

/**
 * @brief       ���� ����,ִ�����ҪʱЧ�ԵĴ���
 * @param       pdata : �������(δ�õ�)
 * @retval      ��
 */
void usart_task(void *pdata)
{
    uint16_t alarmtimse = 0;
    float psin, psex;
    pdata = pdata;

    while (1)
    {
        delay_ms(1000);

        if (alarm.ringsta & 1 << 7)   /* ִ������ɨ�躯�� */
        {
            calendar_alarm_ring(alarm.ringsta & 0x3); /* ���� */
            alarmtimse++;

            if (alarmtimse > 300)   /* ����300����,5�������� */
            {
                alarm.ringsta &= ~(1 << 7); /* �ر����� */
            }
        }
        else if (alarmtimse)
        {
            alarmtimse = 0;
            BEEP(0);    /* �رշ����� */
        }

        if (gsmdev.mode == 3)           /* ������,�������� */
        {
            phone_ring(); 
        }
        
        if (systemset.lcdbklight == 0)  /* �Զ�������� */
        {
            app_lcd_auto_bklight(); 
        }
        
        if (memshow_flag == 1)
        {
            psin = my_mem_perused(SRAMIN);
            //psex = my_mem_perused(SRAMEX);
            printf("in:%3.1f,ex:%3.1f\r\n", psin / 10, psex / 10);  /* ��ӡ�ڴ�ռ���� */
        }
    }
}

volatile uint8_t system_task_return;    /* ����ǿ�Ʒ��ر�־. */
volatile uint8_t ledplay_ds0_sta = 0;   /* ledplay����,DS0�Ŀ���״̬ */
extern uint8_t nes_run_flag;

/**
 * @brief       ���� ����
 * @param       pdata : �������(δ�õ�)
 * @retval      ��
 */
void watch_task(void *pdata)
{
    OS_CPU_SR cpu_sr = 0;
    uint8_t t = 0;
    uint8_t rerreturn = 0;
    uint8_t res;
    uint8_t key;
    pdata = pdata;

    while (1)
    {
        if (alarm.ringsta & 1 << 7)   /* ������ִ�� */
        {
            calendar_alarm_msg((lcddev.width - 44) / 2, (lcddev.height - 20) / 2); /* ���Ӵ��� */
        }

        if (g_gif_decoding)   /* gif���ڽ����� */
        {
            key = pic_tp_scan();

            if (key == 1 || key == 3)g_gif_decoding = 0; /* ֹͣGIF���� */
        }

        if (ledplay_ds0_sta == 0)   /* ����ledplay_ds0_sta����0��ʱ��,����Ϩ��LED0 */
        {
            if (t == 4)LED0(1);     /* ��100ms���� */

            if (t == 119)
            {
                LED0(0);            /* 2.5����������һ�� */
                t = 0;
            }
        }

        t++;
        
        if (rerreturn)              /* �ٴο�ʼTPADɨ��ʱ���һ */
        {
            rerreturn--;
            delay_ms(15);           /* ������ʱ�� */
        }
        else if (tpad_scan(0))      /* TPAD������һ��,�˺���ִ��,������Ҫ15ms */
        {
            rerreturn = 10;         /* �´α���100ms�Ժ�����ٴν��� */
            system_task_return = 1;

            if (g_gif_decoding)g_gif_decoding = 0;  /* ���ٲ���gif */
        }
        

        if ((t % 60) == 0)   /* 900ms���Ҽ��1�� */
        {
            /* SD����λ��� */
            OS_ENTER_CRITICAL();        /* �����ٽ���(�޷����жϴ��) */
            res = sdmmc_get_status();   /* ��ѯSD��״̬ */
            OS_EXIT_CRITICAL();         /* �˳��ٽ���(���Ա��жϴ��) */

            if (res == 0XFF)
            {
                gui_phy.memdevflag &= ~(1 << 0);    /* ���SD������λ */
                g_sd_card_info.CardCapacity = 0;    /* SD���������� */
                
                OS_ENTER_CRITICAL();/* �����ٽ���(�޷����жϴ��) */
                sd_init();          /* ���¼��SD�� */
                OS_EXIT_CRITICAL(); /* �˳��ٽ���(���Ա��жϴ��) */
            }
            else if ((gui_phy.memdevflag & (1 << 0)) == 0)     /* SD����λ? */
            {
                f_mount(fs[0], "0:", 1);        /* ���¹���sd�� */
                gui_phy.memdevflag |= 1 << 0;   /* ���SD����λ�� */
            }
        
            /* gsm��� */
            gsm_status_check();         /* gsm���! */
        }

        gsm_cmsgin_check();             /* ����/���� ��� */

        delay_ms(10);
    }
}

/**
 * @brief       Ӳ��������
 * @param       ��
 * @retval      ��
 */
void HardFault_Handler(void)
{
    uint8_t led1sta = 1;
    uint32_t i;
    uint8_t t = 0;
    uint32_t temp;
    temp = SCB->CFSR;               /* fault״̬�Ĵ���(@0XE000ED28)����:MMSR,BFSR,UFSR */
    printf("CFSR:%8X\r\n", temp);   /* ��ʾ����ֵ */
    temp = SCB->HFSR;               /* Ӳ��fault״̬�Ĵ��� */
    printf("HFSR:%8X\r\n", temp);   /* ��ʾ����ֵ */
    temp = SCB->DFSR;               /* ����fault״̬�Ĵ��� */
    printf("DFSR:%8X\r\n", temp);   /* ��ʾ����ֵ */
    temp = SCB->AFSR;               /* ����fault״̬�Ĵ��� */
    printf("AFSR:%8X\r\n", temp);   /* ��ʾ����ֵ */

    while (t < 5)
    {
        t++;
        LED1(led1sta ^= 1);

        for (i = 0; i < 0X1FFFFF; i++);
    }
}















