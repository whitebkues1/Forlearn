/**
 ****************************************************************************************************
 * @file        videoplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-26
 * @brief       APP-��Ƶ������ ����
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

#include "videoplay.h"
#include "settings.h"
#include "ucos_ii.h"
#include "audioplay.h"
//#include "es8388.h"
//#include "sai.h"
//#include "wavplay.h"
#include "./BSP/TIMER/btim.h"


__videodev videodev;        /* ��Ƶ���ſ����� */
__videoui *vui;             /* ��Ƶ���Ž�������� */


extern volatile uint8_t framecnt;       /* ͳһ��֡������ */
extern volatile uint8_t framecntout;    /* ͳһ��֡������������� */
volatile uint8_t aviframeup;            /* ��Ƶ����ʱ϶���Ʊ���,������1��ʱ��,���Ը�����һ֡��Ƶ */


/* 5��ͼƬ��ť��·�� */
uint8_t *const AUDIO_BTN_PIC_TBL[2][5] =
{
    {
        "1:/SYSTEM/APP/AUDIO/ListR.bmp",
        "1:/SYSTEM/APP/AUDIO/PrevR.bmp",
        "1:/SYSTEM/APP/AUDIO/PauseR.bmp",
        "1:/SYSTEM/APP/AUDIO/NextR.bmp",
        "1:/SYSTEM/APP/AUDIO/ExitR.bmp",
    },
    {
        "1:/SYSTEM/APP/AUDIO/ListP.bmp",
        "1:/SYSTEM/APP/AUDIO/PrevP.bmp",
        "1:/SYSTEM/APP/AUDIO/PlayP.bmp",
        "1:/SYSTEM/APP/AUDIO/NextP.bmp",
        "1:/SYSTEM/APP/AUDIO/ExitP.bmp",
    },
};

uint8_t *const AUDIO_PLAYR_PIC = "1:/SYSTEM/APP/AUDIO/PlayR.bmp";   /* ���� �ɿ� */
uint8_t *const AUDIO_PLAYP_PIC = "1:/SYSTEM/APP/AUDIO/PlayP.bmp";   /* ���� ���� */
uint8_t *const AUDIO_PAUSER_PIC = "1:/SYSTEM/APP/AUDIO/PauseR.bmp"; /* ��ͣ �ɿ� */
uint8_t *const AUDIO_PAUSEP_PIC = "1:/SYSTEM/APP/AUDIO/PauseP.bmp"; /* ��ͣ ���� */


/**
 * @brief       ��ʾaudio����ʱ��
 * @param       sx,sy           : ��ʼ����
 * @param       sec             : ʱ��(��λ:s)
 * @retval      ��
 */
void video_time_show(uint16_t sx, uint16_t sy, uint16_t sec)
{
    uint16_t min;
    uint8_t *buf;
    buf = gui_memin_malloc(100);

    if (buf == 0)return;

    min = sec / 60; /* �õ������� */
    sec = sec % 60; /* �õ������� */
    sprintf((char *)buf, "%03d:%02d", min, sec);
    gui_fill_rectangle(sx, sy, vui->msgfsize * 3, vui->msgfsize, AUDIO_MAIN_BKCOLOR);	/* ����ɫ */
    gui_show_string(buf, sx, sy, vui->msgfsize * 3, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);
    gui_memin_free(buf);
}

/**
 * @brief       ��Ƶ����������UI
 * @param       ��
 * @retval      ��
 */
void video_load_ui(void)
{
    if (lcddev.width == 240)
    {
        vui->tpbar_height = 20;
        vui->capfsize = 12;
        vui->msgfsize = 12; /* ���ܴ���16 */

        vui->msgbar_height = 46;
        vui->nfsize = 12;
        vui->xygap = 4;
        vui->msgdis = 4;    /* ����3��dis */

        vui->prgbar_height = 30;
        vui->pbarwidth = 140;   /* ������������  14*vui->msgfsize/2 */

        vui->btnbar_height = 60;
    }
    else if (lcddev.width == 272)
    {
        vui->tpbar_height = 24;
        vui->capfsize = 12;
        vui->msgfsize = 12; /* ���ܴ���16 */

        vui->msgbar_height = 50;
        vui->nfsize = 12;
        vui->xygap = 4;
        vui->msgdis = 6;    /* ����3��dis */

        vui->prgbar_height = 30;
        vui->pbarwidth = 170;   /* ������������  14*vui->msgfsize/2 */

        vui->btnbar_height = 80;
    }
    else if (lcddev.width == 320)
    {
        vui->tpbar_height = 24;
        vui->capfsize = 12;
        vui->msgfsize = 12; /* ���ܴ���16 */

        vui->msgbar_height = 50;
        vui->nfsize = 12;
        vui->xygap = 4;
        vui->msgdis = 16;   /* ����3��dis */

        vui->prgbar_height = 30;
        vui->pbarwidth = 210;   /* ������������  14*vui->msgfsize/2 */

        vui->btnbar_height = 80;
    }
    else if (lcddev.width == 480)
    {
        vui->tpbar_height = 30;
        vui->capfsize = 16;
        vui->msgfsize = 12; /* ���ܴ���16 */

        vui->msgbar_height = 60;
        vui->nfsize = 12;
        vui->xygap = 6;
        vui->msgdis = 24;   /* ����3��dis */

        vui->prgbar_height = 40;
        vui->pbarwidth = 340;   /* ������������  14*vui->msgfsize/2 */

        vui->btnbar_height = 120;
    }
    else if (lcddev.width == 600)
    {
        vui->tpbar_height = 40;
        vui->capfsize = 24;
        vui->msgfsize = 16; /* ���ܴ���16 */

        vui->msgbar_height = 100;
        vui->nfsize = 16;
        vui->xygap = 10;
        vui->msgdis = 40;   /* ����3��dis */

        vui->prgbar_height = 60;
        vui->pbarwidth = 400;   /* ������������  12*vui->msgfsize/2 */

        vui->btnbar_height = 150;
    }
    else if (lcddev.width == 800)
    {
        vui->tpbar_height = 60;
        vui->capfsize = 32;
        vui->msgfsize = 16; /* ���ܴ���16 */

        vui->msgbar_height = 120;
        vui->nfsize = 16;
        vui->xygap = 10;
        vui->msgdis = 60;   /* ����3��dis */

        vui->prgbar_height = 60;
        vui->pbarwidth = 600;   /* ������������  12*vui->msgfsize/2 */

        vui->btnbar_height = 180;
    }

    vui->vbarheight = vui->msgfsize; /* ����vui->msgfsize�Ĵ�С */
    vui->pbarheight = vui->msgfsize; /* ����vui->msgfsize�Ĵ�С */
    vui->vbarwidth = lcddev.width - 16 - 2 * vui->xygap - 3 * vui->msgdis - 14 * vui->msgfsize / 2;
    vui->vbarx = vui->msgdis + 16 + vui->xygap;
    vui->vbary = vui->tpbar_height + vui->xygap * 2 + vui->msgfsize + (vui->msgbar_height - (vui->msgfsize + vui->xygap * 2 + vui->xygap / 2 + vui->msgfsize + vui->vbarheight)) / 2;
    vui->pbarx = (lcddev.width - vui->pbarwidth - 12 * vui->msgfsize / 2) / 2 + vui->msgfsize * 6 / 2;
    vui->pbary = lcddev.height - vui->btnbar_height - vui->prgbar_height + (vui->prgbar_height - vui->pbarheight) / 2;



    gui_fill_rectangle(0, 0, lcddev.width, vui->tpbar_height, AUDIO_TITLE_BKCOLOR);	/* ����������ɫ */
    gui_show_strmid(0, 0, lcddev.width, vui->tpbar_height, AUDIO_TITLE_COLOR, vui->capfsize, (uint8_t *)APP_MFUNS_CAPTION_TBL[3][gui_phy.language]);    /* ��ʾ���� */
    gui_fill_rectangle(0, vui->tpbar_height, lcddev.width, vui->msgbar_height, AUDIO_MAIN_BKCOLOR);             /* �����Ϣ������ɫ */
    minibmp_decode((uint8_t *)APP_VOL_PIC, vui->msgdis, vui->vbary - (16 - vui->msgfsize) / 2, 16, 16, 0, 0);   /* ��������ͼ�� */
    gui_show_string("00%", vui->vbarx, vui->vbary + vui->vbarheight + vui->xygap / 2, 3 * vui->msgfsize / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);   /* ��ʾ���� */
    gui_fill_rectangle(0, lcddev.height - vui->btnbar_height - vui->prgbar_height, lcddev.width, vui->prgbar_height, AUDIO_MAIN_BKCOLOR);   /* ��������������ɫ */
    gui_fill_rectangle(0, lcddev.height - vui->btnbar_height, lcddev.width, vui->btnbar_height, AUDIO_BTN_BKCOLOR); /* ��䰴ť������ɫ */
    gui_fill_rectangle(0, vui->tpbar_height + vui->msgbar_height, lcddev.width, lcddev.height - vui->tpbar_height - vui->msgbar_height - vui->prgbar_height - vui->btnbar_height, AUDIO_MAIN_BKCOLOR); /* ����ɫ */
}

