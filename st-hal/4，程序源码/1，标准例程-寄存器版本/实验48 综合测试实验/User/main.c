/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-26
 * @brief       综合测试 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./BSP/USART3/usart3.h"
#include "./PICTURE/gif.h"
#include "os.h"
#include "spb.h"
#include "common.h"
#include "ebook.h"
#include "picviewer.h"
#include "settings.h"
#include "calendar.h"
#include "beepplay.h"
#include "ledplay.h"
#include "keyplay.h"
#include "usbplay.h"
#include "nesplay.h"
#include "notepad.h"
#include "paint.h"
#include "exeplay.h"
#include "camera.h"
#include "wirelessplay.h"
#include "calculator.h"
#include "vmeterplay.h"
#include "phoneplay.h"
#include "appplay.h"
#include "netplay.h"

#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
#define __ALIGNED_8     __align(8)  /* AC5使用这个 */
#else                               /* 使用AC6编译器时 */
#define __ALIGNED_8     __ALIGNED(8) /* AC6使用这个 */
#endif

/******************************************************************************************/
/* UCOSII任务设置 */

/* START 任务 配置 */
#define START_TASK_PRIO                 10                  /* 开始任务的优先级设置为最低 */
#define START_STK_SIZE                  64                  /* 堆栈大小 */

__ALIGNED_8 static OS_STK START_TASK_STK[START_STK_SIZE];   /* 任务堆栈,8字节对齐 */
void start_task(void *pdata);                               /* 任务函数 */


/* 串口 任务 配置 */
#define USART_TASK_PRIO                 7                   /* 任务优先级 */
#define USART_STK_SIZE                  128                 /* 堆栈大小 */

__ALIGNED_8 static OS_STK USART_TASK_STK[USART_STK_SIZE];   /* 任务堆栈,8字节对齐 */
void usart_task(void *pdata);                               /* 任务函数 */


/* 主 任务 配置 */
#define MAIN_TASK_PRIO                  6                   /* 任务优先级 */
#define MAIN_STK_SIZE                   512                 /* 堆栈大小 */

__ALIGNED_8 static OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];     /* 任务堆栈,8字节对齐 */
void main_task(void *pdata);                                /* 任务函数 */


/* 监视 任务 配置 */
#define WATCH_TASK_PRIO                 3                   /* 任务优先级 */
#define WATCH_STK_SIZE                  256                 /* 堆栈大小 */

__ALIGNED_8 static OS_STK WATCH_TASK_STK[WATCH_STK_SIZE];   /* 任务堆栈,8字节对齐 */
void watch_task(void *pdata);                               /* 任务函数 */

/******************************************************************************************/

/**
 * @brief       显示错误信息,停止运行,持续提示错误信息
 * @param       x, y : 坐标
 * @param       err  : 错误信息
 * @param       fsize: 字体大小
 * @retval      无
 */
void system_error_show(uint16_t x, uint16_t y, uint8_t *err, uint8_t fsize)
{
    uint8_t ledr = 1;

    while (1)
    {
        lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, (char *)err, RED);
        delay_ms(400);
        lcd_fill(x, y, lcddev.width - 1, y + fsize, BLACK);
        delay_ms(100);
        LED0(ledr ^= 1);
    }
}

/**
 * @brief       显示错误信息, 显示以后(2秒), 继续运行
 * @param       x, y : 坐标
 * @param       fsize: 字体大小
 * @param       str  : 字符串
 * @retval      无
 */
void system_error_show_pass(uint16_t x, uint16_t y, uint8_t fsize, uint8_t *str)
{
    BEEP(1);
    lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, (char *)str, RED);
    delay_ms(2000);
    BEEP(0);
}

/**
 * @brief       擦除整个SPI FLASH(即所有资源都删除),以快速更新系统.
 * @param       x, y : 坐标
 * @param       fsize: 字体大小
 * @retval      0,没有擦除; 1,擦除了;
 */
