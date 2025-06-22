/**
 ****************************************************************************************************
 * @file        keyplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-11-01
 * @brief       APP-�������� ����
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
 * V1.0 20221101
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "keyplay.h"
#include "gradienter.h"
#include "./BSP/KEY/key.h"



/**
 * @brief       ��ʾԲ����ʾ��Ϣ
 * @param       x,y             : Ҫ��ʾ��Բ��������
 * @param       r               : �뾶
 * @param       fsize           : �����С
 * @param       color           : Բ����ɫ
 * @param       str             : ��ʾ���ַ���
 * @retval      ��
 */
void key_show_circle(uint16_t x, uint16_t y, uint16_t r, uint8_t fsize, uint16_t color, uint8_t *str)
{
    gui_fill_circle(x, y, r, color);
    gui_show_strmid(x - r, y - fsize / 2, 2 * r, fsize, BLUE, fsize, str); /* ��ʾ���� */
}

/**
 * @brief       ��������
 * @param       caption         : ��������
 * @retval      δ�õ�
 */
uint8_t key_play(uint8_t *caption)
{
    uint8_t key;
    uint16_t k0y, k1y, kuy;
    uint16_t kx;
    uint16_t kcr;
    uint8_t fsize = 0;          /* key�����С */

    uint8_t keyold = 0XFF;      /* ������֮ǰ�İ���ֵ */

    kcr = (lcddev.height - 20) / 8;
    kx = lcddev.width / 2;

    k0y = lcddev.height - kcr - kcr / 2;
    k1y = k0y - 2 * kcr - kcr / 2;
    kuy = k1y - 2 * kcr - kcr / 2;

    if (lcddev.width <= 272)
    {
        fsize = 12;
    }
    else if (lcddev.width == 320)
    {
        fsize = 16;
    }
    else if (lcddev.width >= 480)
    {
        fsize = 24;
    }


    lcd_clear(LGRAY);
    app_gui_tcbar(0, 0, lcddev.width, gui_phy.tbheight, 0x02);  /* �·ֽ��� */
    gui_show_strmid(0, 0, lcddev.width, gui_phy.tbheight, WHITE, gui_phy.tbfsize, caption); /* ��ʾ���� */
    system_task_return = 0;

    while (1)
    {
        key = key_scan(1);

        if (key != keyold)
        {
            keyold = key;

            if (key == KEY0_PRES)key_show_circle(kx, k0y, kcr, fsize, RED, (uint8_t *)"KEY0");
            else key_show_circle(kx, k0y, kcr, fsize, YELLOW, (uint8_t *)"KEY0");

            if (key == KEY1_PRES)key_show_circle(kx, k1y, kcr, fsize, RED, (uint8_t *)"KEY1");
            else key_show_circle(kx, k1y, kcr, fsize, YELLOW, (uint8_t *)"KEY1");

            if (key == WKUP_PRES)key_show_circle(kx, kuy, kcr, fsize, RED, (uint8_t *)"KEYUP");
            else key_show_circle(kx, kuy, kcr, fsize, YELLOW, (uint8_t *)"KEYUP");
        }

        if (system_task_return)break;   /* TPAD���� */

        delay_ms(10);
    }

    return 0;
}







