/**
 * @brief       ��ʾ�����ٷֱ�
 * @param       pctx            : �ٷֱ�ֵ
 * @retval      ��
 */
void video_show_vol(uint8_t pctx)
{
    uint8_t *buf;
    uint8_t sy = vui->vbary + vui->vbarheight + vui->xygap / 2;
    gui_phy.back_color = AUDIO_MAIN_BKCOLOR; /* ���ñ���ɫΪ��ɫ */
    gui_fill_rectangle(vui->vbarx, sy, 4 * vui->msgfsize / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR); /* ��䱳��ɫ */
    buf = gui_memin_malloc(32);
    sprintf((char *)buf, "%d%%", pctx);
    gui_show_string(buf, vui->vbarx, sy, 4 * vui->msgfsize / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR); /* ��ʾ���� */
    gui_memin_free(buf);
}

/**
 * @brief       ��Ƶ��Ϣ����
 * @param       videodevx       : video ������
 * @param       videoprgbx      : ��Ƶ������
 * @param       volprgbx        : ����������
 * @param       aviinfo         : ��Ƶ��Ϣ
 * @param       flag            : �������ݱ�־, 1, ��Ҫ�������ֵ���Ϣ; 0, ����Ҫ�������ֵ���Ϣ;
 * @retval      ��
 */
void video_info_upd(__videodev *videodevx, _progressbar_obj *videoprgbx, _progressbar_obj *volprgbx, AVI_INFO *aviinfo, uint8_t flag)
{
    static uint32_t oldsec;
    uint16_t tempx, tempy;
    uint8_t *buf;
    float ftemp;
    uint32_t totsec = 0;    /* video�ļ���ʱ�� */
    uint32_t cursec;        /* ��ǰ����ʱ�� */

    if (flag == 1)   /* ��Ҫ�������ֵ���Ϣ */
    {
        buf = gui_memin_malloc(100);    /* ����100�ֽ��ڴ� */

        if (buf == NULL)return; /* game over */

        gui_fill_rectangle(0, vui->tpbar_height + vui->xygap - 1, lcddev.width, vui->msgfsize + 2, AUDIO_MAIN_BKCOLOR); /* ���¸������һ��,���֮ǰ����ʾ */
        gui_show_ptstrwhiterim(vui->xygap, vui->tpbar_height + vui->xygap, lcddev.width - vui->xygap, lcddev.height, 0, 0X0000, 0XFFFF, vui->msgfsize, videodevx->name);    /* ��ʾ�µ����� */
        gui_phy.back_color = AUDIO_MAIN_BKCOLOR; /* ���ñ���ɫΪ��ɫ */

        /* ��ʾ�����ٷֱ� */
        video_show_vol((volprgbx->curpos * 100) / volprgbx->totallen); /* ��ʾ�����ٷֱ� */

        /* ��ʾ��Ŀ��� */
        sprintf((char *)buf, "%03d/%03d", videodevx->curindex + 1, videodevx->mfilenum);
        tempx = vui->vbarx + vui->vbarwidth - 7 * (vui->msgfsize) / 2;
        tempy = vui->vbary + vui->xygap / 2 + vui->vbarheight;
        gui_fill_rectangle(tempx, tempy, 7 * (vui->msgfsize) / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);   /* ���֮ǰ����ʾ */
        gui_show_string(buf, tempx, tempy, 7 * (vui->msgfsize) / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);

        /* ��ʾxxxKhz */
        tempx = vui->vbarx + vui->vbarwidth + vui->msgdis;
        gui_fill_rectangle(tempx, vui->vbary, 9 * vui->msgfsize / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);    /* ���֮ǰ����ʾ */
        ftemp = (float)aviinfo->SampleRate / 1000; /* xxx.xKhz */
        sprintf((char *)buf, "%2.3fKhz", ftemp);
        gui_show_string(buf, tempx, vui->vbary, 9 * vui->msgfsize / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);

        /* ��ʾ�ֱ��� */
        tempy = vui->vbary + vui->xygap / 2 + vui->vbarheight;
        gui_fill_rectangle(tempx, tempy, 11 * (vui->msgfsize) / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);	/* ���֮ǰ����ʾ */
        sprintf((char *)buf, "Pix:%dX%d", aviinfo->Width, aviinfo->Height);
        gui_show_string(buf, tempx, tempy, 11 * (vui->msgfsize) / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);	/* ��ʾ�ֱ��� */

        /* �������� */
        oldsec = 0;
        videoprgbx->totallen = videodevx->file->obj.objsize;        /* �����ܳ��� */
        videoprgbx->curpos = 0;
        gui_memin_free(buf);/* �ͷ��ڴ� */
    }

    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;   /* �����ܳ���(��λ:ms) */
    totsec /= 1000; /* ������ */
    cursec = ((double)videodevx->file->fptr / videodevx->file->obj.objsize) * totsec; /* ��ǰ���ŵ���������? */
    videoprgbx->curpos = f_tell(videodevx->file);   /* �õ���ǰ�Ĳ���λ�� */
    progressbar_draw_progressbar(videoprgbx);       /* ���½�����λ�� */

    if (oldsec != cursec)
    {
        oldsec = cursec;
        /* ��ʾ֡�� */
        buf = gui_memin_malloc(100);	/* ����100�ֽ��ڴ� */

        if (buf == NULL)return;

        tempx = vui->vbarx + vui->vbarwidth + vui->msgdis + 9 * (vui->msgfsize) / 2 + vui->xygap;
        gui_fill_rectangle(tempx, vui->vbary, 5 * (vui->msgfsize) / 2, vui->msgfsize, AUDIO_MAIN_BKCOLOR);		/* ���֮ǰ����ʾ */
        sprintf((char *)buf, "%02dfps", framecntout);
        gui_show_string(buf, tempx, vui->vbary, 5 * (vui->msgfsize) / 2, vui->msgfsize, vui->msgfsize, AUDIO_INFO_COLOR);
        gui_memin_free(buf);/* �ͷ��ڴ� */
        /* ��ʾʱ�� */
        tempx = vui->pbarx - 7 * vui->msgfsize / 2;
        video_time_show(tempx, vui->pbary, cursec);			/* ��ʾ����ʱ�� */
        tempx = vui->pbarx + vui->pbarwidth + vui->msgfsize / 2;
        video_time_show(tempx, vui->pbary, totsec);			/* ��ʾ��ʱ�� */
    }
}