uint8_t system_files_erase(uint16_t x, uint16_t y, uint8_t fsize)
{
    uint8_t ledr = 1;
    uint8_t key;
    uint8_t t = 0;
    uint16_t i = 0;

    lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, "Erase all system files?", RED);

    while (1)
    {
        t++;

        if (t == 20)lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "KEY0:NO / KEY1:YES", RED);

        if (t == 40)
        {
            gui_fill_rectangle(x, y + fsize, lcddev.width, fsize, BLACK); /* 清除显示 */
            t = 0;
            LED0(ledr ^= 1);
        }

        key = key_scan(0);

        if (key == KEY0_PRES)   /* 不擦除,用户取消了 */
        {
            gui_fill_rectangle(x, y, lcddev.width, fsize * 2, BLACK); /* 清除显示 */
            g_point_color = WHITE;
            LED0(1);
            return 0;
        }

        if (key == KEY1_PRES)   /* 要擦除,要重新来过 */
        {
            LED0(1);
            lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "Erasing SPI FLASH...", RED);

            for (i = 200; i < 4096; i++)
            {
                fonts_progress_show(x + fsize * 22 / 2, y + fsize, fsize, 3895, i - 200, RED);  /* 显示百分比 */
                norflash_erase_sector(i);   /* 擦除一个扇区约50ms，3895个，大概要：195秒 */
            }

            lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "Erasing SPI FLASH OK      ", RED);
            delay_ms(600);
            return 1;
        }

        delay_ms(10);
    }
}

/**
 * @brief       字库更新确认提示.
 * @param       x, y : 坐标
 * @param       fsize: 字体大小
 * @retval      0,不需要更新; 1,确认要更新;
 */
uint8_t system_font_update_confirm(uint16_t x, uint16_t y, uint8_t fsize)
{
    uint8_t ledr = 1;
    uint8_t key;
    uint8_t t = 0;
    uint8_t res = 0;
    g_point_color = RED;
    lcd_show_string(x, y, lcddev.width, lcddev.height, fsize, "Update font?", RED);

    while (1)
    {
        t++;

        if (t == 20)lcd_show_string(x, y + fsize, lcddev.width, lcddev.height, fsize, "KEY0:NO / KEY1:YES", RED);

        if (t == 40)
        {
            gui_fill_rectangle(x, y + fsize, lcddev.width, fsize, BLACK); /* 清除显示 */
            t = 0;
            LED0(ledr ^= 1);
        }

        key = key_scan(0);

        if (key == KEY0_PRES)break; /* 不更新 */

        if (key == KEY1_PRES)
        {
            res = 1;    /* 要更新 */
            break;
        }

        delay_ms(10);
    }

    LED0(1);
    gui_fill_rectangle(x, y, lcddev.width, fsize * 2, BLACK); /* 清除显示 */
    g_point_color = WHITE;
    return res;
}

uint8_t g_tpad_failed_flag = 0;     /* TPAD失效标记，如果失效，则使用WK_UP作为退出按键 */

/**
 * @brief       系统初始化
 * @param       无
 * @retval      无
 */
