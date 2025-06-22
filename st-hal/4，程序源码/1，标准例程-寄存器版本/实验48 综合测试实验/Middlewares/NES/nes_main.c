/**
 ****************************************************************************************************
 * @file        nes_main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-29
 * @brief       NES������ ����
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

#include "audioplay.h"
#include "spb.h"
#include "string.h" 
#include "nes_main.h"
#include "nes_ppu.h"
#include "nes_mapper.h"
#include "nes_apu.h"

#include "./MALLOC/malloc.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER/timer.h"
#include "./BSP/JOYPAD/joypad.h"
#include "./BSP/VS10XX/vs10xx.h"
#include "./BSP/SPI/spi.h"

#include "./FATFS/source/ff.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"




extern volatile uint8_t framecnt;   /* ֡������ */
int MapperNo;           /* map��� */
int NES_scanline;       /* nesɨ���� */
int VROM_1K_SIZE;
int VROM_8K_SIZE;
uint32_t NESrom_crc32;

uint8_t PADdata0;       /* �ֱ�1��ֵ [7:0]��7 ��6 ��5 ��4 Start3 Select2 B1 A0 */
uint8_t PADdata1;       /* �ֱ�2��ֵ [7:0]��7 ��6 ��5 ��4 Start3 Select2 B1 A0 */
uint8_t *NES_RAM;       /* ����1024�ֽڶ��� */
uint8_t *NES_SRAM;
NES_header *RomHeader;  /* rom�ļ�ͷ */
MAPPER *NES_Mapper;
MapperCommRes *MAPx;


uint8_t *spr_ram;       /* ����RAM,256�ֽ� */
ppu_data *ppu;          /* ppuָ�� */
uint8_t *VROM_banks;
uint8_t *VROM_tiles;

apu_t *apu;             /* apuָ�� */
uint8_t *wave_buffers;


uint8_t *nesapusbuf[NES_APU_BUF_NUM];   /* ��Ƶ����֡ */
uint8_t *romfile;                       /* nes�ļ�ָ��,ָ������nes�ļ�����ʼ��ַ */


/**
 * @brief       ����ROM
 * @param       ��
 * @retval      0,�ɹ�
 *              1,�ڴ���� 
 *              3,map����
 */
uint8_t nes_load_rom(void)
{
    uint8_t *p;
    uint8_t i;
    uint8_t res = 0;
    p = (uint8_t *)romfile;

    if (strncmp((char *)p, "NES", 3) == 0)
    {
        RomHeader->ctrl_z = p[3];
        RomHeader->num_16k_rom_banks = p[4];
        RomHeader->num_8k_vrom_banks = p[5];
        RomHeader->flags_1 = p[6];
        RomHeader->flags_2 = p[7];

        if (RomHeader->flags_1 & 0x04)p += 512; /* ��512�ֽڵ�trainer */

        if (RomHeader->num_8k_vrom_banks > 0)   /* ����VROM,����Ԥ���� */
        {
            VROM_banks = p + 16 + (RomHeader->num_16k_rom_banks * 0x4000);
#if NES_RAM_SPEED == 1  /* 1:�ڴ�ռ��С 0:�ٶȿ� */
            VROM_tiles = VROM_banks;
#else
            VROM_tiles = mymalloc(SRAMEX, RomHeader->num_8k_vrom_banks * 8 * 1024); /* �������������1MB�ڴ�!!! */

            if (VROM_tiles == 0)VROM_tiles = VROM_banks;                            /* �ڴ治���õ������,����VROM_titles��VROM_banks�����ڴ� */

            compile(RomHeader->num_8k_vrom_banks * 8 * 1024 / 16, VROM_banks, VROM_tiles);
#endif
        }
        else
        {
            VROM_banks = mymalloc(SRAMIN, 8 * 1024);
            VROM_tiles = mymalloc(SRAMEX, 8 * 1024);

            if (!VROM_banks || !VROM_tiles)res = 1;
        }

        VROM_1K_SIZE = RomHeader->num_8k_vrom_banks * 8;
        VROM_8K_SIZE = RomHeader->num_8k_vrom_banks;
        MapperNo = (RomHeader->flags_1 >> 4) | (RomHeader->flags_2 & 0xf0);

        if (RomHeader->flags_2 & 0x0E)MapperNo = RomHeader->flags_1 >> 4;   /* ���Ը���λ�����ͷ����������� */

        printf("use map:%d\r\n", MapperNo);

        for (i = 0; i < 255; i++)   /*  ����֧�ֵ�Mapper�� */
        {
            if (MapTab[i] == MapperNo)break;

            if (MapTab[i] == -1)res = 3;
        }

        if (res == 0)
        {
            switch (MapperNo)
            {
                case 1:
                    MAP1 = mymalloc(SRAMIN, sizeof(Mapper1Res));

                    if (!MAP1)res = 1;

                    break;

                case 4:
                case 6:
                case 16:
                case 17:
                case 18:
                case 19:
                case 21:
                case 23:
                case 24:
                case 25:
                case 64:
                case 65:
                case 67:
                case 69:
                case 85:
                case 189:
                    MAPx = mymalloc(SRAMIN, sizeof(MapperCommRes));

                    if (!MAPx)res = 1;

                    break;

                default:
                    break;
            }
        }
    }

    return res; /* ����ִ�н�� */
}

