/**
 ****************************************************************************************************
 * @file        stmflash.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-26
 * @brief       STM32�ڲ�FLASH��д ��������
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
 * V1.0 20200426
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/STMFLASH/stmflash.h"

/**
 * @brief       ����STM32��FLASH
 * @param       ��
 * @retval      ��
 */
static void stmflash_unlock(void)
{
    FLASH->KEYR = STM32_FLASH_KEY1;     /* FLASH д���������. */
    FLASH->KEYR = STM32_FLASH_KEY2;
}

/**
 * @brief       flash����
 * @param       ��
 * @retval      ��
 */
static void stmflash_lock(void)
{
    FLASH->CR |= 1 << 7;    /* FLASH ���� */
}

/**
 * @brief       �õ�FLASH�Ĵ���״̬
 * @param       ��
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       1   : æ
 *   @arg       2   : ��̴���
 *   @arg       3   : д��������
 */
static uint8_t stmflash_get_error_status(void)
{
    uint32_t res;
    res = FLASH->SR;

    if (res & (1 << 0))return 1;    /* BSY = 1      , æ */
    if (res & (1 << 2))return 2;    /* PGERR = 1    , ��̴���*/
    if (res & (1 << 4))return 3;    /* WRPRTERR = 1 , д�������� */
    
    return 0;   /* û���κδ��� �������. */
}

/**
 * @brief       �ȴ��������
 * @param       time : Ҫ��ʱ�ĳ���
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       2   : ��̴���
 *   @arg       3   : д��������
 *   @arg       0XFF: ��ʱ
 */
static uint8_t stmflash_wait_done(uint32_t time)
{
    uint8_t res;

    do
    {
        res = stmflash_get_error_status();

        if (res != 1)
        {
            break;      /* ��æ, ����ȴ���, ֱ���˳� */
        }
        
        time--;
    } while (time);

    if (time == 0)res = 0XFF;   /* ��ʱ */

    return res;
}

/**
 * @brief       ��������
 * @param       saddr   : ������ַ 0 ~ 256
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       2   : ��̴���
 *   @arg       3   : д��������
 *   @arg       0XFF: ��ʱ
 */
static uint8_t stmflash_erase_sector(uint32_t saddr)
{
    uint8_t res = 0;
    saddr = (saddr * STM32_SECTOR_SIZE) + STM32_FLASH_BASE;   /* STM32F103������ʱ����ָ�����ֵ�ַ */
    res = stmflash_wait_done(0X5FFFFF);     /* �ȴ��ϴβ�������, >20ms */

    if (res == 0)
    {
        FLASH->CR |= 1 << 1;    /* ҳ���� */
        FLASH->AR = saddr;      /* ����ҳ��ַ(ʵ���ǰ��ֵ�ַ) */
        FLASH->CR |= 1 << 6;    /* ��ʼ���� */
        res = stmflash_wait_done(0X5FFFFF); /* �ȴ���������, >20ms */

        if (res != 1)   /* ��æ */
        {
            FLASH->CR &= ~(1 << 1); /* ���ҳ������־ */
        }
    }

    return res;
}

/**
 * @brief       ��FLASHָ����ַд����� (16λ����)
 *   @note      ����д�����, ��ָ2���ֽ�
 * @param       faddr   : д���ַ (�˵�ַ����Ϊ2�ı���!!)
 * @param       data    : Ҫд�������(16λ)
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       2   : ��̴���
 *   @arg       3   : д��������
 *   @arg       0XFF: ��ʱ
 */
static uint8_t stmflash_write_halfword(uint32_t faddr, uint16_t data)
{
    uint8_t res;
    res = stmflash_wait_done(0X5FFFFF);

    if (res == 0)       /* OK */
    {
        FLASH->CR |= 1 << 0;                /* ���ʹ�� */
        *(volatile uint16_t *)faddr = data; /* д������ */
        res = stmflash_wait_done(0X5FFFFF); /* �ȴ�������� */

        if (res != 1)   /* �����ɹ� */
        {
            FLASH->CR &= ~(1 << 0);         /* ���PGλ */
        }
    }

    return res;
}