void system_init(void)
{
    uint16_t okoffset = 162;
    uint16_t ypos = 0;
    uint16_t j = 0;
    uint16_t temp = 0;
    uint8_t res;
    uint32_t dtsize, dfsize;
    uint8_t *stastr = 0;
    uint8_t *version = 0;
    uint8_t verbuf[12];
    uint8_t fsize;
    uint8_t icowidth;

    sys_stm32_clock_init(9);            /* 设置时钟, 72Mhz */
    usart_init(72, 115200);             /* 串口初始化为115200 */
    
    norflash_init();                    /* 初始化SPI FLASH */
    exeplay_app_check();                /* 检测是否需要运行APP代码 */

    delay_init(72);                     /* 延时初始化 */
    usmart_dev.init(72);                /* 初始化USMART */
    usart3_init(36, 115200);            /* 初始化串口3波特率为115200 */
    lcd_init();                         /* 初始化LCD */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */ 
    beep_init();                        /* 蜂鸣器初始化 */
    at24cxx_init();                     /* EEPROM初始化 */
    adc_temperature_init();             /* 初始化内部温度传感器 */
    lsens_init();                       /* 初始化光敏传感器 */

    my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */

    tp_dev.init();
    gui_init();
    piclib_init();                      /* piclib初始化 */
    slcd_dma_init();                    /* SLCD DMA初始化 */
    exfuns_init();                      /* FATFS 申请内存 */

    version = mymalloc(SRAMIN, 31);     /* 申请31个字节内存 */

REINIT: /* 重新初始化 */

    lcd_clear(BLACK);                   /* 黑屏 */
    g_point_color = WHITE;
    g_back_color = BLACK;
    j = 0;

    /* 显示版权信息 */
    ypos = 2;

    if (lcddev.width <= 272)
    {
        fsize = 12;
        icowidth = 24;
        okoffset = 190;
        app_show_mono_icos(5, ypos, icowidth, 24, (uint8_t *)APP_ALIENTEK_ICO2424, YELLOW, BLACK);
    }
    else if (lcddev.width == 320)
    {
        fsize = 16;
        icowidth = 32;
        okoffset = 250;
        app_show_mono_icos(5, ypos, icowidth, 32, (uint8_t *)APP_ALIENTEK_ICO3232, YELLOW, BLACK);
    }
    else if (lcddev.width >= 480)
    {
        fsize = 24;
        icowidth = 48;
        okoffset = 370;
        app_show_mono_icos(5, ypos, icowidth, 48, (uint8_t *)APP_ALIENTEK_ICO4848, YELLOW, BLACK);
    }

    lcd_show_string(icowidth + 5 * 2, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "ALIENTEK STM32F103", g_point_color);
    lcd_show_string(icowidth + 5 * 2, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "Copyright (C) 2022-2032", g_point_color);
    app_get_version(verbuf, HARDWARE_VERSION, 2);
    strcpy((char *)version, "HARDWARE:");
    strcat((char *)version, (const char *)verbuf);
    strcat((char *)version, ", SOFTWARE:");
    app_get_version(verbuf, SOFTWARE_VERSION, 3);
    strcat((char *)version, (const char *)verbuf);
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, (char *)version, g_point_color);
    sprintf((char *)verbuf, "LCD ID:%04X", lcddev.id);  /* LCD ID打印到verbuf里面 */
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, (char *)verbuf, g_point_color);  /* 显示LCD ID */

    /* 开始硬件检测初始化 */
    LED0(0);
    LED1(0);    /* 同时点亮2个LED */
    
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "CPU:STM32F103ZET6 72Mhz", g_point_color);
    lcd_show_string(5, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "FLASH:512KB + 16MB SRAM:64KB", g_point_color);
    LED0(1);
    LED1(1);    /* 同时关闭2个LED */

    /* SPI FLASH检测 */
    if (norflash_read_id() != BY25Q128 && norflash_read_id() != W25Q128 && norflash_read_id() != NM25Q128)  /* 读取QSPI FLASH ID */
    {
        system_error_show(5, ypos + fsize * j++, "SPI Flash Error!!", fsize);
    }
    else temp = 16 * 1024;  /* 16M字节大小 */

    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SPI Flash:     KB", g_point_color);
    lcd_show_xnum(5 + 10 * (fsize / 2), ypos + fsize * j, temp, 5, fsize, 0, g_point_color); /* 显示spi flash大小 */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* 检测是否需要擦除SPI FLASH? */
    res = key_scan(1);

    if (res == WKUP_PRES)   /* 启动的时候，按下WKUP按键，则擦除SPI FLASH字库和文件系统区域 */
    {
        res = system_files_erase(5, ypos + fsize * j, fsize);

        if (res)goto REINIT;
    }

    /* RTC检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "RTC Check...", g_point_color);

    if (rtc_init())system_error_show_pass(5 + okoffset, ypos + fsize * j++, fsize, "ERROR"); /* RTC检测 */
    else
    {
        calendar_get_time(&calendar);/* 得到当前时间 */
        calendar_get_date(&calendar);/* 得到当前日期 */
        lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);
    }

    /* 检查SPI FLASH的文件系统 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "FATFS Check...", g_point_color); /* FATFS检测 */
    f_mount(fs[0], "0:", 1);    /* 挂载SD卡 */
    f_mount(fs[1], "1:", 1);    /* 挂载SPI FLASH */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* SD卡检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SD Card:     MB", g_point_color); /* FATFS检测 */
    temp = 0;

    do
    {
        temp++;
        res = exfuns_get_free("0:", &dtsize, &dfsize);  /* 得到SD卡剩余容量和总容量 */
        delay_ms(200);
    } while (res && temp < 5); /* 连续检测5次 */

    if (res == 0)   /* 得到容量正常 */
    {
        gui_phy.memdevflag |= 1 << 0;	/* 设置SD卡在位 */
        temp = dtsize >> 10; /* 单位转换为MB */
        stastr = "OK";
    }
    else
    {
        temp = 0; /* 出错了,单位为0 */
        stastr = "ERROR";
    }

    lcd_show_xnum(5 + 8 * (fsize / 2), ypos + fsize * j, temp, 5, fsize, 0, g_point_color);	/* 显示SD卡容量大小 */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, (char *)stastr, g_point_color);   /* SD卡状态 */

    /* 25Q128检测,如果不存在文件系统,则先创建 */
    temp = 0;

    do
    {
        temp++;
        res = exfuns_get_free("1:", &dtsize, &dfsize); /* 得到FLASH剩余容量和总容量 */
        delay_ms(200);
    } while (res && temp < 20); /* 连续检测20次 */

    if (res == 0X0D)   /* 文件系统不存在 */
    {
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SPI Flash Disk Formatting...", g_point_color); /* 格式化FLASH */
        res = f_mkfs("1:", 0, 0, FF_MAX_SS);    /* 格式化SPI FLASH,1:,盘符;0,自动选择文件系统类型 */

        if (res == 0)
        {
            f_setlabel((const TCHAR *)"1:ALIENTEK");        /* 设置Flash磁盘的名字为：ALIENTEK */
            lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color); /* 标志格式化成功 */
            res = exfuns_get_free("1:", &dtsize, &dfsize);  /* 重新获取容量 */
        }
    }

    if (res == 0)   /* 得到FLASH卡剩余容量和总容量 */
    {
        gui_phy.memdevflag |= 1 << 1;   /* 设置SPI FLASH在位 */
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SPI Flash Disk:     KB", g_point_color); /* FATFS检测 */
        temp = dtsize;
    }
    else system_error_show(5, ypos + fsize * (j + 1), "Flash Fat Error!", fsize);   /* flash 文件系统错误 */

    lcd_show_xnum(5 + 15 * (fsize / 2), ypos + fsize * j, temp, 5, fsize, 0, g_point_color);    /* 显示FLASH容量大小 */
    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);	/* FLASH卡状态 */

    /* TPAD检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "TPAD Check...", g_point_color);

    if (tpad_init(6))system_error_show(5, ypos + fsize * (j + 1), "TPAD Error!", fsize); /* 触摸按键检测 */
    else lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* 24C02检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "24C02 Check...", g_point_color);

    if (at24cxx_check())system_error_show(5, ypos + fsize * (j + 1), "24C02 Error!", fsize);    /* 24C02检测 */
    else lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* 字库检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "Font Check...", g_point_color);
    res = key_scan(1); /* 检测按键 */

    if (res == KEY1_PRES)   /* KEY1按下，更新？确认 */
    {
        res = system_font_update_confirm(5, ypos + fsize * (j + 1), fsize);
    }
    else res = 0;

    if (fonts_init() || (res == 1))   /* 检测字体,如果字体不存在/强制更新,则更新字库 */
    {
        res = 0; /* 按键无效 */

        if (fonts_update_font(5, ypos + fsize * j, fsize, "0:", g_point_color) != 0)        /* 从SD卡更新 */
        {

            if (fonts_update_font(5, ypos + fsize * j, fsize, "1:", g_point_color) != 0)    /* 从SPI FLASH更新 */
            {
                system_error_show(5, ypos + fsize * (j + 1), "Font Error!", fsize);         /* 字体错误 */
            }

        }

        lcd_fill(5, ypos + fsize * j, lcddev.width - 1, ypos + fsize * (j + 1), BLACK);     /* 填充底色 */
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "Font Check...", g_point_color);
    }

    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color); /* 字库检测OK */

    /* 系统文件检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Files Check...", g_point_color);

    while (app_system_file_check("1"))      /* 系统文件检测 */
    {
        lcd_fill(5, ypos + fsize * j, lcddev.width - 1, ypos + fsize * (j + 1), BLACK); /* 填充底色 */
        lcd_show_string(5, ypos + fsize * j, (fsize / 2) * 8, fsize, fsize, "Updating", g_point_color); /* 显示updating */
        app_boot_cpdmsg_set(5, ypos + fsize * j, fsize);    /* 设置到坐标 */
        temp = 0;

        if (app_system_file_check("0"))     /* 检查SD卡系统文件完整性 */
        {
            if (app_system_file_check("2"))res = 9; /* 标记为不可用的盘 */
            else res = 2;                   /* 标记为U盘 */
        }
        else res = 0;                       /* 标记为SD卡 */

        if (res == 0 || res == 2)           /* 完整了才更新 */
        {
            sprintf((char *)verbuf, "%d:", res);

            if (app_system_update(app_boot_cpdmsg, verbuf))   /* 更新? */
            {
                system_error_show(5, ypos + fsize * (j + 1), "SYSTEM File Error!", fsize);
            }
        }

        lcd_fill(5, ypos + fsize * j, lcddev.width - 1, ypos + fsize * (j + 1), BLACK); /* 填充底色 */
        lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Files Check...", g_point_color);

        if (app_system_file_check("1"))     /* 更新了一次，再检测，如果还有不全，说明SD卡文件就不全！ */
        {
            system_error_show(5, ypos + fsize * (j + 1), "SYSTEM File Lost!", fsize);
        }
        else break;
    }

    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    /* 触摸屏检测 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "Touch Check...", g_point_color);
    res = key_scan(1); /* 检测按键 */

    if (tp_init() || (res == KEY0_PRES && (tp_dev.touchtype & 0X80) == 0))   /* 有更新/按下了KEY0且不是电容屏,执行校准 */
    {
        if (res == 1)tp_adjust();

        res = 0;        /* 按键无效 */
        goto REINIT;    /* 重新开始初始化 */
    }

    lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color); /* 触摸屏检测OK */

    /* 系统参数加载 */
    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Parameter Load...", g_point_color);

    if (app_system_parameter_init())system_error_show(5, ypos + fsize * (j + 1), "Parameter Load Error!", fsize); /* 参数加载 */
    else lcd_show_string(5 + okoffset, ypos + fsize * j++, lcddev.width, lcddev.height, fsize, "OK", g_point_color);

    lcd_show_string(5, ypos + fsize * j, lcddev.width, lcddev.height, fsize, "SYSTEM Starting...", g_point_color);

    /* 蜂鸣器短叫,提示正常启动 */
    BEEP(1);
    delay_ms(100);
    BEEP(0);
    myfree(SRAMIN, version);
    delay_ms(1500);
}