/* ��Ƶ������ʾ�� */
uint8_t *const video_msg_tbl[3][GUI_LANGUAGE_NUM] =
{
    {"��ȡ�ļ�����!!", "�xȡ�ļ��e�`!!", "File Read Error!",},
    {"�ڴ治��!!", "�ȴ治��!!", "Out of memory!",},
    {"�ֱ��ʲ�֧��", "�ֱ��ʲ�֧��", "Image Size Error!",},
};

/**
 * @brief       ��Ƶ����
 * @param       ��
 * @retval      ���Ž��
 */
uint8_t video_play(void)
{
    uint8_t rval = 0;   /* ����ֵ */
    uint8_t *pname = 0;
    uint16_t i;
    uint8_t key;

    _btn_obj *rbtn;     /* ���ذ�ť�ؼ� */
    _filelistbox_obj *flistbox;
    _filelistbox_list *filelistx;   /* �ļ� */
    memset(&videodev, 0, sizeof(__videodev)); /* videodev�ṹ������ */
    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[3][gui_phy.language], 0X07); /* ѡ��Ŀ���ļ�,���õ�Ŀ������ */

    flistbox = filelistbox_creat(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight * 2, 1, gui_phy.listfsize); /* ����һ��filelistbox */

    if (flistbox == NULL)rval = 1;          /* �����ڴ�ʧ�� */
    else
    {
        flistbox->fliter = FLBOX_FLT_VIDEO; /* ͼƬ�ļ� */
        filelistbox_add_disk(flistbox);     /* ��Ӵ���·�� */
        filelistbox_draw_listbox(flistbox);
    }

    rbtn = btn_creat(lcddev.width - 2 * gui_phy.tbfsize - 8 - 1, lcddev.height - gui_phy.tbheight, 2 * gui_phy.tbfsize + 8, gui_phy.tbheight - 1, 0, 0x03); /* �������ְ�ť */

    if (rbtn == NULL)rval = 1;  /* û���㹻�ڴ湻���� */
    else
    {
        rbtn->caption = (uint8_t *)GUI_BACK_CAPTION_TBL[gui_phy.language];	/* ���� */
        rbtn->font = gui_phy.tbfsize; /* �����µ������С */
        rbtn->bcfdcolor = WHITE;    /* ����ʱ����ɫ */
        rbtn->bcfucolor = WHITE;    /* �ɿ�ʱ����ɫ */
        btn_draw(rbtn); /* ����ť */
    }

    while (rval == 0)   /* ��ѭ�� */
    {
        tp_dev.scan(0); /* ɨ�败���� */
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* �õ�������ֵ */
        delay_ms(5);

        if (system_task_return)break;           /* TPAD���� */

        filelistbox_check(flistbox, &in_obj);   /* ɨ���ļ� */

        if (flistbox->dbclick == 0X81)          /* ˫���ļ��� */
        {
            gui_memin_free(videodev.path);      /* �ͷ��ڴ� */
            gui_memin_free(videodev.mfindextbl);    /* �ͷ��ڴ� */
            videodev.path = (uint8_t *)gui_memin_malloc(strlen((const char *)flistbox->path) + 1); /* Ϊ�µ�·�������ڴ� */

            if (videodev.path == NULL)
            {
                rval = 1;    /* �ڴ����ʧ�� */
                break;
            }

            videodev.path[0] = '\0';    /* ���ʼ��������� */
            strcpy((char *)videodev.path, (char *)flistbox->path);
            videodev.mfindextbl = (uint32_t *)gui_memin_malloc(flistbox->filecnt * 4); /* Ϊ�µ�tbl�����ڴ� */

            if (videodev.mfindextbl == NULL)
            {
                rval = 1;    /* �ڴ����ʧ�� */
                break;
            }

            for (i = 0; i < flistbox->filecnt; i++)videodev.mfindextbl[i] = flistbox->findextbl[i]; /* ���� */

            videodev.mfilenum = flistbox->filecnt;  /* ��¼�ļ����� */
            videodev.curindex = flistbox->selindex - flistbox->foldercnt; /* ��ǰӦ�ò��ŵ��ļ����� */
            printf("play:%d/%d\r\n", videodev.curindex, videodev.mfilenum);
            rval = video_play_mjpeg();

            if (rval == 0XFF)break;
            else
            {
                if (rval)   /* ���ڴ��� */
                {
                    if (rval < 20)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)video_msg_tbl[0][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);       /* �ļ���ȡ���� */

                    if (rval == 0X21)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)video_msg_tbl[1][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);    /* �ڴ���� */

                    if (rval == 0X22)window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 100) / 2, 200, 100, (uint8_t *)video_msg_tbl[2][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 0, 0);    /* �ֱ��ʴ��� */

                    delay_ms(2000);
                    rval = 0;
                }

                flistbox->dbclick = 0;  /* ���÷��ļ����״̬ */
                app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[3][gui_phy.language], 0X07); /* ѡ��Ŀ���ļ�,���õ�Ŀ������ */
                btn_draw(rbtn);         /* ����ť */
                flistbox->selindex = flistbox->foldercnt + videodev.curindex; /* ѡ����ĿΪ��ǰ���ڲ��ŵ���Ŀ */
                filelistbox_rebuild_filelist(flistbox);/* �ؽ�flistbox */
                system_task_return = 0; /* �������˳���Ƶ���� */
            }
        }

        key = btn_check(rbtn, &in_obj);

        if (key && ((rbtn->sta & 0X80) == 0))
        {
            if (flistbox->dbclick != 0X81)   /* ���ļ������ʱ���˷��ذ�ť,�򷵻���һ��Ŀ¼ */
            {
                filelistx = filelist_search(flistbox->list, flistbox->selindex); /* �õ���ʱѡ�е�list����Ϣ */

                if (filelistx->type == FICO_DISK)   /* �Ѿ�������������,���˳���Ƶ��� */
                {
                    break;
                }
                else filelistbox_back(flistbox);   /* �˻���һ��Ŀ¼ */
            }
        }
    }

    filelistbox_delete(flistbox);           /* ɾ��filelist */
    btn_delete(rbtn);                       /* ɾ����ť */
    gui_memin_free(pname);                  /* �ͷ��ڴ� */
    gui_memin_free(videodev.path);          /* �ͷ��ڴ� */
    gui_memin_free(videodev.mfindextbl);    /* �ͷ��ڴ� */
    return rval;
}

