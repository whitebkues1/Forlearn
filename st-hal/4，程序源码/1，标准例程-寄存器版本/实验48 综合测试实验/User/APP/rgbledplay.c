/**
 ****************************************************************************************************
 * @file        rgbledplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-06
 * @brief       APP-RGB�ʵƲ��� ����
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
 * V1.1 20220606
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "rgbledplay.h"
#include "paint.h"
#include "./BSP/TIMER/timer.h"
#include "./BSP/LED/led.h"


#define RGBLED_PROBAR_FONT_COLOR        0X736C      /* 0XAD53 */

/**
 * @brief       ����ɫ��
 *  @note       ɫ��x���������18��y���������13��
 *              ÿ����ɫ��֮����1�����صĿ�϶
 *              width��õ��ڣ�18��������+17
 *              height��õ��ڣ�13��������+12
 * @param       x,y             : ��ʼ����
 * @param       width,height    : ����ߴ�
 * @retval      0, �ɹ�; ����, ʧ��;
 */
uint8_t rgbled_palette_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t xsize, ysize;
    uint16_t r, g, b;
    uint16_t color;
    uint8_t i, j;
    uint16_t tempx = x;

    if (width < 53 || height < 38)return 1;

    xsize = (width - 17) / 18;
    ysize = (height - 12) / 13;

    /* ����ɫ��ɫ�� */
    for (j = 0; j < 12; j++)
    {
        r = (j % 6) * 0x33;

        for (i = 0; i < 18; i++)
        {
            g = 0xff - (i / 6 + (j / 6) * 3) * 0x33;
            b = (i % 6) * 0x33;
            color = (r >> 3) << 11;
            color |= (g >> 2) << 5;
            color |= b >> 3;
            lcd_fill(x, y, x + xsize - 1, y + ysize - 1, color);
            x += xsize + 1;
        }

        y += ysize + 1;
        x = tempx;
    }

    /* ���ڰ׵�ɫ�� */
    for (i = 0; i < 18; i++)
    {
        r = g = b = 0XFF - i * 0x0F;
        color = (r >> 3) << 11;
        color |= (g >> 2) << 5;
        color |= b >> 3;
        lcd_fill(x, y, x + xsize - 1, y + ysize - 1, color);
        x += xsize + 1;
    }

    return 0;
}

/**
 * @brief       ��ʾRGBֵ
 * @param       x,y             : ��ʼ����
 * @param       color           : ��ɫֵ
 * @retval      ��
 */
void rgbled_show_colorval(uint16_t x, uint16_t y, uint8_t size, uint16_t color)
{
    uint16_t fcolor = 0XFFFF - color;
    uint8_t cstr[7];
    gui_phy.back_color = color;	/* ���ñ���ɫ */
    sprintf((char *)cstr, "0X%04X", color);
    gui_show_ptstr(x, y, x + size * 3, y + size, 0, fcolor, size, cstr, 0); /* ��ʾ16λ��ɫֵ */
}

extern volatile uint8_t ledplay_ds0_sta;    /* ledplay����,DS0�Ŀ���״̬ */

/**
 * @brief       RGBLED IO����
 * @param       af              : 0,�������; 1,�������;
 * @retval      ��
 */
