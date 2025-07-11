/**
 ****************************************************************************************************
 * @file        phoneplay.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-07
 * @brief       APP-拨号器 代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.1 20220607
 * 1, 修改注释方式
 * 2, 修改u8/u16/u32为uint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "phoneplay.h"
#include "spb.h"
#include "camera.h"
//#include "sai.h"
//#include "audioplay.h"
#include "./BSP/USART3/usart3.h"


/******************************************************************************************/
/* 来电任务设置 */

#define PHONE_INCALL_TASK_PRIO      4       /* 设置任务优先级 */
#define PHONE_INCALL_STK_SIZE       512     /* 设置任务堆栈大小 */

OS_STK *PHONE_INCALL_TASK_STK = 0;          /* 任务堆栈，采用内存管理的方式控制申请 */

void phone_incall_task(void *pdata);        /* 任务函数 */

/******************************************************************************************/

/* 打电话图片 */
uint8_t *const PHONE_CALL_PIC[3] =
{
    "1:/SYSTEM/APP/PHONE/call_28.bmp",
    "1:/SYSTEM/APP/PHONE/call_36.bmp",
    "1:/SYSTEM/APP/PHONE/call_54.bmp",
};

/* 挂电话图片 */
uint8_t *const PHONE_HANGUP_PIC[3] =
{
    "1:/SYSTEM/APP/PHONE/hangup_28.bmp",
    "1:/SYSTEM/APP/PHONE/hangup_36.bmp",
    "1:/SYSTEM/APP/PHONE/hangup_54.bmp",
};

/* 退格图片 */
uint8_t *const PHONE_DEL_PIC[3] =
{
    "1:/SYSTEM/APP/PHONE/del_28.bmp",
    "1:/SYSTEM/APP/PHONE/del_36.bmp",
    "1:/SYSTEM/APP/PHONE/del_54.bmp",
};

/* 电话提示信息 */
uint8_t *const phone_remind_tbl[4][GUI_LANGUAGE_NUM] =
{
    "拨号中...", "撥號中...", "Dialing...",
    "来电...", "来电...", "Calling...",
    "未找到GSM模块,请检查!", "未找到GSM模塊,請檢查!", "GSM Module Error!Pleas Check!",
    "SIM卡异常,请检查!", "SIM卡異常,請檢查!", "SIM Card Error!Pleas Check!",
};

/* 电话接听/拒绝按钮标题 */
uint8_t *const phone_btncaption_tbl[2][GUI_LANGUAGE_NUM] =
{
    "接听", "接聽", "ANSWER",
    "拒绝", "拒絕", "REFUSE",
};

/* 键盘名称表 */
uint8_t *const call_btnstr_tbl[14] =
{
    "1", "2", "3",
    "4", "5", "6",
    "7", "8", "9",
    "*", "0", "#",
    "call", "del",
};

/**
 * @brief       显示区域清屏
 * @param       pdis            : 显示结构体
 * @retval      无
 */
void phone_show_clear(_phonedis_obj *pdis)
{
    gui_fill_rectangle(pdis->xoff, pdis->yoff, pdis->width, pdis->height, BLACK); /* 清除原来的显示 */
}

/**
 * @brief       显示通话时间
 * @param       pdis            : 显示结构体
 * @retval      无
 */
void phone_show_time(_phonedis_obj *pdis)
{
    uint8_t *pbuf;
    uint16_t yoff;
    uint16_t xoff;
    pbuf = gui_memin_malloc(100);

    if (pbuf == 0)return ;

    sprintf((char *)pbuf, "%03d:%02d", pdis->time / 60, pdis->time % 60);
    gui_phy.back_color = BLACK;
    xoff = pdis->xoff + pdis->width / 2 - 5 * pdis->tsize / 2;
    yoff = pdis->yoff + pdis->height / 2 - pdis->tsize / 2;
    gui_fill_rectangle(xoff, yoff, 5 * pdis->tsize, pdis->tsize, BLACK); /* 清除原来的显示 */
    gui_show_strmid(xoff, yoff, 5 * pdis->tsize, pdis->tsize, WHITE, pdis->tsize, pbuf);
    gui_memin_free(pbuf);/* 释放内存 */
}