extern volatile uint8_t memshow_flag;/* mem��ӡʹ�ܱ�־ */

/**
 * @brief       ����mjpeg�ļ�
 * @param       ��
 * @retval      0, �������Ž���,�����ļ��������
 *              0XFF, ���˳���ť,ֱ�ӽ�����Ƶ����
 *              ����, �������
 */
uint8_t video_play_mjpeg(void)
{
    uint8_t dirmode = 0;    /* ������ģʽ:0,����ģʽ;1,����ģʽ. */
    uint8_t *pname = 0;     /* ��·�������� */
    uint8_t *framebuf = 0;  /* ��Ƶ����buf */
    uint8_t *pbuf = 0;      /* bufָ�� */
    DIR videodir;           /* videoĿ¼ */
    FILINFO *videoinfo;     /* video�ļ���Ϣ */
    _progressbar_obj *videoprgb = 0, *volprgb = 0;
    _btn_obj *tbtn[5];
    uint16_t lastvolpos;
    uint8_t btnsize = 0;    /* ��ť�ߴ� */
    uint8_t btnxpit = 0;    /* ��ť��x�����ϵļ�϶ */
    uint8_t i;
    uint8_t tcnt = 0;

    uint8_t  res = 0;
    uint16_t offset = 0;
    uint32_t nr;
    uint8_t saisavebuf = 0;
    uint16_t videoheight;   /* ��Ƶ��ʾ����߶� */
    uint16_t yoff;
    uint8_t key = 0;
    uint8_t playflag = 0;


    for (i = 0; i < 5; i++)tbtn[i] = 0;

//    if(audiodev.status&(1<<7))      /* ��ǰ�ڷŸ�?? */
//    {
//        audio_stop_req(&audiodev);  /* ֹͣ��Ƶ���� */
//        audio_task_delete();        /* ɾ�����ֲ������� */
//    }
//    app_es8388_volset(es8388set.mvol);
//    ES8388_ADDA_Cfg(1,0);           /* ����DAC�ر�ADC */
//    ES8388_Output_Cfg(1,1);         /* ����DAC��� */
//    ES8388_Input_Cfg(0);            /* �ر�����ͨ�� */

    videoinfo = (FILINFO *)gui_memin_malloc(sizeof(FILINFO));   /* ����FILENFO�ڴ� */
    framebuf = gui_memin_malloc(AVI_VIDEO_BUF_SIZE);            /* ������Ƶbuf */
    videodev.saibuf[0] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* ������Ƶ�ڴ� */
    videodev.saibuf[1] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* ������Ƶ�ڴ� */
    videodev.saibuf[2] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* ������Ƶ�ڴ� */
    videodev.saibuf[3] = gui_memin_malloc(AVI_AUDIO_BUF_SIZE);  /* ������Ƶ�ڴ� */
    videodev.file = (FIL *)gui_memin_malloc(sizeof(FIL));       /* ����videodev.file�ڴ� */
    vui = (__videoui *)gui_memin_malloc(sizeof(__videoui));

    if (!videodev.saibuf[3] || !framebuf || !videodev.file)
    {
        res = 0X21;
    }
    else
    {
        memset(videodev.saibuf[0], 0, AVI_AUDIO_BUF_SIZE);
        memset(videodev.saibuf[1], 0, AVI_AUDIO_BUF_SIZE);
        memset(videodev.saibuf[2], 0, AVI_AUDIO_BUF_SIZE);
        memset(videodev.saibuf[3], 0, AVI_AUDIO_BUF_SIZE);
        video_load_ui();            /* ���������� */
        videoprgb = progressbar_creat(vui->pbarx, vui->pbary, vui->pbarwidth, vui->pbarheight, 0X20);   /* audio���Ž����� */

        if (videoprgb == NULL)res = 0X21;

        volprgb = progressbar_creat(vui->vbarx, vui->vbary, vui->vbarwidth, vui->vbarheight, 0X20);     /* ������С������ */

        if (volprgb == NULL)res = 0X21;

        volprgb->totallen = 30;
        videoheight = lcddev.height - (vui->tpbar_height + vui->msgbar_height + vui->prgbar_height + vui->btnbar_height);
        yoff = vui->tpbar_height + vui->msgbar_height;
//        if(es8388set.mvol<=30)volprgb->curpos=es8388set.mvol;
//        else  /* ��������� */
//        {
//            es8388set.mvol=0;
//            volprgb->curpos=0;
//        }
        lastvolpos = volprgb->curpos; /* �趨�����λ�� */

        switch (lcddev.width)
        {
            case 240:
                btnsize = 48;
                break;

            case 272:
                btnsize = 50;
                break;

            case 320:
                btnsize = 60;
                break;

            case 480:
                btnsize = 80;
                break;

            case 600:
                btnsize = 100;
                break;

            case 800:
                btnsize = 150;
                break;

        }

        btnxpit = (lcddev.width - 5 * btnsize) / 5;

        for (i = 0; i < 5; i++)   /* ѭ������5����ť */
        {
            tbtn[i] = btn_creat(btnxpit / 2 + i * (btnsize + btnxpit), lcddev.height - btnsize - (vui->btnbar_height - btnsize) / 2, btnsize, btnsize, 0, 1); /* ����ͼƬ��ť */

            if (tbtn[i] == NULL)
            {
                res = 0X21;    /* ����ʧ�� */
                break;
            }

            tbtn[i]->bcfdcolor = 0X2CFF;            /* ����ʱ�ı���ɫ */
            tbtn[i]->bcfucolor = AUDIO_BTN_BKCOLOR; /* �ɿ�ʱ����ɫ */
            tbtn[i]->picbtnpathu = (uint8_t *)AUDIO_BTN_PIC_TBL[0][i];
            tbtn[i]->picbtnpathd = (uint8_t *)AUDIO_BTN_PIC_TBL[1][i];
            tbtn[i]->sta = 0;
        }

        if (res == 0)
        {
            res = f_opendir(&videodir, (const TCHAR *)videodev.path); /* ��ѡ�е�Ŀ¼ */
        }
    }

    if (res == 0)
    {
        videoprgb->inbkcolora = 0x738E; /* Ĭ��ɫ */
        videoprgb->inbkcolorb = AUDIO_INFO_COLOR;   /* Ĭ��ɫ */
        videoprgb->infcolora = 0X75D;   /* Ĭ��ɫ */
        videoprgb->infcolorb = 0X596;   /* Ĭ��ɫ */
        volprgb->inbkcolora = AUDIO_INFO_COLOR;     /* Ĭ��ɫ */
        volprgb->inbkcolorb = AUDIO_INFO_COLOR;     /* Ĭ��ɫ */
        volprgb->infcolora = 0X75D;     /* Ĭ��ɫ */
        volprgb->infcolorb = 0X596;     /* Ĭ��ɫ */

        for (i = 0; i < 5; i++)btn_draw(tbtn[i]);   /* ����ť */

        tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PLAYR_PIC; /* ����һ��֮���Ϊ�����ɿ�״̬ */
        progressbar_draw_progressbar(videoprgb);    /* �������� */
        progressbar_draw_progressbar(volprgb);      /* �������� */
    }

    while (res == 0)
    {
        ff_enter(videodir.obj.fs);/* ����fatfs,��ֹ����� */
        dir_sdi(&videodir, videodev.mfindextbl[videodev.curindex]);
        ff_leave(videodir.obj.fs);/* �˳�fatfs,��������os�� */
        res = f_readdir(&videodir, videoinfo); /* ��ȡ�ļ���Ϣ */

        if (res)break; /* ��ʧ�� */

        videodev.name = (uint8_t *)(videoinfo->fname);
        pname = gui_memin_malloc(strlen((const char *)videodev.name) + strlen((const char *)videodev.path) + 2); /* �����ڴ� */

        if (pname == NULL)
        {
            res = 0X21;    /* ����ʧ�� */
            break;
        }

        pname = gui_path_name(pname, videodev.path, videodev.name);	/* �ļ�������·�� */
        res = f_open(videodev.file, (char *)pname, FA_READ);
        gui_memin_free(pname);  /* �ͷ��ڴ� */

        if (res == 0)
        {
            pbuf = framebuf;
            res = f_read(videodev.file, pbuf, AVI_VIDEO_BUF_SIZE, &nr); /* ��ʼ��ȡ */

            if (res)   /* �ļ�ϵͳ����,ֱ���˳����ٲ��� */
            {
                break;
            }

            /* ��ʼavi���� */
            res = avi_init(pbuf, AVI_VIDEO_BUF_SIZE);   /* avi���� */

            if (res)   /* һ������,�����Լ������� */
            {
                printf("avi err:%d\r\n", res);
                break;
            }

            if (g_avix.Height > videoheight || g_avix.Width > lcddev.width)   /* ͼƬ�ߴ�������,ֱ���˳����ٲ��� */
            {
                if (g_avix.Height <= lcddev.width && g_avix.Width <= lcddev.height)   /* ����ʱ,������ʾ����,�����ú���ģʽ */
                {
                    dirmode = 1;    /* ����ģʽ */
                    lcd_display_dir(1);     /* ��Ϊ���� */
                    tp_dev.touchtype = 1;   /* ����Ҳ�ĳɺ��� */
                    yoff = 0;
                    videoheight = lcddev.height;
                }
                else     /* ����Ҳ�޷���ʾ */
                {
                    res = 0X22;
                    printf("avi size error\r\n");
                    break;
                }
            }
            else
            {
                if (g_avix.Height >= 480 && g_avix.Width >= 800 && (1000000 / g_avix.SecPerFrame) >= 15)   /* ��Ƶ�ֱ��ʴ��ڵ���800*480,��֡�ʴ��ڵ���15֡��ʱ��,��Ҫ������ʾ,�������ٶ�Ҫ�� */
                {
                    dirmode = 1;            /* ����ģʽ */
                    lcd_display_dir(1);     /* ��Ϊ���� */
                    tp_dev.touchtype = 1;   /* ����Ҳ�ĳɺ��� */
                    yoff = 0;
                    videoheight = lcddev.height;
                }
            }

            tim6_int_init(g_avix.SecPerFrame / 100 - 1, 24000 - 1);   /* 10Khz����Ƶ��,��1��100us,������Ƶ֡������� */
            tim8_int_init(10000 - 1, 24000 - 1);        /* 10Khz����,1�����ж�һ��,����֡���� */
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi"); /* Ѱ��movi ID */
            avi_get_streaminfo(pbuf + offset + 4);      /* ��ȡ����Ϣ */
            f_lseek(videodev.file, offset + 12);        /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */
            res = mjpeg_init((lcddev.width - g_avix.Width) / 2, yoff + (videoheight - g_avix.Height) / 2, g_avix.Width, g_avix.Height); /* JPG�����ʼ�� */

            if (g_avix.SampleRate)    /* ����Ƶ��Ϣ,�ų�ʼ�� */
            {
//                ES8388_I2S_Cfg(0, 3);   /* �����ֱ�׼,16λ���ݳ��� */
//                SAIA_Init(0, 1, 4);     /* ����SAI,������,16λ���� */
//                SAIA_SampleRate_Set(g_avix.SampleRate);/* ���ò����� */
//                SAIA_TX_DMA_Init(videodev.saibuf[1], videodev.saibuf[2], g_avix.AudioBufSize / 2, 1); /* ����DMA */
//                sai_tx_callback = audio_sai_dma_callback;	/* �ص�����ָ��sai_DMA_Callback */
//                videodev.saiplaybuf = 0;
//                saisavebuf = 0;
//                SAI_Play_Start();   /* ����sai���� */
            }

            gui_fill_rectangle(0, yoff, lcddev.width, videoheight, BLACK);  /* �����Ƶ���� */

            if (dirmode == 0)video_info_upd(&videodev, videoprgb, volprgb, &g_avix, 1);   /* ����������Ϣ */

            tcnt = 0;
            playflag = 0;
            videodev.status = 3;    /* ����ͣ,�ǿ������ */
            memshow_flag = 0;       /* ����ӡ�ڴ�ʹ���� */

            while (playflag == 0)   /* ����ѭ�� */
            {
                if (videodev.status & (1 << 0) && videodev.status & (1 << 1))
                {
                    if (g_avix.StreamID == AVI_VIDS_FLAG)   /* ��Ƶ�� */
                    {
                        pbuf = framebuf;
                        f_read(videodev.file, pbuf, g_avix.StreamSize + 8, &nr); /* ������֡+��һ������ID��Ϣ */
                        res = mjpeg_decode(pbuf, g_avix.StreamSize);

                        if (res)
                        {
                            printf("decode error!\r\n");
                        }

                        while (aviframeup == 0);    /* �ȴ�ʱ�䵽��(��TIM6���ж���������Ϊ1) */

                        aviframeup = 0;     /* ��־���� */
                        framecnt++;
                    }
                    else if (g_avix.StreamID == AVI_AUDS_FLAG)     /* ��Ƶ�� */
                    {
//                        saisavebuf++;

//                        if (saisavebuf > 3)saisavebuf = 0;

//                        do
//                        {
//                            nr = videodev.saiplaybuf;

//                            if (nr)nr--;
//                            else nr = 3;
//                        } while (saisavebuf == nr); /* ��ײ�ȴ� */

                        f_read(videodev.file, videodev.saibuf[saisavebuf], g_avix.StreamSize + 8, &nr); /* ���videodev.saibuf */
                        pbuf = videodev.saibuf[saisavebuf];
                    }
                }
                else     /* ��ͣ״̬ */
                {
                    delay_ms(1000 / OS_TICKS_PER_SEC);
                }

                if (dirmode == 0)   /* ����ģʽ,�ż�ⴥ���� */
                {
                    tp_dev.scan(0);
                    in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);	/* �õ�������ֵ */

                    for (i = 0; i < 5; i++)
                    {
                        res = btn_check(tbtn[i], &in_obj);

                        if (res && ((tbtn[i]->sta & (1 << 7)) == 0) && (tbtn[i]->sta & (1 << 6)))   /* �а����������ɿ�,����TP�ɿ��˻���TPAD���� */
                        {
                            switch (i)
                            {
                                case 0:/* file list */
                                    playflag = 0xff;
                                    break;

                                case 1:/* ��һ����Ƶ */
                                    if (systemset.videomode != 1)   /* ��������� */
                                    {
                                        if (videodev.curindex)videodev.curindex--;
                                        else videodev.curindex = videodev.mfilenum - 1;
                                    }
                                    else     /* ������� */
                                    {
                                        videodev.curindex = app_get_rand(videodev.mfilenum); /* �õ���һ�׸��������� */
                                    }

                                    playflag = 1;
                                    break;

                                case 3: /* ��һ����Ƶ */
                                    if (systemset.videomode != 1)   /* ��������� */
                                    {
                                        if (videodev.curindex < (videodev.mfilenum - 1))videodev.curindex++;
                                        else videodev.curindex = 0;
                                    }
                                    else     /* ������� */
                                    {
                                        videodev.curindex = app_get_rand(videodev.mfilenum); /* �õ���һ�׸��������� */
                                    }

                                    playflag = 3;
                                    break;

                                case 2: /* ����/��ͣ */
                                    if (videodev.status & (1 << 0))   /* ����ͣ */
                                    {
                                        videodev.status &= ~(1 << 0);   /* �����ͣ */
                                        //SAI_Play_Stop();              /* �ر���Ƶ */
                                        tbtn[2]->picbtnpathd = (uint8_t *)AUDIO_PLAYP_PIC;
                                        tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PAUSER_PIC;
                                    }
                                    else     /* ��ͣ״̬ */
                                    {
                                        videodev.status |= 1 << 0;  /* ȡ����ͣ */
                                        //SAI_Play_Start();         /* ����DMA���� */
                                        tbtn[2]->picbtnpathd = (uint8_t *)AUDIO_PAUSEP_PIC;

                                        tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PLAYR_PIC;
                                    }

                                    break;

                                case 4:/* ֹͣ����,ֱ���˳��������� */
                                    playflag = 4;
                                    break;
                            }
                        }
                    }

                    res = progressbar_check(volprgb, &in_obj); /* ������������� */

                    if (res && lastvolpos != volprgb->curpos)   /* ��������,��λ�ñ仯��.ִ���������� */
                    {
                        lastvolpos = volprgb->curpos;

//                        if (volprgb->curpos)es8388set.mvol = volprgb->curpos; /* �������� */
//                        else es8388set.mvol = 0;

//                        app_es8388_volset(es8388set.mvol);
                        video_show_vol((volprgb->curpos * 100) / volprgb->totallen);	/* ��ʾ�����ٷֱ� */
                    }

                    res = progressbar_check(videoprgb, &in_obj);

                    if (res)   /* ��������,�����ɿ���,ִ�п������ */
                    {
                        videodev.status &= ~(1 << 1); /* ��־����������״̬ */

                        //if (videodev.status & 1 << 0)SAI_Play_Stop(); /* ����ͣʱ,��ʼ�������,�ر���Ƶ */

                        video_seek(&videodev, &g_avix, framebuf, videoprgb->curpos);
                        pbuf = framebuf;
                        video_info_upd(&videodev, videoprgb, volprgb, &g_avix, 0); /* ������ʾ��Ϣ,ÿ100msִ��һ�� */
                    }
                    else if ((videodev.status & (1 << 1)) == 0)     /* ���ڿ��/����״̬? */
                    {
                        //if (videodev.status & 1 << 0)SAI_Play_Start(); /* ����ͣ״̬,������˽���,����Ƶ */

                        videodev.status |= 1 << 1;  /* ȡ���������״̬ */
                    }

                    tcnt++;

                    if ((tcnt % 20) == 0)video_info_upd(&videodev, videoprgb, volprgb, &g_avix, 0); /* ������ʾ��Ϣ,ÿ100msִ��һ�� */
                }
                else     /* ����ģʽ,��ⰴ�� */
                {
                    key = key_scan(0);

                    switch (key)
                    {
                        case WKUP_PRES:/* KEY1������ */
                            if (systemset.videomode != 1)   /* ��������� */
                            {
                                if (videodev.curindex < (videodev.mfilenum - 1))videodev.curindex++;
                                else videodev.curindex = 0;
                            }
                            else    /* ������� */
                            {
                                videodev.curindex = app_get_rand(videodev.mfilenum); /* �õ���һ�׸��������� */
                            }

                            playflag = 3; /* ������һ����Ƶ */
                            break;

                        case KEY1_PRES:/* KEY1������ */
                            if (systemset.videomode != 1)   /* ��������� */
                            {
                                if (videodev.curindex)videodev.curindex--;
                                else videodev.curindex = videodev.mfilenum - 1;
                            }
                            else        /* ������� */
                            {
                                videodev.curindex = app_get_rand(videodev.mfilenum); /* �õ���һ�׸��������� */
                            }

                            playflag = 1; /* ������һ����Ƶ */
                            break;

//                        case WKUP_PRES: /* WAKE UP���� */
//                            SAI_Play_Stop();/* �ر���Ƶ */
//                            video_seek_key(videodev.file, &g_avix, framebuf);
//                            pbuf = framebuf;
//                            SAI_Play_Start();/* ����DMA���� */
//                            break;

                    }
                }

                if (system_task_return)  	/* TPAD���أ�����Ŀ¼ */
                {
                    playflag = 0XFF;
                    break;
                }

                if (videodev.status & (1 << 0) && videodev.status & (1 << 1))  	/* ����ͣ״̬�Ϳ������״̬ */
                {
                    if (avi_get_streaminfo(pbuf + g_avix.StreamSize))  	/* ��ȡ��һ֡ ����־,�������,�����ǵ�Ӱ������ */
                    {
                        pbuf = framebuf;
                        res = f_read(videodev.file, pbuf, AVI_VIDEO_BUF_SIZE, &nr);	/* ��ʼ��ȡ */

                        if (res == 0 && nr == AVI_VIDEO_BUF_SIZE)  	/* ��ȡ�ɹ�,�Ҷ�ȡ��ָ�����ȵ����� */
                        {
                            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "00dc"); /* Ѱ��AVI_VIDS_FLAG,00dc */
                            avi_get_streaminfo(pbuf + offset);	/* ��ȡ����Ϣ */

                            if (offset)f_lseek(videodev.file, (videodev.file->fptr - AVI_VIDEO_BUF_SIZE) + offset + 8);
                        }
                        else
                        {
                            if (nr == 0)            /* �����ļ�ĩβ��,�������� */
                            {
                                playflag = 0XFE;    /* ���Ŵ���������һ����Ƶ */
                                break;              /* ��Ӱ������ */
                            }
                            else                    /* �ļ���û��ĩβ,����������Ƶ֡ */
                            {
                                //printf("shit:%x\r\n",g_avix.StreamID);
                                //printf("shit1:%d\r\n",g_avix.StreamSize);
                                //SAI_Play_Stop();  /* �ر���Ƶ */
                                if (video_seek_vids(&videodev, &g_avix, framebuf))    /* ������һ����Ƶ֡ */
                                {
                                    printf("shit:%x\r\n",g_avix.StreamID);
                                    printf("shit1:%d\r\n",g_avix.StreamSize);
                                    playflag = 0XFE;/* ���Ŵ���������һ����Ƶ */
                                    break;          /* ��Ӱ������ */
                                }

                                //SAI_Play_Start(); /* ����DMA���� */
                            }
                        }
                    }
                }

                if (nr == 0)break;
            }

            if (playflag == 0XFE)playflag = 0; /* ������һ����Ƶ */

            memshow_flag = 1;   /* ��ӡ�ڴ�ʹ���� */
            //SAI_Play_Stop();  /* �ر���Ƶ */
            TIM6->CR1 &= ~(1 << 0); /* �رն�ʱ��6 */
            TIM8->CR1 &= ~(1 << 0); /* �رն�ʱ��8 */
            mjpeg_free();       /* �ͷ��ڴ� */
            f_close(videodev.file);

            if (dirmode)        /* ֮ǰ�Ǻ��� */
            {
                dirmode = 0;    /* �ָ�����ģʽ */
                lcd_display_dir(0);     /* ��Ϊ���� */
                tp_dev.touchtype = 0;   /* ����Ҳ�ĳ����� */
                video_load_ui();        /* ���������� */
                videoheight = lcddev.height - (vui->tpbar_height + vui->msgbar_height + vui->prgbar_height + vui->btnbar_height);
                yoff = vui->tpbar_height + vui->msgbar_height;
                videoprgb->inbkcolora = 0x738E;     /* Ĭ��ɫ */
                videoprgb->inbkcolorb = AUDIO_INFO_COLOR;   /* Ĭ��ɫ */
                videoprgb->infcolora = 0X75D;       /* Ĭ��ɫ */
                videoprgb->infcolorb = 0X596;       /* Ĭ��ɫ */
                volprgb->inbkcolora = AUDIO_INFO_COLOR;     /* Ĭ��ɫ */
                volprgb->inbkcolorb = AUDIO_INFO_COLOR;     /* Ĭ��ɫ */
                volprgb->infcolora = 0X75D;         /* Ĭ��ɫ */
                volprgb->infcolorb = 0X596;         /* Ĭ��ɫ */

                for (i = 0; i < 5; i++)btn_draw(tbtn[i]);   /* ����ť */

                tbtn[2]->picbtnpathu = (uint8_t *)AUDIO_PLAYR_PIC; /* ����һ��֮���Ϊ�����ɿ�״̬ */
                progressbar_draw_progressbar(videoprgb);    /* �������� */
                progressbar_draw_progressbar(volprgb);      /* �������� */
            }

            if (playflag == 0)
            {
                if (systemset.videomode == 0)   /* ˳�򲥷� */
                {
                    if (videodev.curindex < (videodev.mfilenum - 1))videodev.curindex++;
                    else videodev.curindex = 0;
                }
                else if (systemset.videomode == 1)     /* ������� */
                {
                    videodev.curindex = app_get_rand(videodev.mfilenum); /* �õ���һ�׸��������� */
                }
                else videodev.curindex = videodev.curindex;   /* ����ѭ�� */
            }
            else if (playflag == 0XFF)
            {
                res = 0;
                break;
            }
            else if (playflag == 4)
            {
                res = 0XFF;
                break;
            }
        }
    }

    gui_memin_free(videodev.saibuf[0]);
    gui_memin_free(videodev.saibuf[1]);
    gui_memin_free(videodev.saibuf[2]);
    gui_memin_free(videodev.saibuf[3]);
    gui_memin_free(framebuf);
    gui_memin_free(videoinfo);
    gui_memin_free(videodev.file);
    gui_memin_free(vui);

    for (i = 0; i < 5; i++)if (tbtn[i])btn_delete(tbtn[i]); /* ɾ����ť */

    if (videoprgb)progressbar_delete(videoprgb);

    if (volprgb)progressbar_delete(volprgb);

