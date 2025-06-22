/**
 ****************************************************************************************************
 * @file        sdio_sdcard.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       SD�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200428
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SDIO/sdio_sdcard.h"


/* ���ڱ�.c�ļ�ʹ�õ�ȫ�ֱ���, ����ǰ׺ g_,
 * ����, Ϊ�˼����ϰ汾, �⼸��ȫ�ֱ����������ָĶ�
 */
static uint8_t CardType = STD_CAPACITY_SD_CARD_V1_1;    /* SD�����ͣ�Ĭ��Ϊ1.x���� */
static uint32_t CSD_Tab[4], CID_Tab[4], RCA = 0;        /* SD��CSD,CID�Լ���Ե�ַ(RCA)���� */


/* SD����Ϣ */
SD_CardInfo g_sd_card_info; 


/* sdmmc_read_disk/sdmmc_write_disk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
 * ��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
 */
__ALIGNED(4) uint8_t g_sd_data_buffer[512];


/**
 * @brief       ��ʼ��SD��
 * @param       ��
 * @retval      ��
 */
SD_Error sd_init(void)
{
    SD_Error errorstatus = SD_OK;
    uint8_t clkdiv = 0;
    /* SDIO IO�ڳ�ʼ�� */
    RCC->AHBENR |= 1 << 10;    /* SDIO ʱ��ʹ�� */
    SD_D0_GPIO_CLK_ENABLE();   /* D0����IOʱ��ʹ�� */
    SD_D1_GPIO_CLK_ENABLE();   /* D1����IOʱ��ʹ�� */
    SD_D2_GPIO_CLK_ENABLE();   /* D2����IOʱ��ʹ�� */
    SD_D3_GPIO_CLK_ENABLE();   /* D3����IOʱ��ʹ�� */
    SD_CLK_GPIO_CLK_ENABLE();  /* CLK����IOʱ��ʹ�� */
    SD_CMD_GPIO_CLK_ENABLE();  /* CMD����IOʱ��ʹ�� */

    sys_gpio_set(SD_D0_GPIO_PORT, SD_D0_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SD_D0����ģʽ���� */
    
    sys_gpio_set(SD_D1_GPIO_PORT, SD_D1_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SD_D1����ģʽ���� */
    
    sys_gpio_set(SD_D2_GPIO_PORT, SD_D2_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SD_D2����ģʽ���� */
                 
    sys_gpio_set(SD_D3_GPIO_PORT, SD_D3_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SD_D3����ģʽ���� */
                 
    sys_gpio_set(SD_CLK_GPIO_PORT, SD_CLK_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SD_CLK����ģʽ���� */
                 
    sys_gpio_set(SD_CMD_GPIO_PORT, SD_CMD_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* SD_CMD����ģʽ���� */

    /* SDIO����Ĵ�������ΪĬ��ֵ */
    SDIO->POWER = 0x00000000;
    SDIO->CLKCR = 0x00000000;
    SDIO->ARG = 0x00000000;
    SDIO->CMD = 0x00000000;
    SDIO->DTIMER = 0x00000000;
    SDIO->DLEN = 0x00000000;
    SDIO->DCTRL = 0x00000000;
    SDIO->ICR = 0X1FE00FFF;
    SDIO->MASK = 0x00000000;

    errorstatus = sdmmc_power_on(); /* SD���ϵ� */

    if (errorstatus == SD_OK)
    {
        errorstatus = sdmmc_initialize_cards(); /* ��ʼ��SD�� */
    }
    
    if (errorstatus == SD_OK)
    {
        errorstatus = sdmmc_get_card_info(&g_sd_card_info);  /* ��ȡ����Ϣ */
    }
    
    if (errorstatus == SD_OK)
    {
        errorstatus = sdmmc_select_deselect((uint32_t)(g_sd_card_info.RCA << 16));  /* ѡ��SD�� */
    }
    
    if (errorstatus == SD_OK)
    {
        errorstatus = sdmmc_wide_bus_operation(1);  /* 4λ���,�����MMC��,������4λģʽ */
    }
    
    if ((errorstatus == SD_OK) || (MULTIMEDIA_CARD == CardType))
    {
        if (g_sd_card_info.CardType == STD_CAPACITY_SD_CARD_V1_1 || g_sd_card_info.CardType == STD_CAPACITY_SD_CARD_V2_0)
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV + 4;     /* V1.1/V2.0����������� 72/[2*(1+4)]=7.2Mhz */
        }
        else 
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV;         /* SDHC����������������� 72/(2*2)=18Mhz */
        }
        
        /* ����ʱ��Ƶ��,SDMMCʱ�Ӽ��㹫ʽ: SDIO_CK ʱ�� = SDIOCLK / [2 * clkdiv]; ����, SDIOCLK һ��Ϊ72Mhz */
        sdmmc_clock_set(clkdiv);           
    }

    return errorstatus;
}

/**
 * @brief       SDIO ʱ������
 * @param       clkdiv : ʱ�ӷ�Ƶϵ��
 *   @note      CKʱ�� =  SDIOCLK / [2 * clkdiv]; (SDIOCLK ��һ��Ϊ72Mhz)
 * @retval      ��
 */
static void sdmmc_clock_set(uint16_t clkdiv)
{
    uint32_t tmpreg = SDIO->CLKCR;
    
    tmpreg &= 0XFFFFFF00;
    tmpreg |= clkdiv;
    SDIO->CLKCR = tmpreg;
}

/**
 * @brief       SDIO ���������
 * @param       cmdindex : ��������,����λ��Ч
 * @param       waitrsp  : �ڴ�����Ӧ.
 *   @arg       00/10, ����Ӧ
 *   @arg       01   , ����Ӧ
 *   @arg       11   , ����Ӧ
 * @param       arg      : �������
 * @retval      ��
 */
static void sdmmc_send_cmd(uint8_t cmdindex, uint8_t waitrsp, uint32_t arg)
{
    uint32_t tmpreg = 0;
    
    SDIO->ARG = arg;
    tmpreg |= cmdindex & 0X3F;          /* �����µ�index */
    tmpreg |= (uint32_t)waitrsp << 6;   /* �����µ�wait rsp */
    tmpreg |= 0 << 8;                   /* �޵ȴ� */
    tmpreg |= 1 << 10;                  /* ����ͨ��״̬��ʹ�� */
    SDIO->CMD = tmpreg;
}

/**
 * @brief       SDIO �����������ú���
 * @param       datatimeout : ��ʱʱ������
 * @param       datalen     : �������ݳ���,��25λ��Ч,����Ϊ���С��������
 * @param       blksize     : ���С. ʵ�ʴ�СΪ: 2^blksize�ֽ�
 * @param       dir         : ���ݴ��䷽��: 0, ����������; 1, ����������;
 * @retval      ��
 */
static void sdmmc_send_data_cfg(uint32_t datatimeout, uint32_t datalen, uint8_t blksize, uint8_t dir)
{
    uint32_t tmpreg;
    
    SDIO->DTIMER = datatimeout;
    SDIO->DLEN = datalen & 0X1FFFFFF;   /* ��25λ��Ч */
    tmpreg = SDIO->DCTRL;
    tmpreg &= 0xFFFFFF00;               /* ���֮ǰ������. */
    tmpreg |= blksize << 4;             /* ���ÿ��С */
    tmpreg |= 0 << 2;                   /* �����ݴ��� */
    tmpreg |= (dir & 0X01) << 1;        /* ������� */
    tmpreg |= 1 << 0;                   /* ���ݴ���ʹ��,DPSM״̬�� */
    SDIO->DCTRL = tmpreg;
}

/**
 * @brief       ���ϵ�
 *   @note      ��ѯ����SDIO�ӿ��ϵĿ��豸,����ѯ���ѹ������ʱ��
 * @param       ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_power_on(void)
{
    uint8_t i = 0;
    uint32_t tempreg = 0;
    SD_Error errorstatus = SD_OK;
    uint32_t response = 0, count = 0, validvoltage = 0;
    uint32_t SDType = SD_STD_CAPACITY;
    
    /* ����CLKCR�Ĵ��� */
    tempreg |= 0 << 9;      /* PWRSAV=0,��ʡ��ģʽ */
    tempreg |= 0 << 11;     /* WIDBUS[1:0]=0,1λ���ݿ�� */
    tempreg |= 0 << 13;     /* NEGEDGE=0,SDMMCCK�½��ظ������������ */
    tempreg |= 0 << 14;     /* HWFC_EN=0,�ر�Ӳ�������� */
    SDIO->CLKCR = tempreg;
    sdmmc_clock_set(SDIO_INIT_CLK_DIV); /* ����ʱ��Ƶ��(��ʼ����ʱ��,���ܳ���400Khz) */
    SDIO->POWER = 0X03;     /* �ϵ�״̬,������ʱ�� */
    SDIO->CLKCR |= 1 << 8;  /* SDIOCKʹ�� */

    for (i = 0; i < 74; i++)
    {
        sdmmc_send_cmd(SD_CMD_GO_IDLE_STATE, 0, 0); /* ����CMD0����IDLE STAGEģʽ����. */
        errorstatus = sdmmc_cmd_error();

        if (errorstatus == SD_OK) break;
    }

    if (errorstatus) return errorstatus; /* ���ش���״̬ */

    /* ����CMD8,����Ӧ,���SD���ӿ�����.
     * arg[11:8]:01,֧�ֵ�ѹ��Χ,2.7~3.6V
     * arg[7:0]:Ĭ��0XAA
     * ������Ӧ7
     */
    sdmmc_send_cmd(SD_SDMMC_SEND_IF_COND, 1, SD_CHECK_PATTERN); 
    
    errorstatus = sdmmc_cmd_resp7_error();      /* �ȴ�R7��Ӧ */

    if (errorstatus == SD_OK)                   /* R7��Ӧ���� */
    {
        CardType = STD_CAPACITY_SD_CARD_V2_0;   /* SD 2.0�� */
        SDType = SD_HIGH_CAPACITY;              /* �������� */
    }

    sdmmc_send_cmd(SD_CMD_APP_CMD, 1, 0);       /* ����CMD55,����Ӧ */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_APP_CMD);/* �ȴ�R1��Ӧ */

    if (errorstatus == SD_OK)   /* SD2.0/SD 1.1,����ΪMMC�� */
    {
        /* SD��,����ACMD41 SD_APP_OP_COND,����Ϊ:0x80100000 */
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            sdmmc_send_cmd(SD_CMD_APP_CMD, 1, 0);   /* ����CMD55,����Ӧ */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_APP_CMD);/* �ȴ�R1��Ӧ */

            if (errorstatus != SD_OK) return errorstatus;       /* ��Ӧ���� */

            sdmmc_send_cmd(SD_CMD_SD_APP_OP_COND, 1, SD_VOLTAGE_WINDOW_SD | SDType);    /* ����ACMD41,����Ӧ */
            errorstatus = sdmmc_cmd_resp3_error();              /* �ȴ�R3��Ӧ */

            if (errorstatus != SD_OK) return errorstatus;       /* ��Ӧ���� */

            response = SDIO->RESP1;   /* �õ���Ӧ */
            validvoltage = (((response >> 31) == 1) ? 1 : 0);   /* �ж�SD���ϵ��Ƿ���� */
            count++;
        }

        if (count >= SD_MAX_VOLT_TRIAL)
        {
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }

        if (response &= SD_HIGH_CAPACITY)
        {
            CardType = HIGH_CAPACITY_SD_CARD;
        }
    }
    else    /* MMC�� */
    {
        /* MMC��,����CMD1 SDMMC_SEND_OP_COND,����Ϊ:0x80FF8000 */
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            sdmmc_send_cmd(SD_CMD_SEND_OP_COND, 1, SD_VOLTAGE_WINDOW_MMC);  /* ����CMD1,����Ӧ */
            errorstatus = sdmmc_cmd_resp3_error();          /* �ȴ�R3��Ӧ */

            if (errorstatus != SD_OK) return errorstatus;    /* ��Ӧ���� */

            response = SDIO->RESP1;;  /* �õ���Ӧ */
            validvoltage = (((response >> 31) == 1) ? 1 : 0);
            count++;
        }

        if (count >= SD_MAX_VOLT_TRIAL)
        {
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }

        CardType = MULTIMEDIA_CARD;
    }

    return (errorstatus);
}

