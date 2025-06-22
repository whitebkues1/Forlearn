/**
 ****************************************************************************************************
 * @file        calendar.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-���� ����
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
 * V1.1 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "calendar.h"
#include "stdio.h"
#include "settings.h"
#include "math.h"
#include "camera.h"
#include "./BSP/DS18B20/ds18b20.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/BEEP/beep.h"


_alarm_obj alarm;               /* ���ӽṹ�� */
_calendar_obj calendar;         /* �����ṹ�� */

static uint16_t TIME_TOPY;      /* 	120 */
static uint16_t OTHER_TOPY;     /* 	200 */

/* ���� */
uint8_t *const calendar_week_table[GUI_LANGUAGE_NUM][7] =
{
    {"������", "����һ", "���ڶ�", "������", "������", "������", "������"},
    {"������", "����һ", "���ڶ�", "������", "������", "������", "������"},
    {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
};

/* ���ӱ��� */
uint8_t *const calendar_alarm_caption_table[GUI_LANGUAGE_NUM] =
{
    "����", "�[�", "ALARM",
};

/* ���찴ť */
uint8_t *const calendar_alarm_realarm_table[GUI_LANGUAGE_NUM] =
{
    "����", "���", "REALARM",
};

/* ��ʾ��Ϣ */
uint8_t *const calendar_loading_str[GUI_LANGUAGE_NUM][3] =
{
    {
        "���ڼ���,���Ժ�...",
        "δ��⵽DS18B20!",
        "�����ڲ��¶ȴ�����...",
    },
    {
        "���ڼ���,���Ժ�...",
        "δ��⵽DS18B20!",
        "���ÃȲ��¶ȴ�����...",
    },
    {
        "Loading...",
        "DS18B20 Check Failed!",
        "Use STM32H7 Temp Sensor...",
    },
};

/**
 * @brief       ���³�ʼ������
 * @param       alarmx          : ���ӽṹ��
 * @param       calendarx       : �����ṹ��
 * @retval      ��
 */
void calendar_alarm_init(_alarm_obj *alarmx, _calendar_obj *calendarx)
{
    uint8_t temp;
    calendar_get_date(calendarx);       /* ��ȡ��ǰ������Ϣ */

    if (calendarx->week == 7)temp = 1 << 0;
    else temp = 1 << calendarx->week;

    if (alarmx->weekmask & temp)        /* ��Ҫ���� */
    {
        rtc_set_alarm(calendarx->year, calendarx->month, calendarx->date, alarmx->hour, alarmx->min, 0);    /* ��������ʱ�� */
    }
}

/**
 * @brief       ����������
 * @param       type            : ��������
 *                                0,��.
 *                                1,��.��.
 *                                2,��.��.��
 *                                3,��.��.��.��
 * @retval      ��
 */
void calendar_alarm_ring(uint8_t type)
{
    uint8_t i;

    for (i = 0; i < (type + 1); i++)
    {
        BEEP(1);
        delay_ms(50);
        BEEP(0);
        delay_ms(70);
    }
}

/**
 * @brief       �õ�ʱ��
 * @param       calendarx       : �����ṹ��
 * @retval      ��
 */
void calendar_get_time(_calendar_obj *calendarx)
{
    rtc_get_time(); /* �õ�ʱ��,�ڸú�����,���calendarȫ�ֽṹ�������Ϣ */
}

/**
 * @brief       �õ�����
 * @param       calendarx       : �����ṹ��
 * @retval      ��
 */
void calendar_get_date(_calendar_obj *calendarx)
{
    rtc_get_time(); /* �õ�ʱ��,�ڸú�����,���calendarȫ�ֽṹ�������Ϣ */
}

/**
 * @brief       ���ݵ�ǰ������,����������
 * @param       ��
 * @retval      ��
 */
void calendar_date_refresh(void)
{
    uint8_t weekn;   /* �ܼĴ� */
    uint16_t offx = (lcddev.width - 240) / 2;

    /* ��ʾ���������� */
    g_back_color = BLACK;

    lcd_show_xnum(offx + 5, OTHER_TOPY + 9, (calendar.year / 100) % 100, 2, 16, 0, 0XF81F);     /* ��ʾ��  20/19 */
    lcd_show_xnum(offx + 21, OTHER_TOPY + 9, calendar.year % 100, 2, 16, 0, 0XF81F);            /* ��ʾ�� */
    lcd_show_string(offx + 37, OTHER_TOPY + 9, lcddev.width, lcddev.height, 16, "-", 0XF81F);   /* "-" */
    lcd_show_xnum(offx + 45, OTHER_TOPY + 9, calendar.month, 2, 16, 0X80, 0XF81F);              /* ��ʾ�� */
    lcd_show_string(offx + 61, OTHER_TOPY + 9, lcddev.width, lcddev.height, 16, "-", 0XF81F);   /* "-" */
    lcd_show_xnum(offx + 69, OTHER_TOPY + 9, calendar.date, 2, 16, 0X80, 0XF81F);               /* ��ʾ�� */

    /* ��ʾ�ܼ�? */
    weekn = calendar.week;
    text_show_string(5 + offx, OTHER_TOPY + 35, lcddev.width, lcddev.height, (char *)calendar_week_table[gui_phy.language][weekn], 16, 0, RED); /* ��ʾ�ܼ�? */
}

/**
 * @brief       ��ȡ����������Ϣ
 *  @note       �������ݱ�����:SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings)+sizeof(_es8388_obj)
 * @param       alarm           : ������Ϣ
 * @retval      ��
 */
void calendar_read_para(_alarm_obj *alarm)
{
    at24cxx_read(SYSTEM_PARA_SAVE_BASE + sizeof(_system_setings), (uint8_t *)alarm, sizeof(_alarm_obj));
}

/**
 * @brief       д������������Ϣ
 * @param       alarm           : ������Ϣ
 * @retval      ��
 */
void calendar_save_para(_alarm_obj *alarm)
{
    OS_CPU_SR cpu_sr = 0;
    alarm->ringsta &= 0X7F;     /* ������λ */
    OS_ENTER_CRITICAL();        /* �����ٽ���(�޷����жϴ��) */
    at24cxx_write(SYSTEM_PARA_SAVE_BASE + sizeof(_system_setings), (uint8_t *)alarm, sizeof(_alarm_obj));
    OS_EXIT_CRITICAL();         /* �˳��ٽ���(���Ա��жϴ��) */
}

/**
 * @brief       ���崦��(�ߴ�:44*20)
 * @param       x,y             : ��ʼ����
 * @retval      ������
 */
uint8_t calendar_alarm_msg(uint16_t x, uint16_t y)
{
    uint8_t rval = 0;
    uint16_t *lcdbuf = 0;   /* LCD�Դ� */
    
    lcdbuf = (uint16_t *)gui_memin_malloc(44 * 20 * 2); /* �����ڴ� */

    if (lcdbuf)             /* ����ɹ� */
    {
        app_read_bkcolor(x, y, 44, 20, lcdbuf);         /* ��ȡ����ɫ */
        gui_fill_rectangle(x, y, 44, 20, LIGHTBLUE);
        gui_draw_rectangle(x, y, 44, 20, BROWN);
        gui_show_num(x + 2, y + 2, 2, RED, 16, alarm.hour, 0X81);
        gui_show_ptchar(x + 2 + 16, y + 2, x + 2 + 16 + 8, y + 2 + 16, 0, RED, 16, ':', 1);
        gui_show_num(x + 2 + 24, y + 2, 2, RED, 16, alarm.min, 0X81);
        OSTaskSuspend(6); /* ���������� */

        while (rval == 0)
        {
            tp_dev.scan(0);

            if (tp_dev.sta & TP_PRES_DOWN)  /* ������������ */
            {
                if (app_tp_is_in_area(&tp_dev, x, y, 44, 20))   /* �ж�ĳ��ʱ��,��������ֵ�ǲ�����ĳ�������� */
                {
                    rval = 0XFF;            /* ȡ�� */
                    break;
                }
            }

            delay_ms(5);

            if (system_task_return)break;   /* ��Ҫ���� */
        }

        app_recover_bkcolor(x, y, 44, 20, lcdbuf);  /* ��ȡ����ɫ */
    }
    else rval = 1;

    system_task_return = 0;
    alarm.ringsta &= ~(1 << 7); /* ȡ������ */
    
    calendar_alarm_init((_alarm_obj *)&alarm, &calendar);   /* ���³�ʼ������ */
    
    gui_memin_free(lcdbuf);
    
    OSTaskResume(6);            /* �ָ������� */
    
    system_task_return = 0;
    
    return rval;
}

/**
 * @brief       ��Բ��ָ�����
 * @param       x,y             : �������ĵ�
 * @param       size            : ���̴�С(ֱ��)
 * @param       d               : ���̷ָ�,���ӵĸ߶�
 * @retval      ��
 */
void calendar_circle_clock_drawpanel(uint16_t x, uint16_t y, uint16_t size, uint16_t d)
{
    uint16_t r = size / 2; /* �õ��뾶 */
    uint16_t sx = x - r;
    uint16_t sy = y - r;
    uint16_t px0, px1;
    uint16_t py0, py1;
    uint16_t i;
    gui_fill_circle(x, y, r, WHITE);        /* ����Ȧ */
    gui_fill_circle(x, y, r - 4, BLACK);    /* ����Ȧ */

    for (i = 0; i < 60; i++)   /* �����Ӹ� */
    {
        px0 = sx + r + (r - 4) * sin((app_pi / 30) * i);
        py0 = sy + r - (r - 4) * cos((app_pi / 30) * i);
        px1 = sx + r + (r - d) * sin((app_pi / 30) * i);
        py1 = sy + r - (r - d) * cos((app_pi / 30) * i);
        gui_draw_bline1(px0, py0, px1, py1, 0, WHITE);
    }

    for (i = 0; i < 12; i++)   /* ��Сʱ�� */
    {
        px0 = sx + r + (r - 5) * sin((app_pi / 6) * i);
        py0 = sy + r - (r - 5) * cos((app_pi / 6) * i);
        px1 = sx + r + (r - d) * sin((app_pi / 6) * i);
        py1 = sy + r - (r - d) * cos((app_pi / 6) * i);
        gui_draw_bline1(px0, py0, px1, py1, 2, YELLOW);
    }

    for (i = 0; i < 4; i++)   /* ��3Сʱ�� */
    {
        px0 = sx + r + (r - 5) * sin((app_pi / 2) * i);
        py0 = sy + r - (r - 5) * cos((app_pi / 2) * i);
        px1 = sx + r + (r - d - 3) * sin((app_pi / 2) * i);
        py1 = sy + r - (r - d - 3) * cos((app_pi / 2) * i);
        gui_draw_bline1(px0, py0, px1, py1, 2, YELLOW);
    }

    gui_fill_circle(x, y, d / 2, WHITE);    /* ������Ȧ */
}

/**
 * @brief       ��Բ��ָ�����
 * @param       x,y             : �������ĵ�
 * @param       size            : ���̴�С(ֱ��)
 * @param       d               : ���̷ָ�,���ӵĸ߶�
 * @param       hour            : ʱ��
 * @param       min             : ����
 * @param       sec             : ����
 * @retval      ��
 */
void calendar_circle_clock_showtime(uint16_t x, uint16_t y, uint16_t size, uint16_t d, uint8_t hour, uint8_t min, uint8_t sec)
{
    static uint8_t oldhour = 0; /* ���һ�ν���ú�����ʱ������Ϣ */
    static uint8_t oldmin = 0;
    static uint8_t oldsec = 0;
    float temp;
    uint16_t r = size / 2;      /* �õ��뾶 */
    uint16_t sx = x - r;
    uint16_t sy = y - r;
    uint16_t px0, px1;
    uint16_t py0, py1;
    uint8_t r1;

    if (hour > 11)hour -= 12;

    /* ���Сʱ */
    r1 = d / 2 + 4;
    /* �����һ�ε����� */
    temp = (float)oldmin / 60;
    temp += oldhour;
    px0 = sx + r + (r - 3 * d - 7) * sin((app_pi / 6) * temp);
    py0 = sy + r - (r - 3 * d - 7) * cos((app_pi / 6) * temp);
    px1 = sx + r + r1 * sin((app_pi / 6) * temp);
    py1 = sy + r - r1 * cos((app_pi / 6) * temp);
    gui_draw_bline1(px0, py0, px1, py1, 2, BLACK);

    /* ������� */
    r1 = d / 2 + 3;
    temp = (float)oldsec / 60;
    temp += oldmin;
    /* �����һ�ε����� */
    px0 = sx + r + (r - 2 * d - 7) * sin((app_pi / 30) * temp);
    py0 = sy + r - (r - 2 * d - 7) * cos((app_pi / 30) * temp);
    px1 = sx + r + r1 * sin((app_pi / 30) * temp);
    py1 = sy + r - r1 * cos((app_pi / 30) * temp);
    gui_draw_bline1(px0, py0, px1, py1, 1, BLACK);

    /* ������� */
    r1 = d / 2 + 3;
    /* �����һ�ε����� */
    px0 = sx + r + (r - d - 7) * sin((app_pi / 30) * oldsec);
    py0 = sy + r - (r - d - 7) * cos((app_pi / 30) * oldsec);
    px1 = sx + r + r1 * sin((app_pi / 30) * oldsec);
    py1 = sy + r - r1 * cos((app_pi / 30) * oldsec);
    gui_draw_bline1(px0, py0, px1, py1, 0, BLACK);

    /* ��ʾСʱ */
    r1 = d / 2 + 4;
    /* ��ʾ�µ�ʱ�� */
    temp = (float)min / 60;
    temp += hour;
    px0 = sx + r + (r - 3 * d - 7) * sin((app_pi / 6) * temp);
    py0 = sy + r - (r - 3 * d - 7) * cos((app_pi / 6) * temp);
    px1 = sx + r + r1 * sin((app_pi / 6) * temp);
    py1 = sy + r - r1 * cos((app_pi / 6) * temp);
    gui_draw_bline1(px0, py0, px1, py1, 2, YELLOW);

    /* ��ʾ���� */
    r1 = d / 2 + 3;
    temp = (float)sec / 60;
    temp += min;
    /* ��ʾ�µķ��� */
    px0 = sx + r + (r - 2 * d - 7) * sin((app_pi / 30) * temp);
    py0 = sy + r - (r - 2 * d - 7) * cos((app_pi / 30) * temp);
    px1 = sx + r + r1 * sin((app_pi / 30) * temp);
    py1 = sy + r - r1 * cos((app_pi / 30) * temp);
    gui_draw_bline1(px0, py0, px1, py1, 1, GREEN);

    /* ��ʾ���� */
    r1 = d / 2 + 3;
    /* ��ʾ�µ����� */
    px0 = sx + r + (r - d - 7) * sin((app_pi / 30) * sec);
    py0 = sy + r - (r - d - 7) * cos((app_pi / 30) * sec);
    px1 = sx + r + r1 * sin((app_pi / 30) * sec);
    py1 = sy + r - r1 * cos((app_pi / 30) * sec);
    gui_draw_bline1(px0, py0, px1, py1, 0, RED);
    oldhour = hour;	/* ����ʱ */
    oldmin = min;   /* ����� */
    oldsec = sec;   /* ������ */
}

/**
 * @brief       ʱ����ʾģʽ
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t calendar_play(void)
{
    uint8_t second = 0;
    short temperate = 0;    /* �¶�ֵ */
    uint8_t t = 0;
    uint8_t tempdate = 0;
    uint8_t rval = 0;       /* ����ֵ */
    uint8_t res;
    uint16_t xoff = 0;
    uint16_t yoff = 0;      /* ����yƫ���� */
    uint16_t r = 0;         /* ���̰뾶 */
    uint8_t d = 0;          /* ָ�볤�� */
    uint16_t color = 0X07FF;/* ��ɫ */
    uint32_t br = 0;

    uint8_t TEMP_SEN_TYPE = 0;  /* Ĭ��ʹ��DS18B20 */
    FIL *f_calendar = 0;

    f_calendar = (FIL *)gui_memin_malloc(sizeof(FIL)); /* ����FIL�ֽڵ��ڴ����� */

    if (f_calendar == NULL)rval = 1;    /* ����ʧ�� */
    else
    {
        res = f_open(f_calendar, (const TCHAR *)APP_ASCII_S6030, FA_READ); /* ���ļ� */

        if (res == FR_OK)
        {
            asc2_s6030 = (uint8_t *)gui_memex_malloc(f_calendar->obj.objsize);  /* Ϊ�����忪�ٻ����ַ */

            if (asc2_s6030 == 0)rval = 1;
            else
            {
                res = f_read(f_calendar, asc2_s6030, f_calendar->obj.objsize, (UINT *)&br); /* һ�ζ�ȡ�����ļ� */
            }

            f_close(f_calendar);
        }

        if (res)rval = res;
    }

    if (rval == 0)   /* �޴��� */
    {
        adc_temperature_init(); /* ���³�ʼ���¶ȴ����� */
        lcd_clear(BLACK);       /* ����� */
        second = calendar.sec;  /* �õ��˿̵����� */
        text_show_string(48, 60, lcddev.width, lcddev.height, (char *)calendar_loading_str[gui_phy.language][0], 16, 0x01, color); /* ��ʾ������Ϣ */

        if (ds18b20_init())
        {
            text_show_string(48, 76, lcddev.width, lcddev.height, (char *)calendar_loading_str[gui_phy.language][1], 16, 0x01, color);
            delay_ms(500);
            text_show_string(48, 92, lcddev.width, lcddev.height, (char *)calendar_loading_str[gui_phy.language][2], 16, 0x01, color);
            TEMP_SEN_TYPE = 1;
        }

        delay_ms(1100);     /* �ȴ�1.1s */
        g_back_color = BLACK;
        lcd_clear(BLACK);   /* ����� */
        r = lcddev.width / 3;
        d = lcddev.width / 40;
        yoff = (lcddev.height - r * 2 - 140) / 2;
        TIME_TOPY = yoff + r * 2 + 10;
        OTHER_TOPY = TIME_TOPY + 60 + 10;
        xoff = (lcddev.width - 240) / 2;
        calendar_circle_clock_drawpanel(lcddev.width / 2, yoff + r, r * 2, d); /* ��ʾָ��ʱ�ӱ��� */
        calendar_date_refresh();    /* �������� */
        tempdate = calendar.date;   /* �����ݴ��� */
        gui_phy.back_color = BLACK;
        gui_show_ptchar(xoff + 70 - 4, TIME_TOPY, lcddev.width, lcddev.height, 0, color, 60, ':', 0);	/* ":" */
        gui_show_ptchar(xoff + 150 - 4, TIME_TOPY, lcddev.width, lcddev.height, 0, color, 60, ':', 0);	/* ":" */
    }

    while (rval == 0)
    {
        calendar_get_time(&calendar);   /* ����ʱ�� */

        if (system_task_return)break;   /* ��Ҫ���� */

        if (second != calendar.sec)     /* ���Ӹı��� */
        {
            second = calendar.sec;
            calendar_circle_clock_showtime(lcddev.width / 2, yoff + r, r * 2, d, calendar.hour, calendar.min, calendar.sec); /* ָ��ʱ����ʾʱ�� */
            gui_phy.back_color = BLACK;
            gui_show_num(xoff + 10, TIME_TOPY, 2, color, 60, calendar.hour, 0X80);  /* ��ʾʱ */
            gui_show_num(xoff + 90, TIME_TOPY, 2, color, 60, calendar.min, 0X80);   /* ��ʾ�� */
            gui_show_num(xoff + 170, TIME_TOPY, 2, color, 60, calendar.sec, 0X80);  /* ��ʾ�� */

            if (t % 2 == 0)   /* �ȴ�2���� */
            {
                if (TEMP_SEN_TYPE)temperate = adc_get_temperature() / 10;   /* �õ��ڲ��¶ȴ������ɼ������¶�,0.1�� */
                else temperate = ds18b20_get_temperature();                 /* �õ�18b20�¶� */

                if (temperate < 0)  /* �¶�Ϊ������ʱ�򣬺�ɫ��ʾ */
                {
                    color = RED;
                    temperate = -temperate; /* ��Ϊ���¶� */
                }
                else color = 0X07FF;        /* ����Ϊ�غ�ɫ������ʾ */

                gui_show_num(xoff + 90, OTHER_TOPY, 2, color, 60, temperate / 10, 0X80);    /* XX */
                gui_show_ptchar(xoff + 150, OTHER_TOPY, lcddev.width, lcddev.height, 0, color, 60, '.', 0); /* "." */
                gui_show_ptchar(xoff + 180 - 15, OTHER_TOPY, lcddev.width, lcddev.height, 0, color, 60, temperate % 10 + '0', 0); /* ��ʾС�� */
                gui_show_ptchar(xoff + 210 - 10, OTHER_TOPY, lcddev.width, lcddev.height, 0, color, 60, 95 + ' ', 0);   /* ��ʾ�� */

                if (t > 0)t = 0;
            }

            calendar_get_date(&calendar);   /* �������� */

            if (calendar.date != tempdate)
            {
                calendar_date_refresh();    /* �����仯��,�������� */
                tempdate = calendar.date;   /* �޸�tempdate����ֹ�ظ����� */
            }

            t++;
        }

        delay_ms(20);
    };

    while (tp_dev.sta & TP_PRES_DOWN)tp_dev.scan(0);    /* �ȴ�TP�ɿ� */

    gui_memex_free(asc2_s6030); /* ɾ��������ڴ� */
    asc2_s6030 = 0;             /* ���� */
    gui_memin_free(f_calendar); /* ɾ��������ڴ� */
    g_point_color = BLUE;
    g_back_color = WHITE ;
    return rval;
}




