/**
 * @brief       �ͷ��ڴ�
 * @param       ��
 * @retval      ��
 */
void nes_sram_free(void)
{
    uint8_t i;
    myfree(SRAMIN, NES_RAM);
    myfree(SRAMIN, NES_SRAM);
    myfree(SRAMIN, RomHeader);
    myfree(SRAMIN, NES_Mapper);
    myfree(SRAMIN, spr_ram);
    myfree(SRAMIN, ppu);
    myfree(SRAMIN, apu);
    myfree(SRAMIN, wave_buffers);

    for (i = 0; i < NES_APU_BUF_NUM; i++)myfree(SRAMEX, nesapusbuf[i]); /* �ͷ�APU BUFs */

    myfree(SRAMEX, romfile);

    if ((VROM_tiles != VROM_banks) && VROM_banks && VROM_tiles) /* ����ֱ�ΪVROM_banks��VROM_tiles�������ڴ�,���ͷ� */
    {
        myfree(SRAMIN, VROM_banks);
        myfree(SRAMEX, VROM_tiles);
    }

    switch (MapperNo)       /* �ͷ�map�ڴ� */
    {
        case 1:             /* �ͷ��ڴ� */
            myfree(SRAMIN, MAP1);
            break;

        case 4:
        case 6:
        case 16:
        case 17:
        case 18:
        case 19:
        case 21:
        case 23:
        case 24:
        case 25:
        case 64:
        case 65:
        case 67:
        case 69:
        case 85:
        case 189:
            myfree(SRAMIN, MAPx);
            break;          /* �ͷ��ڴ� */

        default:
            break;
    }

    NES_RAM = 0;
    NES_SRAM = 0;
    RomHeader = 0;
    NES_Mapper = 0;
    spr_ram = 0;
    ppu = 0;
    apu = 0;
    wave_buffers = 0;

    for (i = 0; i < NES_APU_BUF_NUM; i++)
    {
        nesapusbuf[i] = 0;
    }
    
    VROM_banks = 0;
    VROM_tiles = 0;
    MAP1 = 0;
    MAPx = 0;
}

/**
 * @brief       ΪNES���������ڴ�
 * @param       romsize         : nes�ļ���С
 * @retval      0,����ɹ�
 *              1,����ʧ��
 */