/**
 * @brief       显示呼叫中
 * @param       pdis            : 显示结构体
 * @param       gsmx            : GSM状态结构体
 * @retval      无
 */
void phone_show_calling(_phonedis_obj *pdis, __gsmdev *gsmx)
{
    uint16_t yoff;
    uint16_t xoff;
    gui_phy.back_color = BLACK;
    xoff = pdis->xoff + pdis->width / 2 - 5 * pdis->tsize / 2;
    yoff = pdis->yoff + pdis->height / 2 - pdis->tsize / 2;
    gui_fill_rectangle(xoff, yoff, 5 * pdis->tsize, pdis->tsize, BLACK); /* 清除原来的显示 */

    if (gsmx->mode == 3)gui_show_strmid(xoff, yoff, 5 * pdis->tsize, pdis->tsize, WHITE, pdis->tsize, phone_remind_tbl[1][gui_phy.language]);
    else gui_show_strmid(xoff, yoff, 5 * pdis->tsize, pdis->tsize, WHITE, pdis->tsize, phone_remind_tbl[0][gui_phy.language]);
}

/**
 * @brief       显示电话号码
 * @param       pdis            : 显示结构体
 * @param       gsmx            : GSM状态结构体
 * @retval      无
 */
void phone_show_phone(_phonedis_obj *pdis, __gsmdev *gsmx)
{
    uint16_t yoff;
    uint16_t xoff;
    uint8_t maxnum;
    maxnum = pdis->width / (pdis->psize / 2);

    if (pdis->plen > maxnum)maxnum = pdis->plen - maxnum;
    else maxnum = 0;

    if (gsmx->mode == 0) /* 号码输入模式,仅仅存在电话号码显示 */
    {
        xoff = pdis->xoff;
        yoff = pdis->yoff + pdis->height / 2 - pdis->psize / 2;
        gui_fill_rectangle(xoff, yoff, pdis->width, pdis->psize, BLACK); /* 清除原来的显示 */
        gui_show_strmid(xoff, yoff, pdis->width, pdis->psize, WHITE, pdis->psize, pdis->phonebuf + maxnum); /* 显示电话号码 */
    }
    else
    {
        xoff = pdis->xoff;
        yoff = pdis->yoff + pdis->height / 2 - pdis->tsize / 2 - pdis->psize;
        gui_fill_rectangle(xoff, yoff, pdis->width, pdis->psize, BLACK); /* 清除原来的显示 */
        gui_show_strmid(xoff, yoff, pdis->width, pdis->psize, WHITE, pdis->psize, pdis->phonebuf + maxnum); /* 显示电话号码 */
    }
}

/**
 * @brief       显示输入字符串
 * @param       pdis            : 显示结构体
 * @param       gsmx            : GSM状态结构体
 * @retval      无
 */
void phone_show_input(_phonedis_obj *pdis, __gsmdev *gsmx)
{
    uint16_t yoff;
    uint16_t xoff;
    uint8_t maxnum;

    if (gsmx->mode == 2) /* 仅在电话接通后有效 */
    {
        maxnum = pdis->width / (pdis->psize / 2);

        if (pdis->inlen > maxnum)maxnum = pdis->inlen - maxnum;
        else maxnum = 0;

        xoff = pdis->xoff;
        yoff = pdis->yoff + pdis->height / 2 + pdis->tsize;
        gui_fill_rectangle(xoff, yoff, pdis->width, pdis->psize, BLACK); /* 清除原来的显示 */
        gui_show_strmid(xoff, yoff, pdis->width, pdis->psize, WHITE, pdis->psize, pdis->inbuf + maxnum); /* 显示输入内容 */
    }
}

/**
 * @brief       电话提示音
 * @param       无
 * @retval      无
 */
void phone_ring(void)
{
    uint8_t i;

    for (i = 0; i < 2; i++)
    {
        BEEP(1);
        delay_ms(100);
        BEEP(0);
        delay_ms(50);
    }
}