//    ES8388_ADDA_Cfg(0,0);   /* �ر�DAC&ADC */
//    ES8388_Input_Cfg(0);    /* �ر�����ͨ�� */
//    ES8388_Output_Cfg(0,0); /* �ر�DAC��� */
//    app_es8388_volset(0);   /* �ر�ES8388������� */
    slcd_dma_init();            /* Ӳ��JPEG����,������dma2stream0,���Ա������³�ʼ�� */
    return res;
}

/**
 * @brief       avi ��Ƶ��־����,���ڳ���������Ƶ֡
 * @param       videodevx       : video ������
 * @param       aviinfo         : avi�ļ���Ϣ
 * @param       mbuf            : ���ݻ�����
 * @retval      0, �ɹ�;  ����, ʧ��;
 */
uint8_t video_seek_vids(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos;
    uint8_t *pbuf;
    uint32_t br;
    uint8_t res = 0;

    while (1)
    {
        fpos = videodevx->file->fptr;       /* ���浱ǰλ�� */
        res = f_read(videodevx->file, mbuf, AVI_VIDEO_BUF_SIZE, &br);   /* ������֡+��һ������ID��Ϣ */

        if (res || br != AVI_VIDEO_BUF_SIZE)
        {
            if (br != AVI_VIDEO_BUF_SIZE)res = 0XFF; /* ��ǳ��� */

            break;
        }

        pbuf = mbuf;
        br = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG);   /* Ѱ����Ƶ֡ */

        if (br != 0XFFFFFFFF)
        {
            avi_get_streaminfo(pbuf + br);  /* ��ȡ����Ϣ */

            if (g_avix.StreamSize < 100 || g_avix.StreamSize > AVI_VIDEO_BUF_SIZE)continue; /* �������Ƶ֡�ߴ� */

            f_lseek(videodevx->file, fpos + br + 8);    /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */
            break;
        }
    }

    return res;
}

