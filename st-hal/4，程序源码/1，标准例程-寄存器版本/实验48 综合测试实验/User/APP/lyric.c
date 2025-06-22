/**
 ****************************************************************************************************
 * @file        lyric.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       APP-�����ʾʵ�� ����
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
 * V1.1 20221028
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�uint8_t/uint16_t/uint32_tΪuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "lyric.h"


/**
 * @brief       �Ѻ�׺����Ϊsfx
 * @param       name            : �ļ���(����Ϊx.x�ṹ)
 * @param       sfx             : ��׺
 * @retval      ��
 */
void lrc_chg_suffix(uint8_t *name, uint8_t *sfx)
{
    while (*name != '\0')name++;    /* �ҵ���β */

    while (*name != '.')name--;     /* �ҵ�'.' */

    *(++name) = sfx[0];
    *(++name) = sfx[1];
    *(++name) = sfx[2];
    *(++name) = '\0';               /* ��ӽ����� */
}

/**
 * @brief       ������ʽṹ��
 * @param       ��
 * @retval      ��
 */
_lyric_obj *lrc_creat(void)
{
    _lyric_obj *lrc_crt;
    lrc_crt = (_lyric_obj *)gui_memin_malloc(sizeof(_lyric_obj));   /* �����ڴ� */

    if (lrc_crt == NULL)return NULL;/* �ڴ���䲻�� */

    gui_memset((uint8_t *)lrc_crt, 0, sizeof(_lyric_obj));  /* ���� */
    lrc_crt->flrc = (FIL *)gui_memin_malloc(sizeof(FIL));   /* �����ڴ� */

    if (lrc_crt->flrc == NULL)      /* �ڴ���䲻�� */
    {
        gui_memin_free(lrc_crt);    /* ɾ��ǰ������� */
    }

    gui_memset((uint8_t *)lrc_crt->flrc, 0, sizeof(FIL));   /* ���� */
    return lrc_crt;
}

/**
 * @brief       �ͷ�LRC��������ڴ�
 * @param       ��
 * @retval      ��
 */
void lrc_delete(_lyric_obj *lcrdelete)
{
    gui_memin_free(lcrdelete->flrc);/* ɾ���ļ��ṹ�� */
    gui_memin_free(lcrdelete);
}

/**
 * @brief       ��2���ַ�תΪ����
 * @param       str             : �ַ���(��':'/'.'/'\0'����,��ֻ��������/':'/'.')
 * @retval      ת����Ľ��
 */
uint8_t lrc_str2num(uint8_t *str)
{
    uint8_t chr;

    if (*str == ':' || *str == '.' || *str == '\0')return 0; /* ��һ�����ݾ���:/.,��������� */

    chr = *str;
    str++;

    if (*str == ':' || *str == '.' || *str == '\0')
    {
        return chr - '0';   /* ֻ��һ������λ */
    }

    return (chr - '0') * 10 + (*str - '0'); /* 2λ���� */
}

/**
 * @brief       ���TAG����
 * @param       lrcx            : ��ʽṹ��
 * @param       lrcpos          : ��ǰstring����ʼ��ַ
 * @param       str             : ��ǰstring
 * @retval      ��
 */