/**
 * @brief       ��ʼ�����еĿ�,���ÿ��������״̬
 * @param       ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_initialize_cards(void)
{
    SD_Error errorstatus = SD_OK;
    uint16_t rca = 0x01;

    if ((SDIO->POWER & 0X03) == 0)
    {
        return SD_REQUEST_NOT_APPLICABLE;               /* ����Դ״̬,ȷ��Ϊ�ϵ�״̬ */
    }
    
    if (SECURE_DIGITAL_IO_CARD != CardType)             /* ��SECURE_DIGITAL_IO_CARD */
    {
        sdmmc_send_cmd(SD_CMD_ALL_SEND_CID, 3, 0);      /* ����CMD2,ȡ��CID,����Ӧ */
        errorstatus = sdmmc_cmd_resp2_error();          /* �ȴ�R2��Ӧ */

        if (errorstatus != SD_OK) return errorstatus;   /* ��Ӧ���� */

        CID_Tab[0] = SDIO->RESP1;
        CID_Tab[1] = SDIO->RESP2;
        CID_Tab[2] = SDIO->RESP3;
        CID_Tab[3] = SDIO->RESP4;
    }
    
    /* �жϿ����� */
    if ((STD_CAPACITY_SD_CARD_V1_1 == CardType) || (STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SECURE_DIGITAL_IO_COMBO_CARD == CardType) || (HIGH_CAPACITY_SD_CARD == CardType))
    {
        sdmmc_send_cmd(SD_CMD_SET_REL_ADDR, 1, 0);      /* ����CMD3,����Ӧ */
        errorstatus = sdmmc_cmd_resp6_error(SD_CMD_SET_REL_ADDR, &rca); /* �ȴ�R6��Ӧ */

        if (errorstatus != SD_OK) return errorstatus;   /* ��Ӧ���� */
    }

    if (MULTIMEDIA_CARD == CardType)
    {
        sdmmc_send_cmd(SD_CMD_SET_REL_ADDR, 1, (uint32_t)(rca << 16)); /* ����CMD3,����Ӧ */
        errorstatus = sdmmc_cmd_resp2_error();          /* �ȴ�R2��Ӧ */

        if (errorstatus != SD_OK) return errorstatus;   /* ��Ӧ���� */
    }

    if (SECURE_DIGITAL_IO_CARD != CardType)             /* ��SECURE_DIGITAL_IO_CARD */
    {
        RCA = rca;
        sdmmc_send_cmd(SD_CMD_SEND_CSD, 3, (uint32_t)(rca << 16));  /* ����CMD9+��RCA,ȡ��CSD,����Ӧ */
        errorstatus = sdmmc_cmd_resp2_error();          /* �ȴ�R2��Ӧ */

        if (errorstatus != SD_OK) return errorstatus;   /* ��Ӧ���� */

        CSD_Tab[0] = SDIO->RESP1;
        CSD_Tab[1] = SDIO->RESP2;
        CSD_Tab[2] = SDIO->RESP3;
        CSD_Tab[3] = SDIO->RESP4;
    }

    return SD_OK;   /* ����ʼ���ɹ� */
}

