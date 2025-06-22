/**
 ****************************************************************************************************
 * @file        nesplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2022-10-28
 * @brief       APP-NESģ���� ����
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
 * V1.1 20160627
 * ������SMS��Ϸ��֧��
 * V1.2 20221028
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�uint8_t/uint16_t/uint32_tΪuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "nesplay.h"
#include "nes_main.h"
#include "spb.h"
#include "audioplay.h"
#include "./BSP/USART3/usart3.h"


/* ROMָ�� */
uint8_t *rom_file;

/* ���� */
uint8_t *const nes_caption_tbl[GUI_LANGUAGE_NUM] = {"NESģ����", "NESģ�M��", "NES Emulator",};

/* �ֱ�������ʾ */
uint8_t *const nes_remindmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "1,P8:PB10��COM3_RX,PB11��COM3_TX\r\
2,K1:��JOYPAD��\r\
3,���ֱ�����COM3�ӿ�\r\
4,�˳���Ӧ�ú���ָ�ԭ�������ã�",
    "1,P8:PB10��COM3_RX,PB11��COM3_TX\r\
2,K1:��JOYPAD�n\r\
3,���ֱ�����COM3�ӿ�\r\
4,�˳��������ᣬՈ�֏�ԭ����O�ã�",
    "1,P8:PB10-COM3_RX,PB11-COM3_TX\r\
2,K1:Turn to JOYPAD\r\
3,Insert GAMEPAD to COM3\r\
4,Exit,restore the orig settings",
};

/* ������ʾ */
uint8_t *const nes_errormsg_tbl[3][GUI_LANGUAGE_NUM] =
{
    {"�ڴ治��!", "�ȴ治��!", "Out of memory!",},
    {"�ļ���ȡ����!", "�ļ��xȡ�e�`!", "Read file error!",},
    {"MAP��֧��!", "MAP��֧��!", "Not supported MAP!",},
};

/**
 * @brief       ������Ϸ����
 * @param       ��
 * @retval      ��
 */
void nes_load_ui(void)
{
    app_filebrower((uint8_t *)nes_caption_tbl[gui_phy.language], 0X05);     /* ��ʾ���� */
    gui_fill_rectangle(0, 20, lcddev.width, lcddev.height - 20, BLACK);     /* ����ɫ */
}

/**
 * @brief       ��ʼ��Ϸ
 * @param       ��
 * @retval      ��
 */