void lrc_analyze(_lyric_obj *lrcx, uint16_t lrcpos, uint8_t *str)
{
    uint8_t *p;
    uint8_t i = 0;
    uint16_t lrcoffset;
    uint8_t sta = 0;    /* ���λ��ʾ�Ƿ���ʱ��TAG */
                        /* bit1:0,����TAG��;1,����TAG�� */
                        /* bit0:0,ʱ��TAG��ȷ;1,ʱ��TAG���� */
    uint8_t tagbuf[9];  /* tag�Ϊ9���ֽ� */
    uint32_t pstime = 0;/* 10���� */

    lrcoffset = 0;
    p = str;

    while (1)
    {
        if (*p == '[')sta |= 1 << 0;
        else if (*p == ']')sta = 0;
        else if (sta == 0 && *p != ' ')break; /* ��tag�ⷢ���˷ǿո���ַ� */

        lrcoffset++;    /* ͳ�Ƹ�ʿ�ʼ�ĵط� */
        *p++;
    }

    if (lrcoffset == 0)return;  /* ���û��TAG����,ֱ���˳� */

    sta = 0;

    while (1)
    {
        if (*str == '[')
        {
            sta |= 1 << 7;      /* Ĭ�ϱ������һ��ʱ��ͷ */
            sta |= 1 << 1;      /* �����TAG���� */
            i = 0;
        }
        else if (*str == ']')   /* �ҵ���һ��TAG�Ľ�β */
        {
            if (sta & (1 << 7)) /* ��һ���Ϸ���ʱ��TAG */
            {
                tagbuf[i] = '\0'; /* ��ĩβ��ӽ����� */
                p = tagbuf;
                pstime = (uint32_t)lrc_str2num(p) * 6000; /* һ������6000��10ms */

                while (*p != ':' && *p != '.' && *p != '\0')p++; /* ƫ�Ƶ���һ��λ�� */

                if (*p != 0) /* ��û�н��� */
                {
                    p++;
                    pstime += (uint32_t)lrc_str2num(p) * 100; /* һ������100��10ms */

                    while (*p != ':' && *p != '.' && *p != '\0')p++;        /* ƫ�Ƶ���һ��λ�� */

                    if (*p != '\0') /* ��Ȼû�н��� */
                    {
                        p++;
                        pstime += (uint32_t)lrc_str2num(p);                 /* ����10msΪ��λ������ */
                    }

                    lrcx->time_tbl[lrcx->indexsize] = pstime;               /* ��¼ʱ��:��λ,10ms */
                    lrcx->addr_tbl[lrcx->indexsize] = lrcpos + lrcoffset;   /* ��¼��ַ */
                    lrcx->indexsize++;                                      /* ������1 */
                }
            }

            sta = 0;    /* ��ǽ���һ�β��� */
            i = 0;
        }
        else if (*str != ' ')       /* �ǿո� */
        {
            if (sta & (1 << 1))     /* ��TAG�ڲ� */
            {
                if (sta & (1 << 7)) /* ��һ��ʱ��TAG */
                {
                    if ((*str <= '9' && *str >= '0') || *str == ':' || *str == '.')
                    {
                        if (i > 8)sta &= ~(1 << 7); /* �����TAG,̫���� */
                        else
                        {
                            tagbuf[i] = *str;   /* ��¼���� */
                            i++;
                        }
                    }
                    else sta &= ~(1 << 7);      /* ����ʱ��TAG */
                }
            }
            else break;     /* ������TAG��ķǿո�,��ʾ������ */
        }

        if (*str == '\0')break; /* �ǽ����� */

        str++;
    }
}

/**
 * @brief       �������
 *  note        ����С�򵽵�ʱ��˳������
 * @param       lrcx            : ��ʽṹ��
 * @retval      ��
 */
void lrc_sequence(_lyric_obj *lrcx)
{
    uint16_t i, j;
    uint16_t temp;

    if (lrcx->indexsize == 0)return; /* û������,ֱ�Ӳ������� */

    for (i = 0; i < lrcx->indexsize - 1; i++) /* ���� */
    {
        for (j = i + 1; j < lrcx->indexsize; j++)
        {
            if (lrcx->time_tbl[i] > lrcx->time_tbl[j]) /* �������� */
            {
                temp = lrcx->time_tbl[i];
                lrcx->time_tbl[i] = lrcx->time_tbl[j];
                lrcx->time_tbl[j] = temp;

                temp = lrcx->addr_tbl[i];
                lrcx->addr_tbl[i] = lrcx->addr_tbl[j];
                lrcx->addr_tbl[j] = temp;
            }
        }
    }
}

/**
 * @brief       ��ȡ���
 * @param       lrcx            : ��ʽṹ��
 * @param       path            : ·������
 * @param       name            : MP3����������
 * @retval      0, ��LRC,��ȡ����;
 *              ����, ����;
 */