/**
 * @brief       来电状态显示时间
 * @param       x,y,width,height: 坐标和大小
 * @param       c               : 背景颜色 
 * @param       pdis            : 显示结构体
 * @retval      无
 */
void phone_incall_show_time(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t c, _phonedis_obj *pdis)
{
    uint8_t *pbuf;
    uint16_t yoff;
    uint16_t xoff;
    pbuf = gui_memin_malloc(100);

    if (pbuf == 0)return ;

    sprintf((char *)pbuf, "%03d:%02d", pdis->time / 60, pdis->time % 60);
    gui_phy.back_color = c;
    xoff = (width - 3 * pdis->tsize) / 2;
    yoff = (height - pdis->tsize) / 2;
    gui_show_ptstr(x + xoff, y + yoff, x + width, y + height, 0, WHITE, pdis->tsize, pbuf, 0);
    gui_memin_free(pbuf);/* 释放内存 */
}

/**
 * @brief       来电任务,专门处理电话接入.通过其他程序创建
 * @param       pdata           : 未用到的输入参数
 * @retval      无
 */
void phone_incall_task(void *pdata)
{
    uint8_t rval = 0, res;
    _window_obj *twin = 0;  /* 窗体 */
    _btn_obj *rbtn = 0;     /* 取消按钮 */
    _btn_obj *okbtn = 0;    /* 确定按钮 */
    uint16_t calltime = 0;
    uint32_t curtime = 0;
    uint8_t *pbuf;
    uint8_t camflag = 0;    /* 摄像头工作标志 */
    OSTaskSuspend(6);       /* 挂起主任务 */

    if (EXTI->IMR & (1 << 8))   /* 摄像头正在工作? */
    {
        camflag = 1;
        EXTI->IMR &= ~(1 << 8); /* 关闭line8中断 */
        lcd_scan_dir(DFT_SCAN_DIR); /* 恢复默认扫描方向 */
    }

    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* 设置为全屏窗口 */
    twin = window_creat((lcddev.width - 150) / 2, (lcddev.height - 60) / 2, 150, 60, 0, 1 | 1 << 5 | 1 << 6, 16);   /* 创建窗口 */
    okbtn = btn_creat((lcddev.width - 150) / 2 + 15, (lcddev.height - 60) / 2 + 30, 50, 24, 0, 0x02);               /* 创建按钮 */
    rbtn = btn_creat((lcddev.width - 150) / 2 + 15 + 50 + 20, (lcddev.height - 60) / 2 + 30, 50, 24, 0, 0x02);      /* 创建按钮 */
    pbuf = gui_memin_malloc(100);

    if (!twin || !rbtn || !okbtn || !pbuf || rval)rval = 1;
    else
    {
        /* 窗口的名字和背景色 */
        strcpy((char *)pbuf, (char *)gsmdev.incallnum); /* 拷贝号码到pbuf里面 */
        strcat((char *)pbuf, (char *)phone_remind_tbl[1][gui_phy.language]);
        twin->caption = pbuf;
        twin->font = 12;            /* 采用12号字体 */
        twin->captionheight = 24;   /* 修改caption高度为24 */
        twin->windowbkc = APP_WIN_BACK_COLOR;
        
        /* 返回按钮的颜色 */
        rbtn->bkctbl[0] = 0XFA69;   /* 边框颜色 */
        rbtn->bkctbl[1] = 0XFA69;   /* 第一行的颜色 */
        rbtn->bkctbl[2] = 0XFA69;   /* 上半部分颜色 */
        rbtn->bkctbl[3] = 0XFBEF;   /* 下半部分颜色 */

        okbtn->bkctbl[0] = 0X3DEA;  /* 边框颜色 */
        okbtn->bkctbl[1] = 0X3DEA;  /* 第一行的颜色 */
        okbtn->bkctbl[2] = 0X3DEA;  /* 上半部分颜色 */
        okbtn->bkctbl[3] = 0X76B2;  /* 下半部分颜色 */

        rbtn->caption = (uint8_t *)phone_btncaption_tbl[1][gui_phy.language];   /* 标题为拒绝 */
        okbtn->caption = (uint8_t *)phone_btncaption_tbl[0][gui_phy.language];  /* 标题为接听 */
        window_draw(twin);          /* 画出窗体 */
        btn_draw(rbtn);             /* 画按钮 */
        btn_draw(okbtn);            /* 画按钮 */

        while (gsmdev.mode != 0)    /* 非拨号模式 */
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
            delay_ms(1000 / OS_TICKS_PER_SEC);      /* 延时一个时钟节拍 */

            if (system_task_return)break;           /* TPAD返回,即拒绝来电 */

            if (g_usart3_rx_sta & 0X8000)           /* 接收到数据 */
            {
                if (gsm_check_cmd("NO CARRIER"))gsmdev.mode = 0;    /* 拨号失败 */

                if (gsm_check_cmd("NO ANSWER"))gsmdev.mode = 0;     /* 拨号失败 */

                if (gsm_check_cmd("ERROR"))gsmdev.mode = 0;         /* 拨号失败 */

                g_usart3_rx_sta = 0;
            }

            if (gsmdev.mode == 2) /* 通话中 */
            {
                if ((OSTime - curtime) >= OS_TICKS_PER_SEC)         /* 超过1秒钟了 */
                {
                    curtime = OSTime;
                    calltime++;
                    sprintf((char *)pbuf, "%02d:%02d", calltime / 60, calltime % 60);
                    gui_fill_rectangle((lcddev.width - 150) / 2 + 15, (lcddev.height - 60) / 2 + 30, 50, 24, twin->windowbkc);
                    gui_show_strmid((lcddev.width - 150) / 2 + 15, (lcddev.height - 60) / 2 + 30, 50, 24, BLUE, 16, pbuf);
                }
            }

            res = btn_check(rbtn, &in_obj);         /* 拒绝按钮检测 */

            if (res && ((rbtn->sta & 0X80) == 0))   /* 有有效操作 */
            {
                gsmdev.mode = 0;    /* 进入拨号模式 */
                break;
            }

            if (gsmdev.mode != 2)
            {
                res = btn_check(okbtn, &in_obj);    /* 接听按钮检测 */

                if (res && (okbtn->sta & 0X80) == 0)    /* 来电中... */
                {
                    gsmdev.mode = 2;    /* 通话中 */
                    gsm_send_cmd("ATA", "OK", 200);     /* 接听电话 */
                    gsm_cmd_over();     /* 处理完毕 */
                    curtime = OSTime;   /* 记录刚刚建立通话时的时间 */
                    calltime = 0;
                    strcpy((char *)pbuf, (char *)gsmdev.incallnum); /* 拷贝号码到pbuf里面 */
                    strcat((char *)pbuf, (char *)phone_remind_tbl[4][gui_phy.language]);
                    twin->caption = pbuf;
                    window_draw(twin);  /* 画出窗体 */
                    btn_draw(rbtn);     /* 画按钮 */
                }
            }
        }

        gsm_send_cmd("ATH", "OK", 200); /* 挂机 */
        gsm_cmd_over();                 /* 处理完毕 */
    }

    window_delete(twin);    /* 删除窗口 */
    btn_delete(rbtn);       /* 删除按钮 */
    btn_delete(okbtn);      /* 删除按钮 */
    gui_memin_free(pbuf);   /* 释放内存 */
    gsmdev.mode = 0;        /* 进入拨号模式 */
    system_task_return = 0; /* 防止退出其他任务 */
    OSTaskResume(6);        /* 恢复主任务 */

    if (camflag)
    {
        EXTI->IMR |= 1 << 8;    /* 开启line8中断 */
    }

    phone_incall_task_delete(); /* 删除任务 */
}