uint8_t nes_sram_malloc(uint32_t romsize)
{
    uint16_t i = 0;

    for (i = 0; i < 64; i++)    /* ΪNES_RAM,����1024������ڴ� */
    {
        NES_SRAM = mymalloc(SRAMIN, i * 32);
        NES_RAM = mymalloc(SRAMIN, 0X800);  /* ����2K�ֽ�,����1024�ֽڶ��� */

        if ((uint32_t)NES_RAM % 1024)       /* ����1024�ֽڶ��� */
        {
            myfree(SRAMIN, NES_RAM);        /* �ͷ��ڴ�,Ȼ�����³��Է��� */
            myfree(SRAMIN, NES_SRAM);
        }
        else
        {
            myfree(SRAMIN, NES_SRAM);       /* �ͷ��ڴ� */
            break;
        }
    }

    NES_SRAM = mymalloc(SRAMIN, 0X2000);
    RomHeader = mymalloc(SRAMIN, sizeof(NES_header));
    NES_Mapper = mymalloc(SRAMIN, sizeof(MAPPER));
    spr_ram = mymalloc(SRAMIN, 0X100);
    ppu = mymalloc(SRAMIN, sizeof(ppu_data));
    apu = mymalloc(SRAMIN, sizeof(apu_t));  /* sizeof(apu_t)=  12588 */
    wave_buffers = mymalloc(SRAMIN, APU_PCMBUF_SIZE);

    for (i = 0; i < NES_APU_BUF_NUM; i++)
    {
        nesapusbuf[i] = mymalloc(SRAMEX, APU_PCMBUF_SIZE + 10); /* �����ڴ� */
    }

    romfile = mymalloc(SRAMEX, romsize);    /* ������Ϸrom�ռ�,����nes�ļ���С */

    if (romfile == NULL)    /* �ڴ治��?�ͷ�������ռ���ڴ�,���������� */
    {
        spb_delete();       /* �ͷ�SPBռ�õ��ڴ� */
        romfile = mymalloc(SRAMEX, romsize);    /* �������� */
    }

    if (i == 64 || !NES_RAM || !NES_SRAM || !RomHeader || !NES_Mapper || !spr_ram || !ppu || !apu || !wave_buffers || !nesapusbuf[NES_APU_BUF_NUM - 1] || !romfile)
    {
        nes_sram_free();
        return 1;
    }

    memset(NES_SRAM, 0, 0X2000);                /* ���� */
    memset(RomHeader, 0, sizeof(NES_header));   /* ���� */
    memset(NES_Mapper, 0, sizeof(MAPPER));      /* ���� */
    memset(spr_ram, 0, 0X100);                  /* ���� */
    memset(ppu, 0, sizeof(ppu_data));           /* ���� */
    memset(apu, 0, sizeof(apu_t));              /* ���� */
    memset(wave_buffers, 0, APU_PCMBUF_SIZE);   /* ���� */

    for (i = 0; i < NES_APU_BUF_NUM; i++)memset(nesapusbuf[i], 0, APU_PCMBUF_SIZE + 10); /* ���� */

    memset(romfile, 0, romsize);                /* ���� */
    return 0;
}

/**
 * @brief       Ƶ������
 * @param       plln            : ��Ƶ��
 * @retval      ��
 */
void nes_clock_set(uint8_t plln)
{
    uint16_t tpll = plln;
    uint8_t temp = 0;
    
    RCC->CFGR &= 0XFFFFFFFC;    /* �޸�ʱ��Ƶ��Ϊ�ڲ�8M */
    RCC->CR &= ~0x01000000;     /* PLL OFF */
    RCC->CFGR &= ~(0XF << 18);  /* ���ԭ�������� */
    plln -= 2;                  /* ����2����λ */
    
    RCC->CFGR |= plln << 18;    /* ����PLLֵ 2~16 */
    RCC->CFGR |= 1 << 16;       /* PLLSRC ON */
    
    if(tpll > 9)                /* ����72M��ʱ�� */
    {
        FLASH->ACR = 0x13;      /* FLASH 3����ʱ���� */
    }else
    {
        FLASH->ACR = 0x12;      /* FLASH 2����ʱ���� */
    }
    
    RCC->CR |= 0x01000000;      /* PLLON */

    while (!(RCC->CR >> 25));   /* �ȴ�PLL���� */

    RCC->CFGR |= 0x02;          /* PLL��Ϊϵͳʱ�� */

    while (temp != 0x02)        /* �ȴ�PLL��Ϊϵͳʱ�����óɹ� */
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }

    /* ˳��������ʱ�ʹ��� */
    delay_init(tpll * 8);           /* ��ʱ��ʼ�� */
    usart_init(tpll * 8, 115200);   /* ����1��ʼ�� */
}

