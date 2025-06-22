/**
 ****************************************************************************************************
 * @file        notepad.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-���±� ����
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

#include "spb.h"
#include "notepad.h"
#include "calendar.h"
#include "./T9INPUT/t9input.h"


/* ģʽѡ�� */
uint8_t *const notepad_mode_tbl[GUI_LANGUAGE_NUM][2] =
{
    {"�½��ı��ļ�", "�������ļ�",},
    {"�½��ı��ļ�", "���_�����ļ�",},
    {"Create new text file", "Open exist file",},
};

/**
 * @brief       ͨ��ʱ���ȡ�ļ���
 *  @note       ������sd������,��֧��flash disk����
 *              ��ϳ�:����"0:TEXT/TEXT20120321210633.txt"/"2:TEXT/TEXT20120321210633.txt"���ļ���
 * @param       pname           : ��·��������
 * @retval      ��
 */
void notepad_new_pathname(uint8_t *pname)
{
    calendar_get_time(&calendar);
    calendar_get_date(&calendar);

    if (gui_phy.memdevflag & (1 << 0))sprintf((char *)pname, "0:TEXT/TEXT%04d%02d%02d%02d%02d%02d.txt", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);         /* ��ѡ������SD�� */
    else if (gui_phy.memdevflag & (1 << 2))sprintf((char *)pname, "2:TEXT/TEXT%04d%02d%02d%02d%02d%02d.txt", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);    /* SD��������,�򱣴���U�� */
}

/* ��setings.c���涨�� */
extern uint8_t *const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM];


