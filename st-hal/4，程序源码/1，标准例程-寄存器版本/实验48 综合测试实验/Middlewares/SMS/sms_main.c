/**
 ****************************************************************************************************
 * @file        sms_main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-05-31
 * @brief       SMS������ ����
 *              ��������ֲ������ye781205��NESģ��������, �ش˸�л!
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
 * V1.1 20220531
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"

#include "audioplay.h"
#include "sms_vdp.h"
#include "sms_main.h"
#include "sms_sn76496.h"
#include "sms_z80a.h"

#include "nes_main.h"

#include  "./BSP/JOYPAD/joypad.h"
#include  "./BSP/VS10XX/vs10xx.h"
#include  "./BSP/SPI/spi.h"


uint16_t *smssaibuf1;       /* ��Ƶ����֡,ռ���ڴ��� 367*2 �ֽ�@22050Hz */
uint16_t *smssaibuf2;       /* ��Ƶ����֡,ռ���ڴ��� 367*2 �ֽ�@22050Hz */

uint8_t *sms_rom;           /* SMS ROM�洢��ַָ�� */
uint8_t *SMS_romfile;       /* ROMָ��=sms_rom/sms_rom+512 */
uint8_t  *S_RAM;            /* internal SMS RAM	16k  [0x4000] */
uint8_t  *E_RAM;            /* external cartridge RAM (2 X 16K Bank Max) [0x8000] */



/**
 * @brief       �ͷ�SMS����������ڴ�
 * @param       ��
 * @retval      ��
 */
void sms_sram_free(void)
{
    myfree(SRAMIN, sms_rom);
    myfree(SRAM12, E_RAM);
    myfree(SRAM12, cache);

    myfree(SRAMDTCM, S_RAM);
    myfree(SRAMDTCM, SMS_VDP);
    myfree(SRAMDTCM, psg_buffer);
    myfree(SRAMDTCM, SN76496);
    myfree(SRAMDTCM, Z80A);
    myfree(SRAMDTCM, VRam);

    myfree(SRAM12, smssaibuf1);
    myfree(SRAM12, smssaibuf2);
    myfree(SRAM12, lut);
}

/**
 * @brief       ΪSMS���������ڴ�
 * @param       romsize         : sms�ļ���С
 * @retval      0, �ɹ�;  ����, ʧ��;
 */
uint8_t sms_sram_malloc(uint32_t romsize)
{
    E_RAM = mymalloc(SRAM12, 0X8000);       /* ����2*16K�ֽ� */
    cache = mymalloc(SRAM12, 0x20000);      /* 128K */
    sms_rom = mymalloc(SRAMIN, romsize);    /* ����romsize�ֽڵ��ڴ����� */
    S_RAM = mymalloc(SRAMDTCM, 0X4000);     /* ����16K�ֽ� */
    SMS_VDP = mymalloc(SRAMDTCM, sizeof(SVDP));
    psg_buffer = mymalloc(SRAMDTCM, SNBUF_size * 2);    /* �����ڴ�SNBUF_size */
    SN76496 = mymalloc(SRAMDTCM, sizeof(t_SN76496));    /* �����ڴ�184 */
    Z80A = mymalloc(SRAMDTCM, sizeof(CPU80));           /* �����ڴ� */
    VRam = mymalloc(SRAMDTCM, 0x4000);      /* ����16K�ֽ� */

    smssaibuf1 = mymalloc(SRAM12, SNBUF_size * 4 + 10);
    smssaibuf2 = mymalloc(SRAM12, SNBUF_size * 4 + 10);
    lut = mymalloc(SRAM12, 0x10000);        /* 64K */

    if (sms_rom && cache && VRam && lut)
    {
        memset(E_RAM, 0, 0X8000);           /* ���� */
        memset(cache, 0, 0x20000);          /* ���� */
        memset(S_RAM, 0, 0X4000);           /* ���� */
        memset(SMS_VDP, 0, sizeof(SVDP));   /* ���� */

        memset(psg_buffer, 0, SNBUF_size * 2);  /* ���� */
        memset(SN76496, 0, sizeof(t_SN76496));  /* ���� */
        memset(Z80A, 0, sizeof(CPU80));         /* ���� */
        memset(VRam, 0, 0X4000);                /* ���� */

        memset(smssaibuf1, 0, SNBUF_size * 4 + 10); /* ���� */
        memset(smssaibuf2, 0, SNBUF_size * 4 + 10); /* ���� */
        memset(lut, 0, 0x10000);                    /* ���� */
        return 0;
    }
    else
    {
        sms_sram_free();    /* �ͷ������ڴ� */
        return 1;
    }
}

uint16_t sms_xoff = 0;      /* ��ʾ��x�᷽���ƫ����(ʵ����ʾ���=256-2*sms_xoff) */
uint16_t sms_yoff = 0;      /* ��ʾ��y�᷽���ƫ���� */