void rgbled_io_config(uint8_t af)
{
    RCC->AHB4ENR |= 1 << 1; /* ʹ��PORTBʱ�� */
    RCC->AHB4ENR |= 1 << 4; /* ʹ��PORTEʱ�� */

    if (af == 0)
    {
        TIM15->CR1 &= ~(1 << 0); /* �ر�TIM15 */
        

        sys_gpio_set(GPIOB, SYS_GPIO_PIN4,  /* TIM3_CH1 ����ģʽ���� */
                     SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);
        
        sys_gpio_set(GPIOE, SYS_GPIO_PIN5,  /* TIM15_CH1 ����ģʽ���� */
                     SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

        sys_gpio_set(GPIOE, SYS_GPIO_PIN6,  /* TIM15_CH2 ����ģʽ���� */
                     SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);
        
        ledplay_ds0_sta = 0;    /* ����watch�������LED */
        LEDR(1);    /* �رպ�� */
        LEDG(1);    /* �ر��̵� */
        LEDB(1);    /* �ر����� */
    }
    else
    {
        ledplay_ds0_sta = 1;    /* ������watch�������LED */
        tim15_ch12_pwm_init(255, 240 - 1);

        /* GPIO�� ����λ������ */
        sys_gpio_set(GPIOB, SYS_GPIO_PIN4,  /* TIM3_CH1 ����ģʽ���� */
                     SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);
        
        sys_gpio_set(GPIOE, SYS_GPIO_PIN5,  /* TIM15_CH1 ����ģʽ���� */
                     SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

        sys_gpio_set(GPIOE, SYS_GPIO_PIN6,  /* TIM15_CH2 ����ģʽ���� */
                     SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

        sys_gpio_af_set(GPIOB, SYS_GPIO_PIN4, 2);   /* IO�ڸ��ù���ѡ�� �������ö�!! */
        sys_gpio_af_set(GPIOE, SYS_GPIO_PIN5, 4);   /* IO�ڸ��ù���ѡ�� �������ö�!! */
        sys_gpio_af_set(GPIOE, SYS_GPIO_PIN6, 4);   /* IO�ڸ��ù���ѡ�� �������ö�!! */
    }
}

/**
 * @brief       RGBLED������ɫ����PWMֵ
 * @param       color           : ��ǰ��ɫ
 * @retval      ��
 */
void rgbled_pwm_set(uint16_t color)
{
    uint8_t tempval = 0;
    PIX_RGB565 *rgb = 0;    /* ���r,g,bֵ */
    rgb = (PIX_RGB565 *)&color;

    tempval = rgb->r * 8.23;
    RLED_PWM_VAL = tempval;
    tempval = rgb->g * 4.05;
    GLED_PWM_VAL = tempval;
    tempval = rgb->b * 8.23;
    BLED_PWM_VAL = tempval;
}

/**
 * @brief       RGBLED����
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t rgbled_play(void)
{
    uint16_t x, y;
    uint16_t cpcx, cpcy;
    uint8_t i = 0;
    uint8_t cpcsize = 0;
    uint8_t pbarheight = 0;
    uint16_t disy = 0;
    PIX_RGB565 *rgb = 0;    /* ���r,g,bֵ */
    uint8_t rgbtemp[3];
    uint8_t fontsize = 16;
    uint8_t res = 0;
    uint8_t cmask = 1;      /* ��ɫ�Ƿ��б䶯 */
    uint16_t tempcolor = 0XFFFF;


    _progressbar_obj *pencprgb[3];  /* ��ɫ������ */
    lcd_clear(LGRAY);
    app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02); /* �·ֽ��� */
    gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[13][gui_phy.language]); /* ��ʾ���� */

    if (lcddev.width <= 272)
    {
        cpcsize = 10;
        fontsize = 16;
    }
    else if (lcddev.width == 320)
    {
        cpcsize = 14;
        fontsize = 16;
    }
    else if (lcddev.width >= 480)
    {
        cpcsize = 20;
        fontsize = 24;
    }

    pbarheight = (cpcsize / 2) * 3;
    disy = (lcddev.height - gui_phy.tbheight - 8 * pbarheight - 13 * cpcsize - 12) / 3;

    cpcx = (lcddev.width - 18 * cpcsize - 17) / 2;
    cpcy = gui_phy.tbheight + disy;
    rgbled_palette_draw(cpcx, cpcy, 18 * cpcsize + 17, 13 * cpcsize + 12);


    x = 2 * pbarheight;
    y = gui_phy.tbheight + disy * 2 + 4 * pbarheight + 13 * cpcsize + 12;

    for (i = 0; i < 3; i++)
    {
        if (i == 0)gui_show_string("R:", x - fontsize, y, fontsize, fontsize, fontsize, RGBLED_PROBAR_FONT_COLOR);  /* ��ʾR: */

        if (i == 1)gui_show_string("G:", x - fontsize, y, fontsize, fontsize, fontsize, RGBLED_PROBAR_FONT_COLOR);  /* ��ʾG: */

        if (i == 2)gui_show_string("B:", x - fontsize, y, fontsize, fontsize, fontsize, RGBLED_PROBAR_FONT_COLOR);  /* ��ʾB: */

        pencprgb[i] = progressbar_creat(x, y, lcddev.width - 4 * pbarheight, pbarheight, 0X20); /* ���������� */

        if (pencprgb[i] == NULL)res = 1;

        y += pbarheight + pbarheight / 2;
    }

    pencprgb[0]->totallen = 31; /* ��ɫ�������ֵ */
    pencprgb[1]->totallen = 63; /* ��ɫ�������ֵ */
    pencprgb[2]->totallen = 31; /* ��ɫ�������ֵ */
    /* Բ���ĵ����� */
    x = lcddev.width / 2;
    y = gui_phy.tbheight + (disy * 3) / 2 + 2 * pbarheight + 13 * cpcsize + 12;

    if (res == 0)
    {
        rgbtemp[0] = 31;
        rgbtemp[1] = 63;
        rgbtemp[2] = 31;
        rgbled_io_config(1);

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

            if (cmask)   /* ��������Ƿ���Ҫ���� */
            {
                if (cmask == 1)   /* ��������Ҫ���� */
                {
                    rgb = (PIX_RGB565 *)&tempcolor;
                    pencprgb[0]->curpos = rgb->r;
                    pencprgb[1]->curpos = rgb->g;
                    pencprgb[2]->curpos = rgb->b;
                    rgbtemp[0] = rgb->r;            /* ��ʱ���� */
                    rgbtemp[1] = rgb->g;
                    rgbtemp[2] = rgb->b;

                    for (i = 0; i < 3; i++)progressbar_draw_progressbar(pencprgb[i]); /* ���½����� */
                }

                cmask = 0;
                gui_fill_circle(x, y, pbarheight * 2, tempcolor);   /* ��Ԥ��ͼ */
                rgbled_show_colorval(x - (fontsize * 3) / 2, y - fontsize / 2, fontsize, tempcolor); /* ��ʾ��ֵ */
                rgbled_pwm_set(tempcolor);
            }

            for (i = 0; i < 3; i++)   /* ������������� */
            {
                res = progressbar_check(pencprgb[i], &in_obj);

                if (res && (rgbtemp[i] != pencprgb[i]->curpos)) /* �������Ķ��� */
                {
                    rgbtemp[i] = pencprgb[i]->curpos;   /* �������µĽ�� */
                    rgb->r = rgbtemp[0];
                    rgb->g = rgbtemp[1];
                    rgb->b = rgbtemp[2];
                    tempcolor = *(uint16_t *)rgb;
                    cmask = 2; /* ������������,��������Ҫ���� */
                }

            }

            if (app_tp_is_in_area(&tp_dev, cpcx, cpcy, 18 * cpcsize + 16, 13 * cpcsize + 11) && (tp_dev.sta & TP_PRES_DOWN))
            {
                tempcolor = gui_phy.read_point(tp_dev.x[0], tp_dev.y[0]);

                if (tempcolor != LGRAY)cmask = 1;
            }

            if (system_task_return)break;   /* TPAD���� */
        }

        rgbled_io_config(0);
    }

    return res;
}




