/**
 * @brief       ���±�����
 *  @note       �����½��ı��ļ�,��������SD����TEXT�ļ�����
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t notepad_play(void)
{
    FIL *f_txt = 0;         /* �ļ� */
    DIR notepaddir;         /* notepaddirר�� */
    FILINFO *notepadinfo;
    _btn_obj *rbtn = 0;     /* ���ذ�ť�ؼ� */
    _filelistbox_obj *flistbox = 0;
    _filelistbox_list *filelistx = 0;   /* �ļ��� */
    _t9_obj *t9 = 0;        /* ���뷨 */
    _memo_obj *tmemo = 0;   /* memo�ؼ� */
    uint8_t editmask = 0;   /* ��־�Ƿ�����˱༭ */
    uint8_t *pname = 0;
    uint8_t *fn;
    uint8_t res;
    uint8_t rval = 0;
    uint8_t mode = 0;
    uint32_t temp;          /* ��ʱ���� */
    uint16_t t9height = 0;  /* t9���뷨�߶� */

    uint8_t *buf;           /* ���� */
    uint8_t *p;
    uint32_t readlen;       /* �ܶ�ȡ���� */
    uint32_t bread;         /* ��ȡ�ĳ��� */

    /* ��ѡ��ģʽ */
    rval = app_items_sel((lcddev.width - 180) / 2, (lcddev.height - 152) / 2, 180, 72 + 40 * 2, (uint8_t **)notepad_mode_tbl[gui_phy.language], 2, (uint8_t *)&mode, 0X90, (uint8_t *)APP_MODESEL_CAPTION_TBL[gui_phy.language]); /* 2��ѡ�� */
    f_txt = (FIL *)gui_memin_malloc(sizeof(FIL));   /* ����FIL�ֽڵ��ڴ����� */

    if (f_txt == NULL)rval = 1; /* ����ʧ�� */

    rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */
    notepadinfo = (FILINFO *)gui_memin_malloc(sizeof(FILINFO)); /* ����FILENFO�ڴ� */
    buf = gui_memin_malloc(1024);   /* ����1K�ֽ��ڴ� */

    if (!notepadinfo || !rbtn || !buf)rval = 1;	/* û���㹻�ڴ湻���� */
    else
    {
        rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language];  /* ���� */
        rbtn->font = gui_phy.tbfsize;   /* �����µ������С */
        rbtn->bcfdcolor = WHITE;    /* ����ʱ����ɫ */
        rbtn->bcfucolor = WHITE;    /* �ɿ�ʱ����ɫ */
        btn_draw(rbtn);             /* �ػ���ť */
    }

    if (rval == 0)      /* ȷ�ϼ�������,ͬʱ����f_txt����OK,RBTN����OK */
    {
        lcd_clear(0);

        if (mode == 0)  /* �½��ı��ļ� */
        {
            if (gui_phy.memdevflag & (1 << 0))f_mkdir("0:TEXT");    /* ǿ�ƴ����ļ���,�����±��� */

            if (gui_phy.memdevflag & (1 << 3))f_mkdir("2:TEXT");    /* ǿ�ƴ����ļ���,�����±��� */

            pname = gui_memin_malloc(40); /* ����40���ֽ��ڴ�,����"0:TEXT/TEXT20120321210633.txt" */
            pname[0] = '\0'; /* ��ӽ����� */
            notepad_new_pathname(pname);
            f_txt->obj.objsize = 0;

            if (pname == NULL)rval = 1;
        }
        else    /* ѡ��һ���ı��ļ��� */
        {
            app_filebrower((uint8_t *)notepad_mode_tbl[gui_phy.language][1], 0X07); /* ��ʾ��Ϣ */
            btn_read_backcolor(rbtn);   /* ���¶�ȡ��ť����ɫ */
            btn_draw(rbtn);             /* ����ť */
            flistbox = filelistbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.listfsize); /* ����һ��filelistbox */

            if (flistbox == NULL)rval = 1;  /* �����ڴ�ʧ��. */
            else
            {
                flistbox->fliter = FLBOX_FLT_TEXT | FLBOX_FLT_LRC;  /* ����TEXT�ļ� */
                filelistbox_add_disk(flistbox);                     /* ��Ӵ���·�� */
                filelistbox_draw_listbox(flistbox);
            }

            while (rval == 0)
            {
                tp_dev.scan(0);
                in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
                delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

                if (system_task_return)                 /* TPAD���� */
                {
                    rval = 1; /* ǿ���˳� */
                    break;
                }

                filelistbox_check(flistbox, &in_obj);   /* ɨ���ļ� */
                res = btn_check(rbtn, &in_obj);

                if (res)
                {
                    if (((rbtn->sta & 0X80) == 0)) /* ��ť״̬�ı��� */
                    {
                        if (flistbox->dbclick != 0X81)
                        {
                            filelistx = filelist_search(flistbox->list, flistbox->selindex); /* �õ���ʱѡ�е�list����Ϣ */

                            if (filelistx->type == FICO_DISK)
                            {
                                rval = 1;
                                break;  /* �Ѿ������������� */
                            }
                            else filelistbox_back(flistbox);    /* �˻���һ��Ŀ¼ */
                        }
                    }
                }

                if (flistbox->dbclick == 0X81) /* ˫���ļ��� */
                {
                    rval = f_opendir(&notepaddir, (const TCHAR *)flistbox->path); /* ��ѡ�е�Ŀ¼ */

                    if (rval)break;

                    dir_sdi(&notepaddir, flistbox->findextbl[flistbox->selindex - flistbox->foldercnt]);
                    rval = f_readdir(&notepaddir, notepadinfo); /* ��ȡ�ļ���Ϣ */

                    if (rval)break; /* ��ʧ�� */

                    fn = (uint8_t *)(notepadinfo->fname);
                    pname = gui_memin_malloc(strlen((const char *)fn) + strlen((const char *)flistbox->path) + 2); /* �����ڴ� */

                    if (pname == NULL)rval = 1; /* ����ʧ�� */

                    {
                        pname = gui_path_name(pname, flistbox->path, fn);   /* �ļ�������·�� */
                        res = f_open(f_txt, (const TCHAR *)pname, FA_READ); /* �Զ���ʽ���ļ� */

                        if (res)rval = 1;

                        break;
                    }
                }
            }
        }

        if (rval == 0) /* ǰ������������� */
        {
            temp = my_mem_perused(SRAMIN);
            temp = (MEM1_MAX_SIZE * (1000 - temp)) / 1000; /* �õ��ڲ��ڴ�ʣ���ڴ����� */

            if ((f_txt->obj.objsize + NOTEPAD_EDIT_LEN) > temp)
            {
                rval = 1;
            }

            if (lcddev.width == 800)t9height = 548;         /* t9���뷨�߶� */
            else if (lcddev.width == 600)t9height = 368;    /* t9���뷨�߶� */
            else if (lcddev.width == 480)t9height = 266;    /* t9���뷨�߶� */
            else if (lcddev.width == 320 || lcddev.width == 272)t9height = 176; /* t9���뷨�߶� */
            else if (lcddev.width == 240)t9height = 134;

            tmemo = memo_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight - t9height, 0, 1, gui_phy.tbfsize, f_txt->obj.objsize + NOTEPAD_EDIT_LEN); /* ��������txt��ô����ڴ� */

            if (tmemo == NULL)rval = 1;
            else    /* memo�����ɹ� */
            {
                if (mode == 1)
                {
                    p = tmemo->text;
                    readlen = 0;

                    while (readlen < f_txt->obj.objsize)
                    {
                        res = f_read(f_txt, buf, 1024, (UINT *)&bread); /* ����txt��������� */
                        readlen += bread;
                        my_mem_copy(p, buf, bread);
                        p += bread;

                        if (res)break;
                    }

                    f_close(f_txt);/* �ر��ļ� */
                }

                memo_draw_memo(tmemo, 0);
                app_filebrower(pname, 0X05);    /* ��ʾ���������� */
                t9 = t9_creat((lcddev.width % 5) / 2, lcddev.height - t9height, lcddev.width - (lcddev.width % 5), t9height, 0);

                if (t9 == NULL)rval = 1;
                else t9_draw(t9);
            }
        }

        while (rval == 0)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* ��ʱһ��ʱ�ӽ��� */

            if (system_task_return)break;           /* TPAD���� */

            t9_check(t9, &in_obj);
            memo_check(tmemo, &in_obj);

            if (t9->outstr[0] != NULL) /* ����ַ� */
            {
                //printf("%s\r\n",t9->outstr);
                memo_add_text(tmemo, t9->outstr);
                t9->outstr[0] = NULL;               /* �������ַ� */
                editmask = 1;                       /* ��ǽ����˱༭ */
            }
        }
    }

    if (editmask) /* �б༭��,��Ҫ���� */
    {
        res = window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, "", (uint8_t *)APP_SAVE_CAPTION_TBL[gui_phy.language], 12, 0, 0X03, 0);

        if (res == 1) /* ��Ҫ���� */
        {
            if (mode == 0)res = f_open(f_txt, (const TCHAR *)pname, FA_READ | FA_WRITE | FA_CREATE_NEW); /* �����µ��ļ� */
            else res = f_open(f_txt, (const TCHAR *)pname, FA_READ | FA_WRITE); /* �Զ�д��ʽ���ļ� */

            if (res == 0) /* �򿪳ɹ� */
            {
                f_lseek(f_txt, 0);  /* ָ���ļ��Ŀ�ʼ��ַ */
                f_write(f_txt, tmemo->text, strlen((const char *)tmemo->text), (UINT *)&bread);    /* �����ļ� */
                f_close(f_txt);
            }
            else /* ���ļ�ʧ��,��ʾ��Ϣ */
            {
                window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)APP_CREAT_ERR_MSG_TBL[gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* ��ʾSD���Ƿ���� */
                delay_ms(2000);/* �ȴ�2���� */
            }
        }
    }

    t9_delete(t9);
    filelistbox_delete(flistbox);
    btn_delete(rbtn);
    memo_delete(tmemo);
    gui_memin_free(f_txt);
    gui_memin_free(pname);
    gui_memin_free(notepadinfo);
    gui_memin_free(buf);
    return rval;
}