/**
 * @brief       创建来电任务
 * @param       无
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t phone_incall_task_creat(void)
{
    OS_CPU_SR cpu_sr = 0;
    uint8_t res;

    if (PHONE_INCALL_TASK_STK)return 0;         /* 任务已经创建了 */

    PHONE_INCALL_TASK_STK = gui_memin_malloc(PHONE_INCALL_STK_SIZE * sizeof(OS_STK));

    if (PHONE_INCALL_TASK_STK == 0)return 1;    /* 内存申请失败 */

    OS_ENTER_CRITICAL();    /* 进入临界区(无法被中断打断) */
    res = OSTaskCreate(phone_incall_task, (void *)0, (OS_STK *)&PHONE_INCALL_TASK_STK[PHONE_INCALL_STK_SIZE - 1], PHONE_INCALL_TASK_PRIO);
    OS_EXIT_CRITICAL();     /* 退出临界区(可以被中断打断) */
    return res;
}

/**
 * @brief       删除来电任务
 * @param       无
 * @retval      无
 */
void phone_incall_task_delete(void)
{
    gui_memin_free(PHONE_INCALL_TASK_STK);  /* 释放内存 */
    PHONE_INCALL_TASK_STK = 0;
    OSTaskDel(PHONE_INCALL_TASK_PRIO);      /* 删除来电任务 */
}