/**
 * @brief       ������Ϸ��ʾ����
 * @param       ��
 * @retval      ��
 */
void sms_set_window(void)
{
    uint16_t xoff = 0, yoff = 0;
    uint16_t lcdwidth, lcdheight;

    if (lcddev.width == 240)
    {
        lcdwidth = 240;
        lcdheight = 192;
        sms_xoff = (256 - lcddev.width) / 2;    /* �õ�x�᷽���ƫ���� */
    }
    else if (lcddev.width <= 320)
    {
        lcdwidth = 240;
        lcdheight = 192;
        sms_xoff = 8; /* sms��Ҫƫ��8���� */
    }
    else if (lcddev.width >= 480)
    {
        lcdwidth = 480;
        lcdheight = 192 * 2;
        sms_xoff = (256 - (lcdwidth / 2)) / 2; /* �õ�x�᷽���ƫ���� */
    }

    xoff = (lcddev.width - lcdwidth) / 2;
    yoff = (lcddev.height - lcdheight - gui_phy.tbheight) / 2 + gui_phy.tbheight; /* ��Ļ�߶� */
    sms_yoff = yoff;
    lcd_set_window(xoff, yoff, lcdwidth, lcdheight); /* ��smsʼ������Ļ����������ʾ */
    lcd_set_cursor(xoff, yoff);
    lcd_write_ram_prepare();    /* д��LCD RAM��׼�� */
}

extern volatile uint8_t framecnt;
extern uint8_t nes_run_flag;

/**
 * @brief       ģ�������������ֳ�ʼ��,Ȼ��ѭ������ģ����
 * @param       bank_mun        : �������
 * @retval      ��
 */
void sms_start(uint8_t bank_mun)
{
    uint8_t zhen;
    uint8_t res = 0;
    res = VDP_init();
    res += Z80A_Init(S_RAM, E_RAM, SMS_romfile, bank_mun); /* 0x8080000,0x0f,"Sonic the Hedgehog '91" */
    res += sms_audio_init();

    if (res == 0)
    {
        nes_run_flag = 1;
        tim8_int_init(10000 - 1, 24000 - 1); /* ����TIM8  1s�ж�һ�� */
        sms_set_window();           /* ���ô��� */

        while (1)
        {
            SMS_frame(zhen);        /* +FB_OFS  (24+240*32) */
            nes_get_gamepadval();   /* ����sms���ֱ����ݻ�ȡ���� */
            sms_update_Sound();
            sms_update_pad();       /* ��ȡ�ֱ�ֵ */
            zhen++;
            framecnt++;

            if (zhen > 2)zhen = 0;  /* ��2֡ */

            if (system_task_return)break;   /* TPAD���� */

            if (lcddev.id == 0X1963)        /* ����1963,ÿ����һ֡,��Ҫ���贰�� */
            {
                nes_set_window();
            }
        }

        TIM8->CR1 &= ~(1 << 0); /* �رն�ʱ��8 */
        lcd_set_window(0, 0, lcddev.width, lcddev.height); /* �ָ���Ļ���� */
    }

    nes_run_flag = 0;
    sms_sound_close();          /* �ر���Ƶ��� */
}

/**
 * @brief       �����ֱ�����
 *  @note       SMS��ֵ 1111 1111 ȫ1��ʾû����
 *                     D7  D6  D5   D4   D3  D2  D1  D0
 *              SMS    B   A   ��   ��   ��  ��
 *               FC    ��  ��  ��   ��   ST   S   B   A
 * @param       ��
 * @retval      ��
 */
void sms_update_pad(void)
{
    uint8_t key, key1;
    key = 255 - fcpad.ctrlval;  /* ��FC�ֱ���ֵȡ�� */
    key1 = (key >> 4) | 0xf0;   /* ת��ΪSMS�ֱ���ֵ */
    key1 &= ((key << 4) | 0xcf);
    SetController(key1);
}

/**
 * @brief       ����SMS��Ϸ
 * @param       pname           : sms��Ϸ·��
 * @retval      0, �����˳�
 *              1, �ڴ����
 *              2, �ļ�����
 */