/**
 * @brief       �õ�����Ϣ
 * @param       cardinfo : ����Ϣ�洢�ṹ��ָ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_get_card_info(SD_CardInfo *cardinfo)
{
    SD_Error errorstatus = SD_OK;
    uint8_t tmp = 0;
    cardinfo->CardType = (uint8_t)CardType;             /* ������ */
    cardinfo->RCA = (uint16_t)RCA;                      /* ��RCAֵ */
    tmp = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;     /* CSD�ṹ */
    cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;/* 2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨��� */
    cardinfo->SD_csd.Reserved1 = tmp & 0x03;            /* 2������λ */
    tmp = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);   /* ��1���ֽ� */
    cardinfo->SD_csd.TAAC = tmp;                        /* ���ݶ�ʱ��1 */
    tmp = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);    /* ��2���ֽ� */
    cardinfo->SD_csd.NSAC = tmp;                        /* ���ݶ�ʱ��2 */
    tmp = (uint8_t)(CSD_Tab[0] & 0x000000FF);           /* ��3���ֽ� */
    cardinfo->SD_csd.MaxBusClkFrec = tmp;               /* �����ٶ� */
    tmp = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);   /* ��4���ֽ� */
    cardinfo->SD_csd.CardComdClasses = tmp << 4;        /* ��ָ�������λ */
    tmp = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);   /* ��5���ֽ� */
    cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;  /* ��ָ�������λ */
    cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;           /* ����ȡ���ݳ��� */
    tmp = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);    /* ��6���ֽ� */
    cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7; /* ����ֿ�� */
    cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;   /* д���λ */
    cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;   /* �����λ */
    cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.Reserved2 = 0;                     /* ���� */
    
    /* ��׼1.1/2.0��/MMC�� */
    if ((CardType == STD_CAPACITY_SD_CARD_V1_1) || (CardType == STD_CAPACITY_SD_CARD_V2_0) || (MULTIMEDIA_CARD == CardType))
    {
        cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;   /* C_SIZE(12λ) */
        tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);           /* ��7���ֽ� */
        cardinfo->SD_csd.DeviceSize |= (tmp) << 2;
        tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);   /* ��8���ֽ� */
        cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;
        cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
        cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);
        tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);   /* ��9���ֽ� */
        cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
        cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
        cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1; /* C_SIZE_MULT */
        tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);    /* ��10���ֽ� */
        cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
        cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1);     /* ���㿨���� */
        cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
        cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);   /* ���С */
        cardinfo->CardCapacity *= cardinfo->CardBlockSize;
    }
    else if (CardType == HIGH_CAPACITY_SD_CARD) /* �������� */
    {
        tmp = (uint8_t)(CSD_Tab[1] & 0x000000FF);           /* ��7���ֽ� */
        cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;   /* C_SIZE */
        tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);   /* ��8���ֽ� */
        cardinfo->SD_csd.DeviceSize |= (tmp << 8);
        tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);   /* ��9���ֽ� */
        cardinfo->SD_csd.DeviceSize |= (tmp);
        tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);    /* ��10���ֽ� */
        cardinfo->CardCapacity = (long long)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024; /* ���㿨���� */
        cardinfo->CardBlockSize = 512;                      /* ���С�̶�Ϊ512�ֽ� */
    }

    cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;
    tmp = (uint8_t)(CSD_Tab[2] & 0x000000FF);               /* ��11���ֽ� */
    cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
    cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);
    tmp = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);       /* ��12���ֽ� */
    cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
    cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;
    tmp = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);       /* ��13���ֽ� */
    cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
    cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.Reserved3 = 0;
    cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);
    tmp = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);        /* ��14���ֽ� */
    cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
    cardinfo->SD_csd.ECC = (tmp & 0x03);
    tmp = (uint8_t)(CSD_Tab[3] & 0x000000FF);               /* ��15���ֽ� */
    cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
    cardinfo->SD_csd.Reserved4 = 1;
    tmp = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);       /* ��0���ֽ� */
    cardinfo->SD_cid.ManufacturerID = tmp;
    tmp = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);       /* ��1���ֽ� */
    cardinfo->SD_cid.OEM_AppliID = tmp << 8;
    tmp = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);      /* ��2���ֽ� */
    cardinfo->SD_cid.OEM_AppliID |= tmp;
    tmp = (uint8_t)(CID_Tab[0] & 0x000000FF);               /* ��3���ֽ� */
    cardinfo->SD_cid.ProdName1 = tmp << 24;
    tmp = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);       /* ��4���ֽ� */
    cardinfo->SD_cid.ProdName1 |= tmp << 16;
    tmp = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);       /* ��5���ֽ� */
    cardinfo->SD_cid.ProdName1 |= tmp << 8;
    tmp = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);        /* ��6���ֽ� */
    cardinfo->SD_cid.ProdName1 |= tmp;
    tmp = (uint8_t)(CID_Tab[1] & 0x000000FF);               /* ��7���ֽ� */
    cardinfo->SD_cid.ProdName2 = tmp;
    tmp = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);       /* ��8���ֽ� */
    cardinfo->SD_cid.ProdRev = tmp;
    tmp = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);       /* ��9���ֽ� */
    cardinfo->SD_cid.ProdSN = tmp << 24;
    tmp = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);        /* ��10���ֽ� */
    cardinfo->SD_cid.ProdSN |= tmp << 16;
    tmp = (uint8_t)(CID_Tab[2] & 0x000000FF);               /* ��11���ֽ� */
    cardinfo->SD_cid.ProdSN |= tmp << 8;
    tmp = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);       /* ��12���ֽ� */
    cardinfo->SD_cid.ProdSN |= tmp;
    tmp = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);       /* ��13���ֽ� */
    cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
    cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;
    tmp = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);        /* ��14���ֽ� */
    cardinfo->SD_cid.ManufactDate |= tmp;
    tmp = (uint8_t)(CID_Tab[3] & 0x000000FF);               /* ��15���ֽ� */
    cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
    cardinfo->SD_cid.Reserved2 = 1;
    return errorstatus;
}

