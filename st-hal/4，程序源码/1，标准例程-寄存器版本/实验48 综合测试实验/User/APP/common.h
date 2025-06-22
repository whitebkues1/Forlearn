/**
 ****************************************************************************************************
 * @file        common.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.3
 * @date        2022-05-26
 * @brief       APPͨ�� ����
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
 * V1.3 20220526
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#ifndef __COMMON_H
#define __COMMON_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/TOUCH/touch.h"
#include "os.h"
#include "gui.h"


/* Ӳ��ƽ̨��Ӳ���汾���� */
#define HARDWARE_VERSION            25          /* Ӳ���汾,�Ŵ�10��,��1.0��ʾΪ10 */
#define SOFTWARE_VERSION            300         /* ����汾,�Ŵ�100��,��1.00,��ʾΪ100 */

/* ϵͳ���ݱ����ַ */
#define SYSTEM_PARA_SAVE_BASE       100         /* ϵͳ��Ϣ�����׵�ַ.��100��ʼ */


/* ��ͼ��/ͼƬ·�� */
extern uint8_t*const APP_OK_PIC;                /* ȷ��ͼ�� */
extern uint8_t*const APP_CANCEL_PIC;            /* ȡ��ͼ�� */
extern uint8_t*const APP_UNSELECT_PIC;          /* δѡ��ͼ�� */
extern uint8_t*const APP_SELECT_PIC;            /* ѡ��ͼ�� */
extern uint8_t*const APP_VOL_PIC;               /* ����ͼƬ·�� */

extern uint8_t*const APP_ASCII_S14472;          /* ���������144*72����������·�� */
extern uint8_t*const APP_ASCII_S8844;           /* ���������88*44����������·�� */
extern uint8_t*const APP_ASCII_S7236;           /* ���������72*36����������·�� */
extern uint8_t*const APP_ASCII_S6030;           /* ����ܴ�����·�� */
extern uint8_t*const APP_ASCII_5427;            /* ��ͨ������·�� */
extern uint8_t*const APP_ASCII_3618;            /* ��ͨ������·�� */
extern uint8_t*const APP_ASCII_2814;            /* ��ͨ������·�� */

extern uint8_t* asc2_14472;                     /* ��ͨ144*72��������� */
extern uint8_t* asc2_8844;                      /* ��ͨ����88*44��������� */
extern uint8_t* asc2_7236;                      /* ��ͨ����72*36��������� */
extern uint8_t* asc2_s6030;                     /* ���������60*30��������� */
extern uint8_t* asc2_5427;                      /* ��ͨ����54*27��������� */
extern uint8_t* asc2_3618;                      /* ��ͨ����36*18��������� */
extern uint8_t* asc2_2814;                      /* ��ͨ����28*14��������� */

extern const uint8_t APP_ALIENTEK_ICO2424[];    /* ��������ͼ��,�����flash */
extern const uint8_t APP_ALIENTEK_ICO3232[];    /* ��������ͼ��,�����flash */
extern const uint8_t APP_ALIENTEK_ICO4848[];    /* ��������ͼ��,�����flash */


/* APP���ܹ�����Ŀ */
#define APP_FUNS_NUM                18


/* app��Ҫ���ܽ������ */
extern uint8_t*const APP_MFUNS_CAPTION_TBL[APP_FUNS_NUM][GUI_LANGUAGE_NUM];
extern uint8_t*const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM];

extern uint8_t*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern uint8_t*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];

/* ƽ���ߵ���ֹ��ɫ���� */
#define WIN_SMOOTH_LINE_SEC         0XB1FFC4    /* ��ֹ��ɫ */
#define WIN_SMOOTH_LINE_MC          0X1600B1    /* �м���ɫ */

/* ��������ѡ����Ŀ��������Ϣ */
#define APP_ITEM_BTN1_WIDTH         60          /* ��2������ʱ�Ŀ�� */
#define APP_ITEM_BTN2_WIDTH         100         /* ֻ��1������ʱ�Ŀ�� */
#define APP_ITEM_BTN_HEIGHT         30          /* �����߶� */
#define APP_ITEM_ICO_SIZE           32          /* ICOͼ��ĳߴ� */

#define APP_ITEM_SEL_BKCOLOR        0X0EC3      /* ѡ��ʱ�ı���ɫ */
#define APP_WIN_BACK_COLOR          0XC618      /* ���屳��ɫ */


//#define APP_FB_TOPBAR_HEIGHT        20          /* �ļ��������,���������ĸ߶� */
//#define APP_FB_BTMBAR_HEIGHT        20          /* �ļ��������/���Խ���,�ײ������ĸ߶� */
//#define APP_TEST_TOPBAR_HEIGHT      20          /* ���Խ���,���������߶� */

/* ��ֵ���� */
#define app_pi      3.14159


uint32_t app_get_rand(uint32_t max);
void app_srand(uint32_t seed);
void app_set_lcdsize(uint8_t mode);
void app_read_bkcolor(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *ctbl);
void app_recover_bkcolor(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *ctbl);
void app_gui_tcbar(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t mode);
uint8_t app_get_numlen(long long num,uint8_t dir);
void app_show_float(uint16_t x,uint16_t y,long long num,uint8_t flen,uint8_t clen,uint8_t font,uint16_t color,uint16_t bkcolor);
void app_filebrower(uint8_t *topname,uint8_t mode);
void app_show_nummid(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint32_t num,uint8_t len,uint8_t size,uint16_t ptcolor,uint16_t bkcolor);
void app_draw_smooth_line(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint32_t sergb,uint32_t mrgb);

uint8_t app_tp_is_in_area(_m_tp_dev *tp,uint16_t x,uint16_t y,uint16_t width,uint16_t height);
void app_show_items(uint16_t x,uint16_t y,uint16_t itemwidth,uint16_t itemheight,uint8_t*name,uint8_t*icopath,uint16_t color,uint16_t bkcolor);
uint8_t * app_get_icopath(uint8_t mode,uint8_t *selpath,uint8_t *unselpath,uint8_t selx,uint8_t index);
uint8_t app_items_sel(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *items[],uint8_t itemsize,uint8_t *selx,uint8_t mode,uint8_t*caption);
uint8_t app_listbox_select(uint8_t *sel,uint8_t *top,uint8_t * caption,uint8_t *items[],uint8_t itemsize);
void app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor);
void app_muti_remind_msg(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *caption, uint8_t *str);

uint8_t app_system_file_check(uint8_t* diskx);              /* ϵͳ�ļ���� */
uint8_t app_boot_cpdmsg(uint8_t*pname,uint8_t pct,uint8_t mode);
void app_boot_cpdmsg_set(uint16_t x,uint16_t y,uint8_t fsize);
uint8_t app_system_update(uint8_t(*fcpymsg)(uint8_t*pname,uint8_t pct,uint8_t mode),uint8_t* src);
void app_getstm32_sn(uint32_t *sn0,uint32_t *sn1,uint32_t *sn2);
void app_get_version(uint8_t*buf,uint32_t ver,uint8_t len); /* �õ��汾�� */

void app_usmart_getsn(void);                /* USMARTר��. */
uint8_t app_system_parameter_init(void);    /* ϵͳ��Ϣ��ʼ�� */
void app_lcd_auto_bklight(void);            /* LCD�����Զ����ƺ��� */
//void app_es8388_volset(uint8_t vol);        /* ES8388�������� */
//void app_es8388_eqset(_es8388_obj *wmset,uint8_t eqx);  /* eq���� */
//void app_es8388_setall(void);               /* ��������ES8388���� */
void app_set_default(void);                 /* �ָ�Ĭ������ */
#endif




























