/**
 * @brief       电话拨号主程序
 * @param       无
 * @retval      0, 正常退出; 其他, 错误代码;
 */
uint8_t phone_play(void)
{
    _btn_obj *p_btn[14];/* 总共14个按键 */
    _phonedis_obj *pdis;
    uint8_t btnxsize, btnysize;
    uint8_t btnxydis;
    uint8_t fsize;
    uint8_t lcdtype;
    uint16_t yoff;

    uint8_t i, j;
    uint8_t rval = 0;
    uint8_t res = 0;
    FIL *f_phone = 0;
    uint32_t calltime;
    uint32_t br;

    pdis = gui_memin_malloc(sizeof(_phonedis_obj));

    if (pdis == NULL)return 1;              /* 申请不成功,直接退出 */

    memset(pdis, 0, sizeof(_phonedis_obj)); /* 清零数据 */
    pdis->inbuf = gui_memin_malloc(PHONE_MAX_INPUT);
    pdis->phonebuf = gui_memin_malloc(PHONE_MAX_INPUT);
    f_phone = (FIL *)gui_memin_malloc(sizeof(FIL)); /* 开辟FIL字节的内存区域 */

    if (!pdis->phonebuf || !pdis->inbuf || !f_phone)rval = 1;   /* 申请失败 */
    else
    {
        if (lcddev.width <= 272)
        {
            res = f_open(f_phone, (const TCHAR *)APP_ASCII_2814, FA_READ); /* 打开文件 */

            if (res == FR_OK)
            {
                asc2_2814 = (uint8_t *)gui_memex_malloc(f_phone->obj.objsize);  /* 为大字体开辟缓存地址 */

                if (asc2_2814 == 0)rval = 1;
                else res = f_read(f_phone, asc2_2814, f_phone->obj.objsize, (UINT *)&br);   /* 一次读取整个文件 */

                f_close(f_phone);
            }
        }
        else if (lcddev.width == 320)
        {
            res = f_open(f_phone, (const TCHAR *)APP_ASCII_3618, FA_READ); /* 打开文件 */

            if (res == FR_OK)
            {
                asc2_3618 = (uint8_t *)gui_memex_malloc(f_phone->obj.objsize);  /* 为大字体开辟缓存地址 */

                if (asc2_3618 == 0)rval = 1;
                else res = f_read(f_phone, asc2_3618, f_phone->obj.objsize, (UINT *)&br);   /* 一次读取整个文件 */

                f_close(f_phone);
            }
        }
        else if (lcddev.width >= 480)
        {
            res = f_open(f_phone, (const TCHAR *)APP_ASCII_5427, FA_READ);  /* 打开文件 */

            if (res == FR_OK)
            {
                asc2_5427 = (uint8_t *)gui_memex_malloc(f_phone->obj.objsize);  /* 为大字体开辟缓存地址 */

                if (asc2_5427 == 0)rval = 1;
                else res = f_read(f_phone, asc2_5427, f_phone->obj.objsize, (UINT *)&br);   /* 一次读取整个文件 */

                f_close(f_phone);
            }
        }

        if (res)rval = res;
    }

    if (rval == 0) /* 申请成功 */
    {
        lcd_clear(BLACK);

        if (lcddev.width == 240)
        {
            btnxsize = 78;
            btnysize = 40;
            btnxydis = 2;
            fsize = 28;
            pdis->psize = 28;
            pdis->tsize = 12;
            lcdtype = 0;
        }

        if (lcddev.width == 272)
        {
            btnxsize = 86;
            btnysize = 50;
            btnxydis = 4;
            fsize = 28;
            pdis->psize = 28;
            pdis->tsize = 12;
            lcdtype = 0;
        }
        else if (lcddev.width == 320)
        {
            btnxsize = 102;
            btnysize = 60;
            btnxydis = 4;
            fsize = 36;
            pdis->psize = 36;
            pdis->tsize = 16;
            lcdtype = 1;
        }
        else if (lcddev.width == 480)
        {
            btnxsize = 154;
            btnysize = 100;
            btnxydis = 6;
            fsize = 54;
            pdis->psize = 54;
            pdis->tsize = 24;
            lcdtype = 2;
        }
        else if (lcddev.width == 600)
        {
            btnxsize = 190;
            btnysize = 125;
            btnxydis = 10;
            fsize = 54;
            pdis->psize = 54;
            pdis->tsize = 24;
            lcdtype = 2;
        }
        else if (lcddev.width == 800)
        {
            btnxsize = 250;
            btnysize = 150;
            btnxydis = 10;
            fsize = 54;
            pdis->psize = 54;
            pdis->tsize = 24;
            lcdtype = 2;
        }

        pdis->xoff = 0;
        pdis->yoff = 0;
        pdis->width = lcddev.width;
        pdis->height = lcddev.height - 5 * btnysize - 6 * btnxydis;
        yoff = lcddev.height - btnysize * 5 - btnxydis * 6;

        for (i = 0; i < 5; i++) /* 创建14个按钮 */
        {
            for (j = 0; j < 3; j++)
            {
                res = i * 3 + j;

                if (i == 4)
                {
                    if (j == 2)break;

                    p_btn[res] = btn_creat(j * (btnxsize + btnxsize / 2 + (btnxydis * 3) / 2) + btnxydis / 2, yoff + i * (btnysize + btnxydis) + btnxydis, (btnxsize * 3) / 2 + btnxydis / 2, btnysize, 0, 1);  /* 创建图片按钮 */

                    if (j == 0)
                    {
                        p_btn[res]->bcfdcolor = 0X76B2;;    /* 按下时的背景色 */
                        p_btn[res]->bcfucolor = 0X3DEA;     /* 松开时背景色 */
                        p_btn[res]->picbtnpathu = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                        p_btn[res]->picbtnpathd = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                    }
                    else
                    {
                        p_btn[res]->bcfdcolor = 0X630C;;    /* 按下时的背景色 */
                        p_btn[res]->bcfucolor = 0X4208;     /* 松开时背景色 */
                        p_btn[res]->picbtnpathu = (uint8_t *)PHONE_DEL_PIC[lcdtype];
                        p_btn[res]->picbtnpathd = (uint8_t *)PHONE_DEL_PIC[lcdtype];
                    }
                }
                else
                {
                    p_btn[res] = btn_creat(j * (btnxsize + btnxydis) + btnxydis / 2, yoff + i * (btnysize + btnxydis) + btnxydis, btnxsize, btnysize, 0, 2); /* 创建圆角按钮 */
                    p_btn[res]->bkctbl[0] = 0X39E7;     /* 边框颜色 */
                    p_btn[res]->bkctbl[1] = 0X73AE;     /* 第一行的颜色 */
                    p_btn[res]->bkctbl[2] = 0X4208;     /* 上半部分颜色 */
                    p_btn[res]->bkctbl[3] = 0X3186;     /* 下半部分颜色 */
                    p_btn[res]->bcfucolor = WHITE;      /* 松开时为白色 */
                    p_btn[res]->bcfdcolor = BLACK;      /* 按下时为黑色 */
                    p_btn[res]->caption = (uint8_t *)call_btnstr_tbl[res];
                }

                if (lcddev.width <= 272)p_btn[res]->font = 16;
                else p_btn[res]->font = 24;

                if (p_btn[res] == NULL)
                {
                    rval = 1;   /* 创建失败 */
                    break;
                }

                p_btn[res]->sta = 0;
                p_btn[res]->font = fsize;
            }
        }
    }

    if (rval == 0)
    {
        for (i = 0; i < 14; i++)btn_draw(p_btn[i]);

        gsmdev.mode = 0;
        memset(pdis->inbuf, 0, PHONE_MAX_INPUT);
        memset(pdis->phonebuf, 0, PHONE_MAX_INPUT);

        while (1)
        {
            tp_dev.scan(0);
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH); /* 得到按键键值 */
            delay_ms(5);    /* 延时一个时钟节拍 */

            if (system_task_return)break;   /* TPAD返回 */
    
            if (g_usart3_rx_sta & 0X8000)     /* 接收到数据 */
            {
                if (gsmdev.mode == 1 || gsmdev.mode == 2)
                {
                    if (gsmdev.mode == 1)
                    {
                        if (gsm_check_cmd("+COLP:") || gsm_check_cmd("CONNECT"))    /* 收到+COLP或者CONNECT, 表示拨号成功 */
                        {
                            pdis->time = 0;
                            calltime = OSTime;  /* 记录刚刚建立通话时的时间 */
                            gsmdev.mode = 2;    /* 拨号成功 */
                        }
                    }

                    if (gsm_check_cmd("NO CARRIER"))gsmdev.mode = 0;    /* 拨号失败 */

                    if (gsm_check_cmd("NO ANSWER"))gsmdev.mode = 0;     /* 拨号失败 */

                    if (gsm_check_cmd("ERROR"))gsmdev.mode = 0;         /* 拨号失败 */

                    if (gsmdev.mode == 0)
                    {
                        phone_show_clear(pdis);         /* 清除原来的显示 */
                        p_btn[12]->bcfdcolor = 0X76B2;  /* 按下时的背景色 */
                        p_btn[12]->bcfucolor = 0X3DEA;  /* 松开时背景色 */
                        p_btn[12]->picbtnpathu = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                        p_btn[12]->picbtnpathd = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                        btn_draw(p_btn[12]);
                        pdis->inlen = 0;
                        pdis->inbuf[pdis->inlen] = 0;   /* 结束符 */
                        phone_show_phone(pdis, &gsmdev);/* 显示电话号码 */
                    }

                    gsm_cmd_over(); /* 处理完毕 */
                }
            }

            if (gsmdev.mode == 2)   /* 通话中 */
            {
                if ((OSTime - calltime) >= OS_TICKS_PER_SEC) /* 超过1秒钟了 */
                {
                    calltime = OSTime;
                    pdis->time++;
                    phone_show_time(pdis);/* 显示时间 */
                }
            }

            for (i = 0; i < 14; i++)
            {
                res = btn_check(p_btn[i], &in_obj);

                if (res && ((p_btn[i]->sta & (1 << 7)) == 0) && (p_btn[i]->sta & (1 << 6))) /* 有按键按下且松开,并且TP松开了 */
                {
                    if (i < 12) /* 1~# */
                    {
                        if (gsmdev.mode == 0) /* 号码输入模式 */
                        {
                            if (pdis->plen < PHONE_MAX_INPUT - 1)
                            {
                                u3_printf("AT+CLDTMF=2,\"%c\"\r\n", call_btnstr_tbl[i][0]);
                                pdis->phonebuf[pdis->plen] = call_btnstr_tbl[i][0]; /* 添加字符 */
                                pdis->plen++;
                                pdis->phonebuf[pdis->plen] = 0; /* 添加结束符 */
                            }

                        }
                        else if (gsmdev.mode == 2) /* 仅在通话中有效 */
                        {
                            if (pdis->inlen < PHONE_MAX_INPUT - 1)
                            {
                                u3_printf("AT+CLDTMF=2,\"%c\"\r\n", call_btnstr_tbl[i][0]);
                                delay_ms(150);
                                u3_printf("AT+VTS=%c\r\n", call_btnstr_tbl[i][0]);
                                pdis->inbuf[pdis->inlen] = call_btnstr_tbl[i][0];   /* 添加字符 */
                                pdis->inlen++;
                                pdis->inbuf[pdis->inlen] = 0; /* 添加结束符 */
                            }
                        }
                    }

                    if (i == 12 && pdis->plen) /* 仅在有号码的时候进行拨号 */
                    {
                        if (gsmdev.mode == 0)
                        {
                            if ((gsmdev.status & 0XC0) != 0XC0) /* GSM模块/SIM卡 错误,提示错误信息 */
                            {
                                if ((gsmdev.status & 0X80) == 0)window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2, 220, 100, (uint8_t *)phone_remind_tbl[2][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 1 << 6, 1200);
                                else window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2, 220, 100, (uint8_t *)phone_remind_tbl[3][gui_phy.language], (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language], 12, 0, 1 << 6, 1200);

                                break;
                            }

                            gsmdev.mode = 1; /* 进入拨号中模式 */
                            p_btn[12]->bcfdcolor = 0XFBEF;      /* 按下时的背景色 */
                            p_btn[12]->bcfucolor = 0XFA69;      /* 松开时背景色 */
                            p_btn[12]->picbtnpathu = (uint8_t *)PHONE_HANGUP_PIC[lcdtype];
                            p_btn[12]->picbtnpathd = (uint8_t *)PHONE_HANGUP_PIC[lcdtype];
                            phone_show_clear(pdis);             /* 清除原来的显示 */
                            phone_show_phone(pdis, &gsmdev);    /* 显示电话号码 */
                            phone_show_calling(pdis, &gsmdev);  /* 呼叫中 */
                            u3_printf("ATD%s;\r\n", pdis->phonebuf);    /* 拨号 */
                        }
                        else
                        {
                            gsmdev.mode = 0; /* 恢复拨号模式 */
                            p_btn[12]->bcfdcolor = 0X76B2;      /* 按下时的背景色 */
                            p_btn[12]->bcfucolor = 0X3DEA;      /* 松开时背景色 */
                            p_btn[12]->picbtnpathu = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                            p_btn[12]->picbtnpathd = (uint8_t *)PHONE_CALL_PIC[lcdtype];
                            phone_show_clear(pdis);             /* 清除原来的显示 */
                            pdis->inlen = 0;
                            pdis->inbuf[pdis->inlen] = 0;       /* 结束符 */
                            gsm_send_cmd("ATH", "OK", 200);     /* 挂机 */
                            gsm_cmd_over();/* 处理完毕 */
                        }

                        btn_draw(p_btn[12]);
                    }

                    if (i == 13) /* 退格 */
                    {
                        if (gsmdev.mode == 0) /* 号码输入模式 */
                        {
                            if (pdis->plen)pdis->plen--;

                            pdis->phonebuf[pdis->plen] = 0; /* 结束符 */
                        }
                        else if (gsmdev.mode == 2)
                        {
                            if (pdis->inlen)pdis->inlen--;

                            pdis->inbuf[pdis->inlen] = 0;   /* 结束符 */
                        }
                    }

                    if (gsmdev.mode == 0)phone_show_phone(pdis, &gsmdev); /* 显示电话号码 */

                    if (gsmdev.mode == 2)phone_show_input(pdis, &gsmdev); /* 显示输入数据 */
                }
            }
        }
    }

    if (gsmdev.mode)
    {
        gsm_send_cmd("ATH", "OK", 200); /* 挂机 */
        gsm_cmd_over();/* 处理完毕 */
    }

    gui_memin_free(f_phone);            /* 释放内存 */
    gui_memin_free(pdis->inbuf);        /* 释放内存 */
    gui_memin_free(pdis->phonebuf);     /* 释放内存 */
    gui_memin_free(pdis);               /* 释放内存 */

    if (lcddev.width <= 272)
    {
        gui_memex_free(asc2_2814);
        asc2_2814 = 0;
    }
    else if (lcddev.width == 320)
    {
        gui_memex_free(asc2_3618);
        asc2_3618 = 0;
    }
    else if (lcddev.width >= 480)
    {
        gui_memex_free(asc2_5427);
        asc2_5427 = 0;
    }

    for (i = 0; i < 14; i++)btn_delete(p_btn[i]); /* 删除按钮 */

    return 0;
}








