extern volatile uint8_t nes_spped_para; /* NES��Ϸ����ʱ,����Դ�ֵ����,Ĭ��Ϊ0 */

/**
 * @brief       ��ʼnes��Ϸ
 * @param       pname           : nes��Ϸ·��
 * @retval      0,�����˳�
 *              1,�ڴ����
 *              2,�ļ�����
 *              3,��֧�ֵ�map
 */
uint8_t nes_load(uint8_t *pname)
{
    FIL *file;
    UINT br;
    uint8_t res = 0;
    file = mymalloc(SRAMIN, sizeof(FIL));

    if (file == 0)return 1; /* �ڴ�����ʧ�� */

    res = f_open(file, (char *)pname, FA_READ);

    if (res != FR_OK)       /* ���ļ�ʧ�� */
    {
        myfree(SRAMIN, file);
        return 2;
    }

    res = nes_sram_malloc(file->obj.objsize); /* �����ڴ� */

    if (res == 0)
    {
        f_read(file, romfile, file->obj.objsize, &br);                  /* ��ȡnes�ļ� */
        NESrom_crc32 = get_crc32(romfile + 16, file->obj.objsize - 16); /* ��ȡCRC32��ֵ */
        res = nes_load_rom();       /* ����ROM */

        if (res == 0)
        {
            nes_clock_set(16);      /* ��Ƶ��128M,�������MCU��������,���Կ��ǽ��ͳ�Ƶ */
            tpad_init(16);          /* ��������˳�, �ѷ�Ƶ���� */
            
            nes_spped_para = 1;     /* SPI�ٶȼ��� */
           
            joypad_init();
            cpu6502_init();         /* ��ʼ��6502,����λ */
            Mapper_Init();          /* map��ʼ�� */
            PPU_reset();            /* ppu��λ */
            apu_init();             /* apu��ʼ�� */
            
            nes_sound_open(0, APU_SAMPLE_RATE); /* ��ʼ�������豸 */
            
            nes_emulate_frame();    /* ����NESģ������ѭ�� */
            nes_sound_close();      /* �ر�������� */
            nes_clock_set(9);       /* 72M */
            tpad_init(6);
            
            nes_spped_para = 0;     /* SPI�ٶȻָ� */
        }
    }

    f_close(file);
    myfree(SRAMIN, file);   /* �ͷ��ڴ� */
    nes_sram_free();        /* �ͷ��ڴ� */
    return res;
}

uint8_t nes_xoff = 0;       /* ��ʾ��x�᷽���ƫ����(ʵ����ʾ���=256-2*nes_xoff) */

/**
 * @brief       ������Ϸ��ʾ����
 * @param       ��
 * @retval      ��
 */
void nes_set_window(void)
{
    uint16_t xoff = 0, yoff = 0;
    uint16_t lcdwidth, lcdheight;

    if (lcddev.width == 240)
    {
        lcdwidth = 240;
        lcdheight = 240;
        nes_xoff = (256 - lcddev.width) / 2;        /* �õ�x�᷽���ƫ���� */
        xoff = 0;
    }
    else if (lcddev.width == 320)
    {
        lcdwidth = 256;
        lcdheight = 240;
        nes_xoff = 0;
        xoff = (lcddev.width - 256) / 2;
    }
    else if (lcddev.width == 480)
    {
        lcdwidth = 480;
        lcdheight = 480;
        nes_xoff = (256 - (lcddev.width / 2)) / 2;  /* �õ�x�᷽���ƫ���� */
        xoff = 0;
    }

    yoff = (lcddev.height - lcdheight) / 2;         /* ��Ļ�߶� */
    lcd_set_window(xoff, yoff, lcdwidth, lcdheight);/* ��NESʼ������Ļ����������ʾ */
    lcd_write_ram_prepare();                        /* д��LCD RAM��׼�� */
}