/**
 * @brief       ����SDIO���߿��(MMC����֧��4bitģʽ)
 * @param       wmode  : λ��ģʽ
 *   @arg       0, 1λ���ݿ��;
 *   @arg       1, 4λ���ݿ��;
 *   @arg       2, 8λ���ݿ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_wide_bus_operation(uint32_t wmode)
{
    SD_Error errorstatus = SD_OK;
    uint16_t clkcr = 0;

    if (MULTIMEDIA_CARD == CardType)
    {
        return SD_UNSUPPORTED_FEATURE;      /* MMC����֧�� */
    }
    else if ((STD_CAPACITY_SD_CARD_V1_1 == CardType) || (STD_CAPACITY_SD_CARD_V2_0 == CardType) || (HIGH_CAPACITY_SD_CARD == CardType))
    {
        if (wmode >= 2)
        {
            return SD_UNSUPPORTED_FEATURE;  /* ��֧��8λģʽ */
        }
        else
        {
            errorstatus = sdmmc_wide_bus_enable(wmode);

            if (SD_OK == errorstatus)
            {
                clkcr = SDIO->CLKCR;    /* ��ȡCLKCR��ֵ */
                clkcr &= ~(3 << 11);    /* ���֮ǰ��λ������ */
                clkcr |= (uint32_t)wmode << 11; /* 1λ/4λ���߿�� */
                clkcr |= 0 << 14;       /* ������Ӳ�������� */
                SDIO->CLKCR = clkcr;    /* ��������CLKCRֵ */
            }
        }
    }

    return errorstatus;
}

/**
 * @brief       ��ѡ��
 *   @note      ����CMD7,ѡ����Ե�ַ(rca)Ϊaddr�Ŀ�,ȡ��������.���Ϊ0,�򶼲�ѡ��.
 * @param       addr : ����RCA��ַ
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_select_deselect(uint32_t addr)
{
    sdmmc_send_cmd(SD_CMD_SEL_DESEL_CARD, 1, addr); /* ����CMD7,ѡ��,����Ӧ */
    return sdmmc_cmd_resp1_error(SD_CMD_SEL_DESEL_CARD);
}