uint8_t nes_play(void)
{
    DIR nesdir;         /* nesdirר�� */
    FILINFO *nesinfo;
    uint8_t res;
    uint8_t rval = 0;   /* ����ֵ */
    uint8_t *pname = 0;
    uint8_t *fn;

    _btn_obj *rbtn;     /* ���ذ�ť�ؼ� */
    _filelistbox_obj *flistbox;
    _filelistbox_list *filelistx;   /* �ļ� */

    app_muti_remind_msg((lcddev.width - 210) / 2, (lcddev.height - 158) / 2, 210, 158, APP_REMIND_CAPTION_TBL[gui_phy.language], nes_remindmsg_tbl[gui_phy.language]);
    lcd_clear(BLACK);
    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[6][gui_phy.language], 0X07);    /* ѡ��Ŀ���ļ�,���õ�Ŀ������ */
    flistbox = filelistbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.listfsize); /* ����һ��filelistbox */

    if (flistbox == NULL)rval = 1;          /* �����ڴ�ʧ�� */
    else
    {
        flistbox->fliter = FLBOX_FLT_NES;   /* �����ı��ļ� */
        filelistbox_add_disk(flistbox);     /* ��Ӵ���·�� */
        filelistbox_draw_listbox(flistbox);
    }

    /* Ϊ���ļ������뻺���� */
    
    nesinfo = gui_memin_malloc(sizeof(FILINFO));    /* Ϊ�ļ���Ϣ�����ڴ� */
 

    rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */

    if (rbtn == NULL)rval = 1;  /* û���㹻�ڴ湻���� */
    else
    {
        rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language]; /* ���� */
        rbtn->font = gui_phy.tbfsize;   /* �����µ������С */
        rbtn->bcfdcolor = WHITE;        /* ����ʱ����ɫ */
        rbtn->bcfucolor = WHITE;        /* �ɿ�ʱ����ɫ */
        btn_draw(rbtn);                 /* ����ť */
    }

    while (rval == 0)
    {
        tp_dev.scan(0);
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
        delay_ms(5);

        if (system_task_return)break;           /* TPAD���� */

        filelistbox_check(flistbox, &in_obj);   /* ɨ���ļ� */
        res = btn_check(rbtn, &in_obj);

        if (res)
        {
            if (((rbtn->sta & 0X80) == 0))      /* ��ť״̬�ı��� */
            {
                if (flistbox->dbclick != 0X81)
                {
                    filelistx = filelist_search(flistbox->list, flistbox->selindex); /* �õ���ʱѡ�е�list����Ϣ */

                    if (filelistx->type == FICO_DISK)   /* �Ѿ������������� */
                    {
                        break;
                    }
                    else filelistbox_back(flistbox);    /* �˻���һ��Ŀ¼ */
                }
            }
        }

        if (flistbox->dbclick == 0X81) /* ˫���ļ��� */
        {
            rval = f_opendir(&nesdir, (const TCHAR *)flistbox->path); /* ��ѡ�е�Ŀ¼ */

            if (rval)break;

            dir_sdi(&nesdir, flistbox->findextbl[flistbox->selindex - flistbox->foldercnt]);
            rval = f_readdir(&nesdir, nesinfo); /* ��ȡ�ļ���Ϣ */

            if (rval)break; /* �򿪳ɹ� */

            fn = (uint8_t *)nesinfo->fname;
            pname = gui_memin_malloc(strlen((const char *)fn) + strlen((const char *)flistbox->path) + 2); /* �����ڴ� */

            if (pname == NULL)rval = 1; /* ����ʧ�� */
            else
            {
                pname = gui_path_name(pname, flistbox->path, fn); /* �ļ�������·�� */

                if (audiodev.status & (1 << 7)) /* ��ǰ�ڷŸ�?? */
                {
                    audio_stop_req(&audiodev);  /* ֹͣ��Ƶ���� */
                    audio_task_delete();        /* ɾ�����ֲ������� */
                    delay_ms(1010);
                }

                lcd_clear(BLACK);
                app_filebrower(fn, 0X05);   /* ��ʾ��ǰ���������Ϸ���� */

                /* SRAM���� */
                FSMC_Bank1->BTCR[5] &= 0XFFFF00FF;  /* ���֮ǰ������ */
                FSMC_Bank1->BTCR[5] |= 7 << 8;      /* ���ݱ���ʱ�䣨DATAST��Ϊ8��HCLK 8/128M=62.5ns */
                USART3->CR1 &= ~(1 << 5);           /* ��ֹ����3���գ���ʱ���ٴ���GSM�����Ϣ�� */
                
                /* ��ʼnes��Ϸ */
                res = nes_load(pname);              /* ��ʼnes��Ϸ */

                if (res)
                {
                    window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2, 220, 100, (uint8_t *)nes_errormsg_tbl[res - 1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);
                    delay_ms(1200);
                }

                /* SRAM���� */
                FSMC_Bank1->BTCR[5] &= 0XFFFF00FF;      /* ���֮ǰ������ */
                FSMC_Bank1->BTCR[5] |= 2 << 8;          /* ���ݱ���ʱ�䣨DATAST��Ϊ7��HCLK 7/128M=55ns */

                system_task_return = 0;                 /* �˳���־���� */
                
                usart3_init(36, 115200);                /* �ָ�����3���� */
            }

            flistbox->dbclick = 0;      /* ���÷��ļ����״̬ */
            gui_memin_free(pname);      /* �ͷ��ڴ� */
            pname = NULL;
            app_filebrower((uint8_t *)nes_caption_tbl[gui_phy.language], 0X07); /* ѡ��Ŀ���ļ�,���õ�Ŀ������ */
            btn_draw(rbtn);/* ����ť */
            delay_ms(100);
            
            filelistbox_rebuild_filelist(flistbox);/* �ؽ�flistbox */
            system_task_return = 0; /* �ո��˳����������Ϸ,�������˳����ѭ�� */
            tpad_init(6);           /* ��������TPADֵ */
        }
    }

    filelistbox_delete(flistbox);   /* ɾ��filelist */
    btn_delete(rbtn);               /* ɾ����ť */
    gui_memin_free(pname);          /* �ͷ��ڴ� */
    gui_memin_free(nesinfo);
    return rval;
}






