extern void KEYBRD_FCPAD_Decode(uint8_t *fcbuf, uint8_t mode);

/**
 * @brief       ��ȡ��Ϸ�ֱ�����
 * @param       ��
 * @retval      ��
 */
void nes_get_gamepadval(void)
{
    PADdata0 = joypad_read();   /* ��ȡ�ֱ�1��ֵ */
    //PADdata1=0;               /* û���ֱ�2,�ʲ����� */
}

/**
 * @brief       nesģ������ѭ��
 * @param       ��
 * @retval      ��
 */
void nes_emulate_frame(void)
{
    uint8_t nes_frame;
    tim3_int_init(10000 - 1, 12800 - 1);    /* ����TIM3 ,1s�ж�һ�� */
    nes_set_window();                       /* ���ô��� */
    system_task_return = 0;

    while (1)
    {
        /*  LINES 0-239 */
        PPU_start_frame();

        for (NES_scanline = 0; NES_scanline < 240; NES_scanline++)
        {
            run6502(113 * 256);
            NES_Mapper->HSync(NES_scanline);

            /* ɨ��һ�� */
            if (nes_frame == 0)scanline_draw(NES_scanline);
            else do_scanline_and_dont_draw(NES_scanline);
        }

        NES_scanline = 240;
        run6502(113 * 256); /* ����1�� */
        NES_Mapper->HSync(NES_scanline);
        start_vblank();

        if (NMI_enabled())
        {
            cpunmi = 1;
            run6502(7 * 256); /* �����ж� */
        }

        NES_Mapper->VSync();

        /*  LINES 242-261 */
        for (NES_scanline = 241; NES_scanline < 262; NES_scanline++)
        {
            run6502(113 * 256);
            NES_Mapper->HSync(NES_scanline);
        }

        end_vblank();
        nes_get_gamepadval();   /* ÿ3֡��ѯһ��USB */
        apu_soundoutput();      /* �����Ϸ���� */
        framecnt++;
        nes_frame++;

        if (nes_frame > NES_SKIP_FRAME)
        {
            nes_frame = 0;      /* ��֡ */

            if (lcddev.id == 0X1963)nes_set_window();   /* ���贰�� */
        }

        if (system_task_return)
        {
            system_task_return = 0;
            break;
        }
    }

    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* �ָ���Ļ���� */
    TIM3->CR1 &= ~(1 << 0); /* �رն�ʱ��3 */
}

/* ��6502.s���汻���� */
void debug_6502(uint16_t reg0, uint8_t reg1)
{
    printf("6502 error:%x,%d\r\n", reg0, reg1);
}

/* nes,��Ƶ���֧�ֲ��� */
volatile uint16_t nesbufpos = 0;    /* ��ǰ������ */
volatile uint8_t nesplaybuf = 0;    /* �������ŵ���Ƶ֡������ */
volatile uint8_t nessavebuf = 0;    /* ��ǰ���浽����Ƶ������ */

/**
 * @brief       ��Ƶ���Żص�����
 * @param       ��
 * @retval      ��
 */
void nes_vs10xx_feeddata(void)
{
    uint8_t n;
    uint8_t nbytes;
    uint8_t *p;

    if (nesplaybuf == nessavebuf)return;    /* ��û���յ��µ���Ƶ���� */

    if (VS10XX_DQ != 0) /* ���Է������ݸ�VS10XX */
    {
        p = nesapusbuf[nesplaybuf] + nesbufpos;
        nesbufpos += 32;

        if (nesbufpos > APU_PCMBUF_SIZE)
        {
            nesplaybuf++;

            if (nesplaybuf > (NES_APU_BUF_NUM - 1))nesplaybuf = 0;

            nbytes = APU_PCMBUF_SIZE + 32 - nesbufpos;
            nesbufpos = 0;
        }
        else nbytes = 32;

        for (n = 0; n < nbytes; n++)
        {
            if (p[n] != 0)break;    /* �ж��ǲ���ʣ�����е����ݶ�Ϊ0? */
        }

        if (n == nbytes)return;     /* ����0,��ֱ�Ӳ�д��VS1053��,�������������� */

        VS10XX_XDCS(0);

        for (n = 0; n < nbytes; n++)
        {
            vs10xx_spi_read_write_byte(p[n]);
        }

        VS10XX_XDCS(1);
    }
}