/**
 * @brief       SDIO ��ȡ����/�����
 * @param       pbuf    : �����ݻ�����
 * @param       addr    : ����ַ
 * @param       blksize : ���С
 * @param       nblks   : Ҫ���Ŀ���, 1,��ʾ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_read_blocks(uint8_t *pbuf, long long addr, uint16_t blksize, uint32_t nblks)
{
    SD_Error errorstatus = SD_OK;
    uint32_t count = 0;
    uint32_t timeout = SDMMC_DATATIMEOUT;
    uint32_t *tempbuff = (uint32_t *)pbuf;  /* ת��Ϊuint32_tָ�� */
    SDIO->DCTRL = 0x0;                    /* ���ݿ��ƼĴ�������(��DMA) */

    if (CardType == HIGH_CAPACITY_SD_CARD)  /* �������� */
    {
        blksize = 512;
        addr >>= 9;
    }

    sdmmc_send_cmd(SD_CMD_SET_BLOCKLEN, 1, blksize);            /* ����CMD16+�������ݳ���Ϊblksize,����Ӧ */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SET_BLOCKLEN);   /* �ȴ�R1��Ӧ */

    if (errorstatus != SD_OK)return errorstatus;                /* ��Ӧ���� */

    sdmmc_send_data_cfg(SD_DATATIMEOUT, nblks * blksize, 9, 1); /* nblks*blksize,���С��Ϊ512,���������� */

    if (nblks > 1)  /* ���� */
    {
        sdmmc_send_cmd(SD_CMD_READ_MULT_BLOCK, 1, addr);        /* ����CMD18+��addr��ַ����ȡ����,����Ӧ */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_READ_MULT_BLOCK);    /* �ȴ�R1��Ӧ */

        if (errorstatus != SD_OK)
        {
            printf("SD_CMD_READ_MULT_BLOCK Error\r\n");
            return errorstatus; /* ��Ӧ���� */
        }
    }
    else    /* ����� */
    {
        sdmmc_send_cmd(SD_CMD_READ_SINGLE_BLOCK, 1, addr);      /* ����CMD17+��addr��ַ����ȡ����,����Ӧ */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_READ_SINGLE_BLOCK);  /* �ȴ�R1��Ӧ */

        if (errorstatus != SD_OK)return errorstatus;            /* ��Ӧ���� */
    }

    sys_intx_disable();/* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDMMC��д����!!!) */

    while (!(SDIO->STA & ((1 << 5) | (1 << 1) | (1 << 3) | (1 << 8))))    /* ������/CRC/��ʱ/���(��־) */
    {
        if (SDIO->STA & (1 << 15))            /* ����������,��ʾ���ٴ���8���� */
        {
            for (count = 0; count < 8; count++) /* ѭ����ȡ���� */
            {
                *(tempbuff + count) = SDIO->FIFO;
            }

            tempbuff += 8;
            timeout = SDMMC_DATATIMEOUT;    /* ���������ʱ�� */
        }
        else    /* ����ʱ */
        {
            if (timeout == 0)
            {
                //printf("r fifo time out\r\n");
                SDIO->ICR = 0X1FE00FFF;     /* ������б�� */
                sys_intx_enable();          /* �������ж� */
                return SD_DATA_TIMEOUT;
            }

            timeout--;
        }
    }

    sys_intx_enable();              /* �������ж� */

    if (SDIO->STA & (1 << 3))       /* ���ݳ�ʱ���� */
    {
        SDIO->ICR |= 1 << 3;        /* ������־ */
        return SD_DATA_TIMEOUT;
    }
    else if (SDIO->STA & (1 << 1))  /* ���ݿ�CRC���� */
    {
        SDIO->ICR |= 1 << 1;        /* ������־ */

        if (nblks > 1)              /* ��Կ��ܳ��ֵ�CRC����,����Ƕ���ȡ,���뷢�ͽ�����������! */
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0); /* ����CMD12+�������� */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  /* �ȴ�R1��Ӧ */
        }

        return SD_DATA_CRC_FAIL;
    }
    else if (SDIO->STA & (1 << 5))  /* ����fifo������� */
    {
        SDIO->ICR |= 1 << 5;        /* ������־ */
        return SD_RX_OVERRUN;
    }

    if ((SDIO->STA & (1 << 8)) && (nblks > 1))    /* �����ս���,���ͽ���ָ�� */
    {
        if ((STD_CAPACITY_SD_CARD_V1_1 == CardType) || (STD_CAPACITY_SD_CARD_V2_0 == CardType) || (HIGH_CAPACITY_SD_CARD == CardType))
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0); /* ����CMD12+�������� */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  /* �ȴ�R1��Ӧ */

            if (errorstatus != SD_OK)return errorstatus;
        }
    }

    SDIO->ICR = 0X1FE00FFF;       /* ������б�� */
    return errorstatus;
}

