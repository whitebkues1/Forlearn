/**
 ****************************************************************************************************
 * @file        exeplay.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-11-16
 * @brief       APP-������ ����
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
 * V1.1 20221116
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "exeplay.h"


/* �ٺ���,��PCָ���ܵ��µ�main����ȥ */
dummyfun jump2app;

/* ִ��BIN�ļ�ǰ����ʾ */
uint8_t *const exeplay_remindmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "  ִ��BIN�ļ���,ϵͳ���޷���������,ֻ�ܸ�λ����.��ȷ��ִ��ô?",
    "  ����BIN�ļ���,ϵ�y���o���^�m�\��,ֻ�܏�λ�؆�.���_�J����ô?",
    "  After run the BIN file,SYSTEM will unable to continue.Confirm?",
};

/* ���� */
uint8_t *const exeplay_warning_tbl[GUI_LANGUAGE_NUM] =
{
    "����:",
    "����:",
    "Warning:",
};

/* APP����Ƿ���ʾ�� */
uint8_t *const exeplay_apperrmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "�Ƿ���SRAM APP����", "�Ƿ���SRAM APP����", "Illegal SRAM APP File!",
};

/* APP���ڶ�ȡ��ʾ�� */
uint8_t *const exeplay_readmsg_tbl[GUI_LANGUAGE_NUM] =
{
    "�ļ���ȡ��...", "�ļ��xȡ��...", "File reading...",
};

/**
 * @brief       д���־ֵ
 * @param       val             : ��־ֵ
 * @retval      ��
 */
void exeplay_write_appmask(uint16_t val)
{
    RCC->APB1ENR |= 1 << 28;    /* ʹ�ܵ�Դʱ�� */
    RCC->APB1ENR |= 1 << 27;    /* ʹ�ܱ���ʱ�� */
    rtc_write_bkr(1, val);
}

/**
 * @brief       ����Ƿ�ִ��APP����
 *  @note       ���������ʼ��ʱ�򱻵���. ����Ƿ���app������Ҫִ��.�����,��ֱ��ִ��.
 * @param       ��
 * @retval      ��
 */
void exeplay_app_check(void)
{
    uint8_t *xdes = (uint8_t *)EXEPLAY_APP_BASE;    /* Ŀ���ַ */ 
    uint32_t offset = 0;
    uint32_t n = EXEPLAY_APP_SIZE;                  /* ���ݳ��� */

    if (rtc_read_bkr(1) == 0X5050)      /* ���BKP1,���Ϊ0X5050,��˵����Ҫִ��app���� */
    {
        exeplay_write_appmask(0X0000);  /* д��0,��ֹ��λ���ٴ�ִ��app����. */

        while (n)   /* �������� */
        {
            norflash_read(xdes + offset, EXEPLAY_SRC_BASE + offset, 1024);
            offset += 1024;
            n -= 1024;
        }

        jump2app = (dummyfun) * (volatile uint32_t *)(EXEPLAY_APP_BASE + 4);                    /* �û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ) */
        sys_msr_msp(*(volatile uint32_t *)EXEPLAY_APP_BASE);                                    /* ��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ�Ŷ�ջ��ַ) */

        jump2app();                     /* ִ��app����. */
    }
}

/**
 * @brief       ������
 * @param       ��
 * @retval      0, �����˳�; ����, �������;
 */