uint8_t lrc_read(_lyric_obj *lrcx, uint8_t *path, uint8_t *name)
{
    uint8_t *pname;
    uint16_t fpos;
    uint8_t *p;
    uint8_t res = 0;
    /* ���֮ǰ������ */
    lrcx->indexsize = 0;
    lrcx->curindex = 0;
    lrcx->oldostime = 0;
    lrcx->curtime = 0;
    lrcx->detatime = 0;
    lrcx->updatetime = 0;
    lrcx->namelen = 0;
    lrcx->curnamepos = 0;

    pname = gui_memin_malloc(strlen((const char *)path) + strlen((const char *)name) + 2); /* �����ڴ� */

    if (pname == NULL)res = 1; /* ����ʧ�� */

    if (res == 0)
    {
        pname = gui_path_name(pname, path, name);   /* �ļ�������·�� */

        if (pname)
        {
            lrc_chg_suffix(pname, "lrc");           /* �޸ĺ�׺Ϊ.lrc */
            res = f_open(lrcx->flrc, (const TCHAR *)pname, FA_READ); /* ���ļ� */

            if (res == 0)
            {
                while (1)
                {
                    fpos = f_tell(lrcx->flrc);      /* �õ�Ƭƫ��ǰ��λ�� */
                    p = (uint8_t *)f_gets((TCHAR *)lrcx->buf, LYRIC_BUFFER_SIZE, lrcx->flrc); /* ��ȡ��� */

                    if (p == 0)break;               /* ���������� */
                    else
                    {
                        lrc_analyze(lrcx, fpos, p);
                    }
                }

                lrc_sequence(lrcx);
            }
        }
    }

    gui_memset((uint8_t *)lrcx->buf, 0, LYRIC_BUFFER_SIZE); /* �������������� */
    gui_memin_free(pname);
    return res;
}

/**
 * @brief       ��ʾһ�и��
 * @param       lrcx            : ��ʽṹ��
 * @param       x,y,width,height: ����,��height=0ʱ,����ʾ���
 * @retval      ��
 */
void lrc_show_linelrc(_lyric_obj *lrcx, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint8_t *p;
    f_lseek(lrcx->flrc, lrcx->addr_tbl[lrcx->curindex]);
    p = (uint8_t *)f_gets((TCHAR *)lrcx->buf, LYRIC_BUFFER_SIZE, lrcx->flrc);   /* ��ȡ��� */

    if (p)
    {
        lrcx->namelen = strlen((const char *)p) * (lrcx->font / 2);             /* ��������ռ���� */

        //gui_phy.back_color=lrcx->bkcolor;                         /* ���ñ���ɫ */
        if (height)gui_show_strmid(x, y, width, height, lrcx->color, lrcx->font, p); /* ��ʾ��� */

        lrcx->namelen = strlen((const char *)p) * lrcx->font / 2;   /* �õ���������ռ��ĳ��� */
        lrcx->curnamepos = 0;
        lrcx->detatime = 0;	/* ʱ������ */
    }
}

/**
 * @brief       ��ʲ���
 * @param       path            : ·��
 * @param       name            : ����
 * @retval      ���Խ��
 */
uint8_t lrc_test(uint8_t *path, uint8_t *name)
{
    _lyric_obj *lrcx;
    uint8_t i;
    uint8_t *p;
    uint8_t res = 0;
    lrcx = lrc_creat(); /* ���� */

    if (lrcx)
    {
        res = lrc_read(lrcx, path, name);

        if (res == 0)
        {
            printf("�������:%d\r\n", lrcx->indexsize);

            for (i = 0; i < lrcx->indexsize; i++)
            {
                printf("%3d ʱ��:%5d ", i, lrcx->time_tbl[i]);
                f_lseek(lrcx->flrc, lrcx->addr_tbl[i]);
                p = (uint8_t *)f_gets((TCHAR *)lrcx->buf, LYRIC_BUFFER_SIZE, lrcx->flrc); /* ��ȡ��� */
                printf("���:%s\r\n", p);
            }
        }
    }

    lrc_delete(lrcx);
    return res;
}



