/**
 * @brief       SDIO д����/�����
 * @param       pbuf    : д���ݻ�����
 * @param       addr    : д��ַ
 * @param       blksize : ���С
 * @param       nblks   : Ҫд�Ŀ���, 1,��ʾд������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_write_blocks(uint8_t *pbuf, long long addr, uint16_t blksize, uint32_t nblks)
{
    SD_Error errorstatus = SD_OK;
    uint8_t  cardstate = 0;
    uint32_t timeout = 0, bytestransferred = 0;
    uint32_t cardstatus = 0, count = 0, restwords = 0;
    uint32_t tlen = nblks * blksize;                /* �ܳ���(�ֽ�) */
    uint32_t *tempbuff = (uint32_t *)pbuf;

    if (pbuf == NULL)return SD_INVALID_PARAMETER;   /* �������� */

    SDIO->DCTRL = 0x0;                              /* ���ݿ��ƼĴ�������(��DMA) */

    if (CardType == HIGH_CAPACITY_SD_CARD)          /* �������� */
    {
        blksize = 512;
        addr >>= 9;
    }

    sdmmc_send_cmd(SD_CMD_SET_BLOCKLEN, 1, blksize);/* ����CMD16+�������ݳ���Ϊblksize,����Ӧ */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SET_BLOCKLEN);   /* �ȴ�R1��Ӧ */

    if (errorstatus != SD_OK)return errorstatus;    /* ��Ӧ���� */

    if (nblks > 1)  /* ���д */
    {
        if (nblks * blksize > SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;

        if ((STD_CAPACITY_SD_CARD_V1_1 == CardType) || (STD_CAPACITY_SD_CARD_V2_0 == CardType) || (HIGH_CAPACITY_SD_CARD == CardType))
        {
            /* �������*/
            sdmmc_send_cmd(SD_CMD_APP_CMD, 1, (uint32_t)RCA << 16); /* ����ACMD55,����Ӧ */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_APP_CMD);    /* �ȴ�R1��Ӧ */

            if (errorstatus != SD_OK)return errorstatus;

            sdmmc_send_cmd(SD_CMD_SET_BLOCK_COUNT, 1, nblks);       /* ����CMD23,���ÿ�����,����Ӧ */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SET_BLOCK_COUNT);    /* �ȴ�R1��Ӧ */

            if (errorstatus != SD_OK)return errorstatus;
        }

        sdmmc_send_cmd(SD_CMD_WRITE_MULT_BLOCK, 1, addr);   /* ����CMD25,���дָ��,����Ӧ */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_WRITE_MULT_BLOCK);   /* �ȴ�R1��Ӧ */
    }
    else    /* ����д */
    {
        sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, (uint32_t)RCA << 16); /* ����CMD13,��ѯ����״̬,����Ӧ */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SEND_STATUS);    /* �ȴ�R1��Ӧ */

        if (errorstatus != SD_OK)return errorstatus;

        cardstatus = SDIO->RESP1;
        timeout = SD_DATATIMEOUT;

        while (((cardstatus & 0x00000100) == 0) && (timeout > 0))   /* ���READY_FOR_DATAλ�Ƿ���λ */
        {
            timeout--;
            sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, (uint32_t)RCA << 16); /* ����CMD13,��ѯ����״̬,����Ӧ */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SEND_STATUS);    /* �ȴ�R1��Ӧ */

            if (errorstatus != SD_OK)return errorstatus;

            cardstatus = SDIO->RESP1;
        }

        if (timeout == 0)return SD_ERROR;

        sdmmc_send_cmd(SD_CMD_WRITE_SINGLE_BLOCK, 1, addr);             /* ����CMD24,д����ָ��,����Ӧ */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_WRITE_SINGLE_BLOCK); /* �ȴ�R1��Ӧ */
    }

    if (errorstatus != SD_OK)return errorstatus;

    sdmmc_send_data_cfg(SD_DATATIMEOUT, nblks * blksize, 9, 0); /* blksize,���С��Ϊ512�ֽ�,���������� */
    timeout = SDMMC_DATATIMEOUT;
    sys_intx_disable(); /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDMMC��д����!!!) */

    while (!(SDIO->STA & ((1 << 4) | (1 << 1) | (1 << 8) | (1 << 3))))    /* ����/CRC/���ݽ���/��ʱ */
    {
        if (SDIO->STA & (1 << 14))                            /* ���������,��ʾ���ٴ���8��(32�ֽ�) */
        {
            if ((tlen - bytestransferred) < SD_HALFFIFOBYTES)   /* ����32�ֽ��� */
            {
                restwords = ((tlen - bytestransferred) % 4 == 0) ? ((tlen - bytestransferred) / 4) : ((tlen - bytestransferred) / 4 + 1);

                for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
                {
                    SDIO->FIFO = *tempbuff;
                }
            }
            else    /* ���������,���Է�������8��(32�ֽ�)���� */
            {
                for (count = 0; count < SD_HALFFIFO; count++)
                {
                    SDIO->FIFO = *(tempbuff + count);
                }

                tempbuff += SD_HALFFIFO;
                bytestransferred += SD_HALFFIFOBYTES;
            }

            timeout = SDMMC_DATATIMEOUT;    /* д�������ʱ�� */
        }
        else
        {
            if (timeout == 0)
            {
                //printf("w fifo time out\r\n");
                SDIO->ICR = 0X1FE00FFF; /* ������б�� */
                sys_intx_enable();      /* �������ж� */
                return SD_DATA_TIMEOUT;
            }

            timeout--;
        }
    }

    sys_intx_enable();              /* �������ж� */

    if (SDIO->STA & (1 << 3))       /* ���ݳ�ʱ���� */
    {
        SDIO->ICR |= 1 << 3;        /* ������־ */
        return SD_DATA_TIMEOUT;
    }
    else if (SDIO->STA & (1 << 1))  /* ���ݿ�CRC���� */
    {
        SDIO->ICR |= 1 << 1;        /* ������־ */

        if (nblks > 1)              /* ��Կ��ܳ��ֵ�CRC����,����Ƕ���ȡ,���뷢�ͽ�����������! */
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0); /* ����CMD12+�������� */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  /* �ȴ�R1��Ӧ */
        }

        return SD_DATA_CRC_FAIL;
    }
    else if (SDIO->STA & (1 << 4))    /* ����fifo������� */
    {
        SDIO->ICR |= 1 << 4;          /* ������־ */
        return SD_TX_UNDERRUN;
    }

    if ((SDIO->STA & (1 << 8)) && (nblks > 1))    /* ��鷢�ͽ���,���ͽ���ָ�� */
    {
        if ((STD_CAPACITY_SD_CARD_V1_1 == CardType) || (STD_CAPACITY_SD_CARD_V2_0 == CardType) || (HIGH_CAPACITY_SD_CARD == CardType))
        {
            sdmmc_send_cmd(SD_CMD_STOP_TRANSMISSION, 1, 0); /* ����CMD12+�������� */
            errorstatus = sdmmc_cmd_resp1_error(SD_CMD_STOP_TRANSMISSION);  /* �ȴ�R1��Ӧ */

            if (errorstatus != SD_OK)return errorstatus;
        }
    }

    SDIO->ICR = 0X1FE00FFF;           /* ������б�� */
    errorstatus = sdmmc_is_card_programming(&cardstate);

    while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
    {
        errorstatus = sdmmc_is_card_programming(&cardstate);
    }

    return errorstatus;
}

/**
 * @brief       ���CMD0��ִ��״̬
 * @param       ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_cmd_error(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t timeout = SDMMC_CMD0TIMEOUT;

    while (timeout--)
    {
        if (SDIO->STA & (1 << 7))
        {
            break;  /* �����ѷ���(������Ӧ) */
        }
    }

    if (timeout == 0) return SD_CMD_RSP_TIMEOUT;

    SDIO->ICR = 0X1FE00FFF;   /* ������ */
    return errorstatus;
}

/**
 * @brief       ���R1��Ӧ�Ĵ���״̬
 * @param       cmd : ��ǰ����
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_cmd_resp1_error(uint8_t cmd)
{
    uint32_t status;
    uint32_t timeout = SDMMC_CMD1TIMEOUT;

    while (timeout--)
    {
        status = SDIO->STA;

        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;      /* CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�) */
        }
    }

    if ((timeout == 0) || (status & (1 << 2)))  /* ��Ӧ��ʱ */
    {
        SDIO->ICR = 1 << 2;         /* ���������Ӧ��ʱ��־ */
        SDIO->ICR = 0X1FE00FFF;     /* ������ */
        return SD_CMD_RSP_TIMEOUT;
    }

    if (status & (1 << 0))          /* CRC���� */
    {
        SDIO->ICR = 1 << 0;         /* �����־ */
        return SD_CMD_CRC_FAIL;
    }

    if (SDIO->RESPCMD != cmd)
    {
        SDIO->ICR = 0X1FE00FFF;     /* ������ */
        return SD_ILLEGAL_CMD;      /* ���ƥ�� */
    }

    SDIO->ICR = 0X1FE00FFF;         /* ������ */
    return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS);    /* ���ؿ���Ӧ */
}