/* NESģ����������VS1053���,ģ��WAV�����wavͷ���� */
const uint8_t nes_wav_head[] =
{
    0X52, 0X49, 0X46, 0X46, 0XFF, 0XFF, 0XFF, 0XFF, 0X57, 0X41, 0X56, 0X45, 0X66, 0X6D, 0X74, 0X20,
    0X10, 0X00, 0X00, 0X00, 0X01, 0X00, 0X01, 0X00, 0X11, 0X2B, 0X00, 0X00, 0X11, 0X2B, 0X00, 0X00,
    0X01, 0X00, 0X08, 0X00, 0X64, 0X61, 0X74, 0X61, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
};

/**
 * @brief       NES����Ƶ���
 * @param       samples_per_sync: ����ͬ��(δ�õ�)
 * @param       sample_rate     : ��Ƶ������
 * @retval      0, �ɹ�;
 *              ����, ʧ��;
 */
int nes_sound_open(int samples_per_sync, int sample_rate)
{
    uint8_t *p;
    uint8_t i;
    p = mymalloc(SRAMIN, 100);  /* ����100�ֽ��ڴ� */

    if (p == NULL)return 1;     /* �ڴ�����ʧ��,ֱ���˳� */

    printf("sound open:%d\r\n", sample_rate);

    for (i = 0; i < sizeof(nes_wav_head); i++)  /* ����nes_wav_head���� */
    {
        p[i] = nes_wav_head[i];
    }

    if (lcddev.width == 480)    /* ��480*480��Ļ */
    {
        sample_rate = 8000;     /* ����8Khz,Լԭ���ٶȵ�0.75�� */
    }
 
    p[24] = sample_rate & 0XFF; /* ���ò����� */
    p[25] = (sample_rate >> 8) & 0XFF;
    p[28] = sample_rate & 0XFF; /* �����ֽ�����(8λģʽ,���ڲ�����) */
    p[29] = (sample_rate >> 8) & 0XFF;
    nesplaybuf = 0;
    nessavebuf = 0;

    vs10xx_reset();             /* Ӳ��λ */
    vs10xx_soft_reset();        /* ��λ */
    vs10xx_set_all();           /* ���������Ȳ��� */
    vs10xx_reset_decode_time(); /* ��λ����ʱ�� */

    while (vs10xx_send_music_data(p));      /* ����wav head */

    while (vs10xx_send_music_data(p + 32)); /* ����wav head */

    tim6_int_init(100 - 1, 1280 - 1);       /* 1ms�ж�һ�� */

    myfree(SRAMIN, p);          /* �ͷ��ڴ� */
    return 0;
}

/**
 * @brief       NES�ر���Ƶ���
 * @param       ��
 * @retval      ��
 */
void nes_sound_close(void)
{
    TIM6->CR1 &= ~(1 << 0); /* �رն�ʱ��6 */
    vs10xx_set_speaker(0);  /* �ر�������� */
    vs10xx_set_volume(0);   /* ��������Ϊ0 */
}

/**
 * @brief       NES��Ƶ�����VS1053����
 * @param       samples         : ����(δ�õ�)
 * @param       wavebuf         : ��Ƶ���ݻ���
 * @retval      ��
 */
void nes_apu_fill_buffer(int samples, uint8_t *wavebuf)
{
    uint16_t i;
    uint8_t tbuf;

    for (i = 0; i < APU_PCMBUF_SIZE; i++)
    {
        nesapusbuf[nessavebuf][i] = wavebuf[i];
    }

    tbuf = nessavebuf;
    tbuf++;

    if (tbuf > (NES_APU_BUF_NUM - 1))tbuf = 0;

    while (tbuf == nesplaybuf)  /* ������ݸ�����Ƶ���ŵ�λ����,�ȴ� */
    {
        delay_ms(5);
    }

    nessavebuf = tbuf;
}



