/**
 * @brief       avi�ļ�����
 * @param       videodevx       : video ������
 * @param       aviinfo         : avi�ļ���Ϣ
 * @param       mbuf            : ���ݻ�����
 * @param       dstpos          : Ŀ��λ��
 * @retval      0, �ɹ�;  ����, ʧ��;
 */
uint8_t video_seek(__videodev *videodevx, AVI_INFO *aviinfo, uint8_t *mbuf, uint32_t dstpos)
{
    uint32_t fpos = dstpos;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;

    f_lseek(videodevx->file, fpos); /* /ƫ�Ƶ�Ŀ��λ�� */
    f_read(videodevx->file, mbuf, AVI_VIDEO_BUF_SIZE, &br); /* ������֡+��һ������ID��Ϣ */
    pbuf = mbuf;

    if (fpos == 0)   /* ��0��ʼ,����Ѱ��movi ID */
    {
        offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
    }
    else offset = 0;

    offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, aviinfo->VideoFLAG); /* Ѱ����Ƶ֡ */
    avi_get_streaminfo(pbuf + offset);              /* ��ȡ����Ϣ */
    f_lseek(videodevx->file, fpos + offset + 8);    /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */

    if (aviinfo->StreamID == AVI_VIDS_FLAG)
    {
        f_read(videodevx->file, mbuf, aviinfo->StreamSize + 8, &br);    /* ������֡ */
        mjpeg_decode(mbuf, g_avix.StreamSize);      /* ��ʾ��Ƶ֡ */
    }

    return 0;
}