uint8_t exe_play(void)
{
    FIL *f_exe;
    DIR exeplaydir;         /* exeplaydirר�� */
    FILINFO *exeplayinfo;
    uint8_t res;
    uint8_t rval = 0;       /* ����ֵ */
    uint8_t *pname = 0;
    uint8_t *fn;
    uint8_t *pbuf;
    uint32_t offset = 0;
    uint32_t bread;         /* ��ȡ�ĳ��� */

    _btn_obj *rbtn;         /* ���ذ�ť�ؼ� */
    _filelistbox_obj *flistbox;
    _filelistbox_list *filelistx;   /* �ļ� */

    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[5][gui_phy.language], 0X07); /* ��ʾ��Ϣ */
    flistbox = filelistbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.listfsize); /* ����һ��filelistbox */

    if (flistbox == NULL)rval = 1;          /* �����ڴ�ʧ�� */
    else
    {
        flistbox->fliter = FLBOX_FLT_BIN;   /* ����BIN�ļ� */
        filelistbox_add_disk(flistbox);     /* ��Ӵ���·�� */
        filelistbox_draw_listbox(flistbox);
    }

    f_exe = (FIL *)gui_memin_malloc(sizeof(FIL));   /* ����FIL�ֽڵ��ڴ����� */

    if (f_exe == NULL)rval = 1; /* ����ʧ�� */

    rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */

    exeplayinfo = (FILINFO *)gui_memin_malloc(sizeof(FILINFO)); /* ����FILENFO�ڴ� */

    if (!exeplayinfo || !rbtn)rval = 1; /* û���㹻�ڴ湻���� */
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
            if (((rbtn->sta & 0X80) == 0)) /* ��ť״̬�ı��� */
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
            rval = f_opendir(&exeplaydir, (const TCHAR *)flistbox->path); /* ��ѡ�е�Ŀ¼ */

            if (rval)break;

            dir_sdi(&exeplaydir, flistbox->findextbl[flistbox->selindex - flistbox->foldercnt]);
            rval = f_readdir(&exeplaydir, exeplayinfo);     /* ��ȡ�ļ���Ϣ */

            if (rval)break; /* �򿪳ɹ� */

            fn = (uint8_t *)(exeplayinfo->fname);
            pname = gui_memin_malloc(strlen((const char *)fn) + strlen((const char *)flistbox->path) + 2); /* �����ڴ� */

            if (pname == NULL)rval = 1; /* ����ʧ�� */

            {
                pname = gui_path_name(pname, flistbox->path, fn);   /* �ļ�������·�� */
                rval = f_open(f_exe, (const TCHAR *)pname, FA_READ);/* ֻ����ʽ���ļ� */

                if (rval)break; /* ��ʧ�� */

                if (f_exe->obj.objsize < EXEPLAY_APP_SIZE)  /* ���Էŵ���,�����û�ȷ��ִ�� */
                {
                    pbuf = gui_memin_malloc(4096);          /* ����4K�ֽ��ڴ� */
 
                    if (!pbuf)break;            /* ����ʧ�� */

                    rval = f_read(f_exe, pbuf, 512, (UINT *)&bread); /* ����ǰ512�ֽ� */

                    if (rval)break;             /* ��ȡ���� */

                    if (((*(volatile uint32_t *)(pbuf + 4)) & 0xFF000000) == 0x20000000)    /* �ж��Ƿ�Ϊ0X20XXXXXX.���APP�ĺϷ��ԡ� */
                    {
                        res = window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 160) / 2, 200, 160, (uint8_t *)exeplay_remindmsg_tbl[gui_phy.language], (uint8_t *)exeplay_warning_tbl[gui_phy.language], 16, 0, 0X03, 0); /* ��ʾ������Ϣ */

                        if (res == 1)           /* ������ȷ�ϼ� */
                        {
                            window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)exeplay_readmsg_tbl[gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0); /* ��ʾ���ڶ�ȡ */
                            f_lseek(f_exe, 0);  /* ƫ�Ƶ���ͷ */

                            while (1)           /* ��ȡ�����ļ� */
                            {
                                rval = f_read(f_exe, pbuf, 4096, (UINT *)&bread); /* ����BIN���������� */
                                norflash_write(pbuf, EXEPLAY_SRC_BASE + offset, bread);
                                offset += bread;

                                if (bread != 4096)break;
                            }

                            if (rval)break; /* ��ȡ���� */

                            exeplay_write_appmask(0X5050);  /* д���־��,��־��app��Ҫ���� */
                            sys_soft_reset();               /* ����һ����λ */
                        }
                    }
                    else /* �Ƿ�APP�ļ� */
                    {
                        window_msg_box((lcddev.width - 160) / 2, (lcddev.height - 80) / 2, 160, 80, (uint8_t *)exeplay_apperrmsg_tbl[gui_phy.language], (uint8_t *)exeplay_warning_tbl[gui_phy.language], 12, 0, 0, 0); /* ��ʾ�Ƿ���APP�ļ� */
                        delay_ms(1500);/* ��ʱ1.5�� */
                    }

                    filelistbox_rebuild_filelist(flistbox);/* �ؽ�flistbox */
                }
            }
            flistbox->dbclick = 0;  /* ���÷��ļ����״̬ */
            gui_memin_free(pname);  /* �ͷ��ڴ� */
        }
    }

    filelistbox_delete(flistbox);   /* ɾ��filelist */
    btn_delete(rbtn);               /* ɾ����ť */
    gui_memin_free(pname);          /* �ͷ��ڴ� */
    gui_memin_free(exeplayinfo);
    gui_memin_free(f_exe);
    gui_memin_free(pbuf);
    return rval;
}