/**
 * @brief       ��ָ����ַ��ȡһ������ (16λ����)
 * @param       faddr   : ��ȡ��ַ (�˵�ַ����Ϊ2�ı���!!)
 * @retval      ��ȡ�������� (16λ)
 */
uint16_t stmflash_read_halfword(uint32_t faddr)
{
    return *(volatile uint16_t *)faddr;
}

/**
 * @brief       ��FLASH ָ��λ��, д��ָ�����ȵ�����(�Զ�����)
 *   @note      �ú����� STM32 �ڲ� FLASH ָ��λ��д��ָ�����ȵ�����
 *              �ú������ȼ��Ҫд��������Ƿ��ǿ�(ȫ0XFFFF)��?, ���
 *              ����, ���Ȳ���, �����, ��ֱ������������д������.
 *              ��������, �������д��ʧ��.
 * @param       waddr   : ��ʼ��ַ (�˵�ַ����Ϊ2�ı���!!,����д�����!)
 * @param       pbuf    : ����ָ��
 * @param       length  : Ҫд��� ����(16λ)��
 * @retval      ��
 */

uint16_t g_flashbuf[STM32_SECTOR_SIZE / 2]; /* �����2K�ֽ� buffer */

void stmflash_write(uint32_t waddr, uint16_t *pbuf, uint16_t length)
{

    uint8_t status = 0;
    uint16_t secnum = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;

    if (waddr < STM32_FLASH_BASE || waddr % 2 ||        /* д���ַС�� STM32_FLASH_BASE, ����2��������, �Ƿ�. */
        waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE))  /* д���ַ���� STM32_FLASH_BASE + STM32_FLASH_SIZE, �Ƿ�. */
    {
        return;
    }
    
    stmflash_unlock();              /* FLASH���� */
    addrx = waddr;                  /* д�����ʼ��ַ */
    endaddr = waddr + length * 2;   /* д��Ľ�����ַ */

    while (addrx < endaddr)         /* ɨ��һ���ϰ� (�Է�0XFFFF�ĵط�, �Ȳ���) */
    {
        secnum = (addrx - STM32_FLASH_BASE) / STM32_SECTOR_SIZE;    /* �õ�������� */

        if (stmflash_read_halfword(addrx) != 0XFFFF)    /* �з�0XFFFF�ĵط�,Ҫ����������� */
        {
            status = stmflash_erase_sector(secnum);     /* ����������� */

            if (status) /* ���������� */
            {
                break;  /* �˳� */
            }
        }
        else
        {
            addrx += 4;
        }
    }

    if (status == 0)    /* ǰ��������� */
    {
        while (waddr < endaddr) /* д���� */
        {
            if (stmflash_write_halfword(waddr, *pbuf))  /* д������(ÿ��2���ֽ�) */
            {
                break;  /* д���쳣 */
            }

            waddr += 2; /* ��ַ��2 */
            pbuf++;     /* pbuf����, ��Ϊpbuf��16λ��, ����++�Ϳ����� */
        }
    }
    
    stmflash_lock();    /* ���� */
}

/**
 * @brief       ��ָ����ַ��ʼ����ָ�����ȵ�����
 * @param       raddr : ��ʼ��ַ
 * @param       pbuf  : ����ָ��
 * @param       length: Ҫ��ȡ�İ���(16λ)��,��2���ֽڵ�������
 * @retval      ��
 */
void stmflash_read(uint32_t raddr, uint16_t *pbuf, uint16_t length)
{
    uint16_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_halfword(raddr);    /* ��ȡ2���ֽ� */
        raddr += 2; /* ƫ��2���ֽ� */
    }
}

/******************************************************************************************/
/* �����ô��� */

/**
 * @brief       ����д����(д1����)
 * @param       waddr : ��ʼ��ַ
 * @param       wdata : Ҫд�������
 * @retval      ��ȡ��������
 */
void test_write(uint32_t waddr, uint16_t wdata)
{
    stmflash_write(waddr, &wdata, 1);   /* д��һ������ */
}


