/**
 * @brief       avi�ļ�����
 * @param       favi            : avi�ļ�
 * @param       aviinfo         : avi�ļ���Ϣ
 * @param       mbuf            : ���ݻ�����
 * @retval      0, �ɹ�;  ����, ʧ��;
 */
uint8_t video_seek_key(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf)
{
    uint32_t fpos = favi->fptr;
    uint8_t *pbuf;
    uint16_t offset;
    uint32_t br;
    uint32_t delta;
    uint32_t totsec;
    uint8_t key;
    totsec = (aviinfo->SecPerFrame / 1000) * aviinfo->TotalFrame;
    totsec /= 1000; /* ������ */
    delta = (favi->obj.objsize / totsec) * 5; /* ÿ��ǰ��5���ӵ������� */

    while (1)
    {
        key = key_scan(1);

        if (key == WKUP_PRES)   /* ��� */
        {
            if (fpos < favi->obj.objsize)fpos += delta;

            if (fpos > (favi->obj.objsize - AVI_VIDEO_BUF_SIZE))
            {
                fpos = favi->obj.objsize - AVI_VIDEO_BUF_SIZE;
            }
        }

//        else if (key == KEY2_PRES)  /* ���� */
//        {
//            if (fpos > delta)fpos -= delta;
//            else fpos = 0;
//        }
        else if (g_avix.StreamID == AVI_VIDS_FLAG)break;

        f_lseek(favi, fpos);
        f_read(favi, mbuf, AVI_VIDEO_BUF_SIZE, &br);    /* ������֡+��һ������ID��Ϣ */
        pbuf = mbuf;

        if (fpos == 0)   /* ��0��ʼ,����Ѱ��movi ID */
        {
            offset = avi_srarch_id(pbuf, AVI_VIDEO_BUF_SIZE, "movi");
        }
        else offset = 0;

        offset += avi_srarch_id(pbuf + offset, AVI_VIDEO_BUF_SIZE, g_avix.VideoFLAG);	/* Ѱ����Ƶ֡ */
        avi_get_streaminfo(pbuf + offset);  /* ��ȡ����Ϣ */
        f_lseek(favi, fpos + offset + 8);   /* ������־ID,����ַƫ�Ƶ������ݿ�ʼ�� */

        if (g_avix.StreamID == AVI_VIDS_FLAG)
        {
            f_read(favi, mbuf, g_avix.StreamSize + 8, &br);   /* ������֡ */
            mjpeg_decode(mbuf, g_avix.StreamSize);            /* ��ʾ��Ƶ֡ */
        }
        else
        {
            printf("error flag");
        }

        delay_ms(5);/* ��ʱ5ms */
    }

    return 0;
}





