/**
 * @brief       ���R2��Ӧ�Ĵ���״̬
 * @param       ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_cmd_resp2_error(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t timeout = SDMMC_CMD1TIMEOUT;

    while (timeout--)
    {
        status = SDIO->STA;

        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;      /* CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�) */
        }
    }

    if ((timeout == 0) || (status & (1 << 2)))  /* ��Ӧ��ʱ */
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO->ICR |= 1 << 2;  /* ���������Ӧ��ʱ��־ */
        return errorstatus;
    }

    if (status & 1 << 0)        /* CRC���� */
    {
        errorstatus = SD_CMD_CRC_FAIL;
        SDIO->ICR |= 1 << 0;    /* �����Ӧ��־ */
    }

    SDIO->ICR = 0X1FE00FFF;     /* ������ */
    return errorstatus;
}

/**
 * @brief       ���R3��Ӧ�Ĵ���״̬
 * @param       ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_cmd_resp3_error(void)
{
    uint32_t status;
    uint32_t timeout = SDMMC_CMD1TIMEOUT;

    while (timeout--)
    {
        status = SDIO->STA;

        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;  /* CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�) */
        }
    }

    if ((timeout == 0) || (status & (1 << 2)))  /* ��Ӧ��ʱ */
    {
        SDIO->ICR |= 1 << 2;    /* ���������Ӧ��ʱ��־ */
        return SD_CMD_RSP_TIMEOUT;
    }

    SDIO->ICR = 0X1FE00FFF;     /* ������ */
    return SD_OK;
}

/**
 * @brief       ���R6��Ӧ�Ĵ���״̬
 * @param       cmd : ��ǰ����
 * @param       prca: �����ص�RCA��ַ
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_cmd_resp6_error(uint8_t cmd, uint16_t *prca)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t rspr1;
    uint32_t timeout = SDMMC_CMD1TIMEOUT;

    while (timeout--)
    {
        status = SDIO->STA;

        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;      /* CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�) */
        }
    }

    if (status & (1 << 2))      /* ��Ӧ��ʱ */
    {
        SDIO->ICR |= 1 << 2;    /* ���������Ӧ��ʱ��־ */
        return SD_CMD_RSP_TIMEOUT;
    }

    if ((timeout == 0) || (status & (1 << 2)))  /* CRC���� */
    {
        SDIO->ICR |= 1 << 0;    /* �����Ӧ��־ */
        return SD_CMD_CRC_FAIL;
    }

    if (SDIO->RESPCMD != cmd)   /* �ж��Ƿ���Ӧcmd���� */
    {
        return SD_ILLEGAL_CMD;
    }

    SDIO->ICR = 0X1FE00FFF;     /* ������б�� */
    rspr1 = SDIO->RESP1;        /* �õ���Ӧ */

    if (SD_ALLZERO == (rspr1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
    {
        *prca = (uint16_t)(rspr1 >> 16);    /* ����16λ�õ�,rca */
        return errorstatus;
    }

    if (rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR) return SD_GENERAL_UNKNOWN_ERROR;

    if (rspr1 & SD_R6_ILLEGAL_CMD) return SD_ILLEGAL_CMD;

    if (rspr1 & SD_R6_COM_CRC_FAILED) return SD_COM_CRC_FAILED;

    return errorstatus;
}

/**
 * @brief       ���R7��Ӧ�Ĵ���״̬
 * @param       ��
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_cmd_resp7_error(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t timeout = SDMMC_CMD0TIMEOUT;

    while (timeout--)
    {
        status = SDIO->STA;

        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;      /* CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�) */
        }
    }

    if ((timeout == 0) || (status & (1 << 2)))  /* ��Ӧ��ʱ */
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;       /* ��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ */
        SDIO->ICR |= 1 << 2;    /* ���������Ӧ��ʱ��־ */
        return errorstatus;
    }

    if (status & 1 << 6)        /* �ɹ����յ���Ӧ */
    {
        errorstatus = SD_OK;
        SDIO->ICR |= 1 << 6;    /* �����Ӧ��־ */
    }

    return errorstatus;
}

/**
 * @brief       SDIO ʹ�ܿ�����ģʽ
 * @param       enx : 0, ��ʹ��; 1, ʹ��;
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_wide_bus_enable(uint8_t enx)
{
    SD_Error errorstatus = SD_OK;
    uint32_t scr[2] = {0, 0};
    uint8_t arg = 0X00;

    if (enx)arg = 0X02;
    else arg = 0X00;

    if (SDIO->RESP1 & SD_CARD_LOCKED) return SD_LOCK_UNLOCK_FAILED;    /* SD������LOCKED״̬ */

    errorstatus = sdmmc_find_scr(RCA, scr);        /* �õ�SCR�Ĵ������� */

    if (errorstatus != SD_OK) return errorstatus;

    if ((scr[1]&SD_WIDE_BUS_SUPPORT) != SD_ALLZERO) /* ֧�ֿ����� */
    {
        sdmmc_send_cmd(SD_CMD_APP_CMD, 1, (uint32_t)RCA << 16); /* ����CMD55+RCA,����Ӧ */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_APP_CMD);

        if (errorstatus != SD_OK) return errorstatus;

        sdmmc_send_cmd(SD_CMD_APP_SD_SET_BUSWIDTH, 1, arg);     /* ����ACMD6,����Ӧ,����:10,4λ;00,1λ. */
        errorstatus = sdmmc_cmd_resp1_error(SD_CMD_APP_SD_SET_BUSWIDTH);
        return errorstatus;
    }
    else
    {
        return SD_REQUEST_NOT_APPLICABLE;   /* ��֧�ֿ��������� */
    }
}