uint8_t sms_load(uint8_t *pname)
{
    uint8_t bank_mun;   /* 16K bank������ */
    uint8_t res = 0;
    FIL *f_sms;
    uint8_t *buf;       /* ���� */
    uint8_t *p;
    uint32_t readlen;   /* �ܶ�ȡ���� */
    uint16_t bread;     /* ��ȡ�ĳ��� */

//    if (audiodev.status & (1 << 7))   /* ��ǰ�ڷŸ�?? */
//    {
//        audio_stop_req(&audiodev);  /* ֹͣ��Ƶ���� */
//        audio_task_delete();        /* ɾ�����ֲ�������. */
//    }

    buf = mymalloc(SRAM12, 1024);
    f_sms = (FIL *)mymalloc(SRAM12, sizeof(FIL));   /* ����FIL�ֽڵ��ڴ����� */

    if (f_sms == NULL)
    {
        myfree(SRAM12, buf);    /* �ͷ��ڴ� */
        return 1;   /* ����ʧ�� */
    }

    res = f_open(f_sms, (const TCHAR *)pname, FA_READ); /* ���ļ� */

    if (res == 0)res = sms_sram_malloc(f_sms->obj.objsize); /* �����ڴ� */

    if (res == 0)
    {
        if ((f_sms->obj.objsize / 512) & 1)   /* �չ�ͼ�����,������� */
        {
            SMS_romfile = sms_rom + 512;
            bank_mun = ((f_sms->obj.objsize - 512) / 0x4000) - 1;   /* 16K bank������ */
        }
        else
        {
            SMS_romfile = sms_rom;
            bank_mun = (f_sms->obj.objsize / 0x4000) - 1;   /* 16K bank������ */
        }

        p = sms_rom;
        readlen = 0;

        while (readlen < f_sms->obj.objsize)    /* ѭ����ȡ����SMS��Ϸ�ļ� */
        {
            res = f_read(f_sms, buf, 1024, (UINT *)&bread); /* �����ļ����� */
            readlen += bread;
            my_mem_copy(p, buf, bread);
            p += bread;

            if (res)break;
        }

        //res=f_read(f_sms,sms_rom,f_sms->obj.objsize,&br);
        if (res)res = 2;    /* �ļ����� */

        f_close(f_sms);     /* �ر��ļ� */
    }

    myfree(SRAM12, buf);    /* �ͷ��ڴ� */
    myfree(SRAM12, f_sms);  /* �ͷ��ڴ� */

    if (res == 0)
    {
        sms_start(bank_mun);    /* ��ʼ��Ϸ */
    }

    sms_sram_free();
    return res;
}


volatile uint8_t smstransferend = 0;    /* sai������ɱ�־ */
volatile uint8_t smswitchbuf = 0;       /* saibufxָʾ��־ */

/**
 * @brief       SAI��Ƶ���Żص�����
 * @param       ��
 * @retval      ��
 */
void sms_sai_dma_tx_callback(void)
{
    uint16_t i;

    if (DMA2_Stream3->CR & (1 << 19))smswitchbuf = 0;
    else smswitchbuf = 1;

    if (smstransferend)
    {
        if (smswitchbuf)for (i = 0; i < SNBUF_size * 2; i++)smssaibuf2[i] = 0;
        else for (i = 0; i < SNBUF_size * 2; i++)smssaibuf1[i] = 0;
    }

    smstransferend = 1;
}

/**
 * @brief       SMS����Ƶ���
 * @param       sample_rate     : ��Ƶ������
 * @retval      ��
 */
void sms_sound_open(int sample_rate)
{
//    printf("sound open:%d\r\n", sample_rate);
//    app_es8388_volset(es8388set.mvol);
//    ES8388_ADDA_Cfg(1, 0);  /* ����DAC */
//    ES8388_Input_Cfg(0);    /* �ر�����ͨ�� */
//    ES8388_Output_Cfg(1, 0);    /* ����DAC��� */
//    ES8388_I2S_Cfg(2, 0);   /* �����ֱ�׼,16λ���ݳ��� */

//    SAIA_Init(0, 1, 4); /* ����SAI,������,16λ���� */
//    SAIA_SampleRate_Set(sample_rate);   /* ���ò����� */
//    SAIA_TX_DMA_Init((uint8_t *)smssaibuf1, (uint8_t *)smssaibuf2, 2 * SNBUF_size, 1); /* DMA���� */
//    sai_tx_callback = sms_sai_dma_tx_callback; /* �ص�����ָsms_sai_dma_tx_callback */
//    SAI_Play_Start();   /* ����DMA */
}

/**
 * @brief       SMS�ر���Ƶ���
 * @param       ��
 * @retval      ��
 */
void sms_sound_close(void)
{
//    SAI_Play_Stop();
//    app_es8388_volset(0);   /* �ر�ES8388������� */
}

/**
 * @brief       SMS��Ƶ�����SAI����
 * @param       ��
 * @retval      ��
 */
void sms_apu_fill_buffer(int samples, uint16_t *wavebuf)
{
    uint8_t i = 5;

    while (i)
    {
        nes_get_gamepadval();   /* ÿ3֡��ѯһ��USB */
        delay_ms(2);
        i--;
    }

//    int i;

//    while (!smstransferend) {};

//    smstransferend = 0;

//    if (smswitchbuf == 0)
//    {
//        for (i = 0; i < SNBUF_size; i++)
//        {
//            smssaibuf1[2 * i] = wavebuf[i];
//            smssaibuf1[2 * i + 1] = wavebuf[i];
//        }
//    }
//    else
//    {
//        for (i = 0; i < SNBUF_size; i++)
//        {
//            smssaibuf2[2 * i] = wavebuf[i];
//            smssaibuf2[2 * i + 1] = wavebuf[i];
//        }
//    }
}