int main(void)
{
    system_init();  /* 系统初始化 */
    OSInit();
    OSTaskCreateExt((void(*)(void *) )start_task,               /* 任务函数 */
                    (void *          )0,                        /* 传递给任务函数的参数 */
                    (OS_STK *        )&START_TASK_STK[START_STK_SIZE - 1], /* 任务堆栈栈顶 */
                    (INT8U          )START_TASK_PRIO,           /* 任务优先级 */
                    (INT16U         )START_TASK_PRIO,           /* 任务ID，这里设置为和优先级一样 */
                    (OS_STK *        )&START_TASK_STK[0],       /* 任务堆栈栈底 */
                    (INT32U         )START_STK_SIZE,            /* 任务堆栈大小 */
                    (void *          )0,                        /* 用户补充的存储区 */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* 任务选项,为了保险起见，所有任务都保存浮点寄存器的值 */
    OSStart();
}

/**
 * @brief       开始任务
 * @param       pdata : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pdata)
{
    uint32_t cnts;
    OS_CPU_SR cpu_sr = 0;
    pdata = pdata;
    
    
    /* 根据配置的节拍频率配置SysTick */
    cnts = (CPU_INT32U)((168 * 1000000) / OS_TICKS_PER_SEC);
    OS_CPU_SysTickInit(cnts);
    
    OSStatInit();                           /* 初始化统计任务.这里会延时1秒钟左右 */
    app_srand(OSTime);

    OS_ENTER_CRITICAL();                    /* 进入临界区(无法被中断打断) */
    OSTaskCreateExt((void(*)(void *) )main_task,                /* 任务函数 */
                    (void *          )0,                        /* 传递给任务函数的参数 */
                    (OS_STK *        )&MAIN_TASK_STK[MAIN_STK_SIZE - 1], /* 任务堆栈栈顶 */
                    (INT8U          )MAIN_TASK_PRIO,            /* 任务优先级 */
                    (INT16U         )MAIN_TASK_PRIO,            /* 任务ID，这里设置为和优先级一样 */
                    (OS_STK *        )&MAIN_TASK_STK[0],        /* 任务堆栈栈底 */
                    (INT32U         )MAIN_STK_SIZE,             /* 任务堆栈大小 */
                    (void *          )0,                        /* 用户补充的存储区 */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* 任务选项,为了保险起见，所有任务都保存浮点寄存器的值 */

    OSTaskCreateExt((void(*)(void *) )usart_task,              	/* 任务函数 */
                    (void *          )0,                        /* 传递给任务函数的参数 */
                    (OS_STK *        )&USART_TASK_STK[USART_STK_SIZE - 1], /* 任务堆栈栈顶 */
                    (INT8U          )USART_TASK_PRIO,           /* 任务优先级 */
                    (INT16U         )USART_TASK_PRIO,           /* 任务ID，这里设置为和优先级一样 */
                    (OS_STK *        )&USART_TASK_STK[0],       /* 任务堆栈栈底 */
                    (INT32U         )USART_STK_SIZE,            /* 任务堆栈大小 */
                    (void *          )0,                        /* 用户补充的存储区 */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* 任务选项,为了保险起见，所有任务都保存浮点寄存器的值 */

    OSTaskCreateExt((void(*)(void *) )watch_task,               /* 任务函数 */
                    (void *          )0,                        /* 传递给任务函数的参数 */
                    (OS_STK *        )&WATCH_TASK_STK[WATCH_STK_SIZE - 1], /* 任务堆栈栈顶 */
                    (INT8U          )WATCH_TASK_PRIO,           /* 任务优先级 */
                    (INT16U         )WATCH_TASK_PRIO,           /* 任务ID，这里设置为和优先级一样 */
                    (OS_STK *        )&WATCH_TASK_STK[0],       /* 任务堆栈栈底 */
                    (INT32U         )WATCH_STK_SIZE,            /* 任务堆栈大小 */
                    (void *          )0,                        /* 用户补充的存储区 */
                    (INT16U         )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR | OS_TASK_OPT_SAVE_FP); /* 任务选项,为了保险起见，所有任务都保存浮点寄存器的值 */

    OSTaskSuspend(START_TASK_PRIO);     /* 挂起起始任务 */
    OS_EXIT_CRITICAL();                 /* 退出临界区(可以被中断打断) */
}

/**
 * @brief       主任务
 * @param       pdata : 传入参数(未用到)
 * @retval      无
 */
void main_task(void *pdata)
{
    uint8_t selx;
    uint16_t tcnt = 0;
    
    spb_init(0);        /* 初始化SPB界面 */
    spb_load_mui();     /* 加载SPB主界面 */
    slcd_frame_show(spbdev.spbahwidth); /* 显示界面 */

    while (1)
    {

        selx = spb_move_chk();
        system_task_return = 0;         /* 清退出标志 */

        switch (selx)   /* 发生了双击事件 */
        {
            case 0:
                ebook_play();
                break;  /* 电子图书 */

            case 1:
                picviewer_play();
                break;  /* 数码相框 */

            case 2:
                calendar_play();
                break;  /* 时钟 */

            case 3:
                sysset_play();
                break;  /* 系统设置 */

            case 4:
                notepad_play();
                break;  /* 记事本 */

            case 5:
                exe_play();
                break;  /* 运行器 */

            case 6:
                paint_play();
                break;  /* 手写画笔 */

            case 7:
                camera_play();
                break;  /* 摄像头 */

            case 8:
                usb_play();
                break;  /* USB连接 */

            case 9:
                net_play();
                break;  /* 网络通信 */

            case 10:
                wireless_play();
                break;  /* 无线通信 */

            case 11:
                calc_play();
                break;  /* 计算器 */

            case 12:
                key_play((uint8_t *)APP_MFUNS_CAPTION_TBL[selx][gui_phy.language]);
                break;  /* 按键测试 */

            case 13:
                led_play((uint8_t *)APP_MFUNS_CAPTION_TBL[selx][gui_phy.language]);
                break;  /* LED测试 */

            case 14:
                beep_play((uint8_t *)APP_MFUNS_CAPTION_TBL[selx][gui_phy.language]);
                break;  /* 蜂鸣器通信 */

            case SPB_ICOS_NUM:
                phone_play();
                break;  /* 电话功能 */

            case SPB_ICOS_NUM+1:
                app_play();
                break;  /* APP */

            case SPB_ICOS_NUM+2:
                vmeter_play();
                break;  /* 电压表 */
        }

        if (selx != 0XFF)spb_load_mui();    /* 显示主界面 */

        delay_ms(1000 / OS_TICKS_PER_SEC);  /* 延时一个时钟节拍 */
        tcnt++;

        if (tcnt == OS_TICKS_PER_SEC)       /* OS_TICKS_PER_SEC个节拍为1秒钟 */
        {
            tcnt = 0;
            spb_stabar_msg_show(0);/* 更新状态栏信息 */
        }
    }
}

volatile uint8_t memshow_flag = 1;  /* 默认打印mem使用率 */

/**
 * @brief       串口 任务,执行最不需要时效性的代码
 * @param       pdata : 传入参数(未用到)
 * @retval      无
 */
void usart_task(void *pdata)
{
    uint16_t alarmtimse = 0;
    float psin, psex;
    pdata = pdata;

    while (1)
    {
        delay_ms(1000);

        if (alarm.ringsta & 1 << 7)   /* 执行闹钟扫描函数 */
        {
            calendar_alarm_ring(alarm.ringsta & 0x3); /* 闹铃 */
            alarmtimse++;

            if (alarmtimse > 300)   /* 超过300次了,5分钟以上 */
            {
                alarm.ringsta &= ~(1 << 7); /* 关闭闹铃 */
            }
        }
        else if (alarmtimse)
        {
            alarmtimse = 0;
            BEEP(0);    /* 关闭蜂鸣器 */
        }

        if (gsmdev.mode == 3)           /* 蜂鸣器,来电提醒 */
        {
            phone_ring(); 
        }
        
        if (systemset.lcdbklight == 0)  /* 自动背光控制 */
        {
            app_lcd_auto_bklight(); 
        }
        
        if (memshow_flag == 1)
        {
            psin = my_mem_perused(SRAMIN);
            //psex = my_mem_perused(SRAMEX);
            printf("in:%3.1f,ex:%3.1f\r\n", psin / 10, psex / 10);  /* 打印内存占用率 */
        }
    }
}

volatile uint8_t system_task_return;    /* 任务强制返回标志. */
volatile uint8_t ledplay_ds0_sta = 0;   /* ledplay任务,DS0的控制状态 */
extern uint8_t nes_run_flag;

/**
 * @brief       监视 任务
 * @param       pdata : 传入参数(未用到)
 * @retval      无
 */
void watch_task(void *pdata)
{
    OS_CPU_SR cpu_sr = 0;
    uint8_t t = 0;
    uint8_t rerreturn = 0;
    uint8_t res;
    uint8_t key;
    pdata = pdata;

    while (1)
    {
        if (alarm.ringsta & 1 << 7)   /* 闹钟在执行 */
        {
            calendar_alarm_msg((lcddev.width - 44) / 2, (lcddev.height - 20) / 2); /* 闹钟处理 */
        }

        if (g_gif_decoding)   /* gif正在解码中 */
        {
            key = pic_tp_scan();

            if (key == 1 || key == 3)g_gif_decoding = 0; /* 停止GIF解码 */
        }

        if (ledplay_ds0_sta == 0)   /* 仅当ledplay_ds0_sta等于0的时候,正常熄灭LED0 */
        {
            if (t == 4)LED0(1);     /* 亮100ms左右 */

            if (t == 119)
            {
                LED0(0);            /* 2.5秒钟左右亮一次 */
                t = 0;
            }
        }

        t++;
        
        if (rerreturn)              /* 再次开始TPAD扫描时间减一 */
        {
            rerreturn--;
            delay_ms(15);           /* 补充延时差 */
        }
        else if (tpad_scan(0))      /* TPAD按下了一次,此函数执行,至少需要15ms */
        {
            rerreturn = 10;         /* 下次必须100ms以后才能再次进入 */
            system_task_return = 1;

            if (g_gif_decoding)g_gif_decoding = 0;  /* 不再播放gif */
        }
        

        if ((t % 60) == 0)   /* 900ms左右检测1次 */
        {
            /* SD卡在位检测 */
            OS_ENTER_CRITICAL();        /* 进入临界区(无法被中断打断) */
            res = sdmmc_get_status();   /* 查询SD卡状态 */
            OS_EXIT_CRITICAL();         /* 退出临界区(可以被中断打断) */

            if (res == 0XFF)
            {
                gui_phy.memdevflag &= ~(1 << 0);    /* 标记SD卡不在位 */
                g_sd_card_info.CardCapacity = 0;    /* SD卡容量清零 */
                
                OS_ENTER_CRITICAL();/* 进入临界区(无法被中断打断) */
                sd_init();          /* 重新检测SD卡 */
                OS_EXIT_CRITICAL(); /* 退出临界区(可以被中断打断) */
            }
            else if ((gui_phy.memdevflag & (1 << 0)) == 0)     /* SD不在位? */
            {
                f_mount(fs[0], "0:", 1);        /* 重新挂载sd卡 */
                gui_phy.memdevflag |= 1 << 0;   /* 标记SD卡在位了 */
            }
        
            /* gsm检测 */
            gsm_status_check();         /* gsm检测! */
        }

        gsm_cmsgin_check();             /* 来电/短信 监测 */

        delay_ms(10);
    }
}

/**
 * @brief       硬件错误处理
 * @param       无
 * @retval      无
 */
void HardFault_Handler(void)
{
    uint8_t led1sta = 1;
    uint32_t i;
    uint8_t t = 0;
    uint32_t temp;
    temp = SCB->CFSR;               /* fault状态寄存器(@0XE000ED28)包括:MMSR,BFSR,UFSR */
    printf("CFSR:%8X\r\n", temp);   /* 显示错误值 */
    temp = SCB->HFSR;               /* 硬件fault状态寄存器 */
    printf("HFSR:%8X\r\n", temp);   /* 显示错误值 */
    temp = SCB->DFSR;               /* 调试fault状态寄存器 */
    printf("DFSR:%8X\r\n", temp);   /* 显示错误值 */
    temp = SCB->AFSR;               /* 辅助fault状态寄存器 */
    printf("AFSR:%8X\r\n", temp);   /* 显示错误值 */

    while (t < 5)
    {
        t++;
        LED1(led1sta ^= 1);

        for (i = 0; i < 0X1FFFFF; i++);
    }
}