/**
 * @brief       SDIO ��鿨�Ƿ�����ִ��д����
 * @param       pstatus : ��ǰ״̬.
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_is_card_programming(uint8_t *pstatus)
{
    volatile uint32_t respR1 = 0, status = 0;
    sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, (uint32_t)RCA << 16); /* ����CMD13 */
    status = SDIO->STA;

    while (!(status & ((1 << 0) | (1 << 6) | (1 << 2))))status = SDIO->STA; /* �ȴ�������� */

    if (status & (1 << 0))      /* CRC���ʧ�� */
    {
        SDIO->ICR |= 1 << 0;    /* ��������� */
        return SD_CMD_CRC_FAIL;
    }

    if (status & (1 << 2))      /* ���ʱ */
    {
        SDIO->ICR |= 1 << 2;    /* ��������� */
        return SD_CMD_RSP_TIMEOUT;
    }

    if (SDIO->RESPCMD != SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;

    SDIO->ICR = 0X1FE00FFF;     /* ������б�� */
    respR1 = SDIO->RESP1;
    *pstatus = (uint8_t)((respR1 >> 9) & 0x0000000F);
    return SD_OK;
}

/**
 * @brief       SDIO ��ȡ��ǰ��״̬
 * @param       pstatus : ��ǰ״̬.
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_send_status(uint32_t *pstatus)
{
    SD_Error errorstatus = SD_OK;

    if (pstatus == NULL)
    {
        errorstatus = SD_INVALID_PARAMETER;
        return errorstatus;
    }

    sdmmc_send_cmd(SD_CMD_SEND_STATUS, 1, RCA << 16);	/* ����CMD13,����Ӧ */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SEND_STATUS);	/* ��ѯ��Ӧ״̬ */

    if (errorstatus != SD_OK)return errorstatus;

    *pstatus = SDIO->RESP1;     /* ��ȡ��Ӧֵ */
    return errorstatus;
}

/**
 * @brief       ����SD����״̬
 * @param       pstatus : ��ǰ״̬.
 * @retval      SD��״̬(���SDCardState����)
 */
SDCardState sdmmc_get_status(void)
{
    uint32_t resp1 = 0;

    if (sdmmc_send_status(&resp1) != SD_OK)
    {
        return SD_CARD_ERROR;
    }
    else 
    {
        return (SDCardState)((resp1 >> 9) & 0x0F);
    }
}

/**
 * @brief       SDIO ����SD����SCR�Ĵ���ֵ
 * @param       rca  : ����Ե�ַ
 * @param       pscr : ���ݻ�����(�洢SCR����)
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
static SD_Error sdmmc_find_scr(uint16_t rca, uint32_t *pscr)
{
    SD_Error errorstatus = SD_OK;
    uint32_t tempscr[2] = {0, 0};
    sdmmc_send_cmd(SD_CMD_SET_BLOCKLEN, 1, 8);      /* ����CMD16,����Ӧ,����Block SizeΪ8�ֽ� */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SET_BLOCKLEN);

    if (errorstatus != SD_OK)return errorstatus;

    sdmmc_send_cmd(SD_CMD_APP_CMD, 1, (uint32_t)rca << 16); /* ����CMD55,����Ӧ */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_APP_CMD);

    if (errorstatus != SD_OK)return errorstatus;

    sdmmc_send_data_cfg(SD_DATATIMEOUT, 8, 3, 1);   /* 8���ֽڳ���,blockΪ8�ֽ�,SD����SDMMC. */
    sdmmc_send_cmd(SD_CMD_SD_APP_SEND_SCR, 1, 0);   /* ����ACMD51,����Ӧ,����Ϊ0 */
    errorstatus = sdmmc_cmd_resp1_error(SD_CMD_SD_APP_SEND_SCR);

    if (errorstatus != SD_OK)return errorstatus;

    while (!(SDIO->STA & (SDIO_STA_RXOVERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_DBCKEND | SDIO_STA_DATAEND)))
    {
        if (!(SDIO->STA & (1 << 19))) /* ����FIFO���ݿ��� */
        {
            tempscr[0] = SDIO->FIFO;  /* ��ȡFIFO���� */
            tempscr[1] = SDIO->FIFO;  /* ��ȡFIFO���� */
            break;
        }
    }

    if (SDIO->STA & (1 << 3))         /* �������ݳ�ʱ */
    {
        SDIO->ICR |= 1 << 3;          /* ������ */
        return SD_DATA_TIMEOUT;
    }
    else if (SDIO->STA & (1 << 1))    /* �ѷ���/���յ����ݿ�CRCУ����� */
    {
        SDIO->ICR |= 1 << 1;          /* ������ */
        return SD_DATA_CRC_FAIL;
    }
    else if (SDIO->STA & (1 << 5))    /* ����FIFO��� */
    {
        SDIO->ICR |= 1 << 5;          /* ������ */
        return SD_RX_OVERRUN;
    }

    SDIO->ICR = 0X1FE00FFF;           /* ������ */
    /* ������˳��8λΪ��λ������. */
    *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);
    *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);
    return errorstatus;
}

/**
 * @brief       ��SD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = SD_OK;
    long long lsaddr = saddr;
    uint32_t n;

    if (CardType != STD_CAPACITY_SD_CARD_V1_1)lsaddr <<= 9;

    if ((uint32_t)pbuf % 4 != 0)/* ���ݻ������׵�ַ������4�ı��� */
    {
        for (n = 0; n < cnt; n++)
        {
            sta = sdmmc_read_blocks(g_sd_data_buffer, lsaddr + 512 * n, 512, 1);    /* ����sector�Ķ����� */

            if (sta)break;      /* �����ȡ����,���˳� */

            memcpy(pbuf, g_sd_data_buffer, 512);
            pbuf += 512;
        }
    }
    else
    {
        sta = sdmmc_read_blocks(pbuf, lsaddr, 512, cnt);    /* ��ȡ����/���sector */
    }

    return sta;
}

/**
 * @brief       дSD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = SD_OK;
    uint32_t n;
    long long lsaddr = saddr;

    if (CardType != STD_CAPACITY_SD_CARD_V1_1)lsaddr <<= 9;

    if ((uint32_t)pbuf % 4 != 0)/* ���ݻ������׵�ַ������4�ı��� */
    {
        for (n = 0; n < cnt; n++)
        {
            memcpy(g_sd_data_buffer, pbuf, 512);
            sta = sdmmc_write_blocks(g_sd_data_buffer, lsaddr + 512 * n, 512, 1); /* ����sector��д���� */

            if (sta)break;      /* ���д�����,���˳� */

            pbuf += 512;
        }
    }
    else
    {
        sta = sdmmc_write_blocks(pbuf, lsaddr, 512, cnt);   /* д����/���sector */
    }
    
    return sta;
}







