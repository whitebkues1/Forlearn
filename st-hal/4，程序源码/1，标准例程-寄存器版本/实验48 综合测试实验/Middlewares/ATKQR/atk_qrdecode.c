/**
 ****************************************************************************************************
 * @file        atk_qrdecode.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-06-02
 * @brief       ��ά��ʶ��� ����
 *              ����ά��&����ʶ��LIB��ALIENTEK�ṩ,������ʹ��.
 *              �ö�ά��&����ʶ��LIB����:
 *              1,֧��QR��ά��ʶ��.
 *              2,֧��CODE128,CODE39,I25,EAN13�����ֱ��뷽ʽ������ʶ��.
 *              3,֧��UTF8-OEMת�����(��Ҫ�ͻ��Լ��ṩת�����).
 *
 *              ����Ҫ֧��UTF82OEMת�������ʱ��:
 *              1,��ATK_QR_UTF82OEM_SUPPORT���ֵ��Ϊ1(Ĭ����1)
 *              2,ʵ��atk_qr_convert����.�ú�����unicode����ת��ΪOEM����.
 *              ����,������,����OEM������ַ���(һ�����GBK����),�����ڵ�Ƭ��ϵͳ������ʾ.
 *              �������Ҫ֧��OEMת��,��:��ATK_QR_UTF82OEM_SUPPORT���ֵ��Ϊ0����,��������������ԭʼ����
 *              ���ַ���(UTF8/GBK).���������,�ڵ�Ƭ��ϵͳ����,�������޷�ֱ����ʾ��,�����Ӣ��,�����ֱ����ʾ.
 *
 *              ����Ҫ֧��GBK����Ķ�ά��ʱ:��ATK_QR_GBK_SUPPORT���ֵ��Ϊ1(Ĭ����1)����.
 *
 *              ��LIB��ֲ����:
 *              1,ʵ��atk_qrdecode.c��������к���.
 *              2,��ջ(Stack_Size)����Ϊ0X1000������.
 *
 *              ��LIBʹ�ò���:
 *              1,����atk_qr_init����,��ʼ��ʶ�����,����ֵΪATK_QR_OK,���ʼ���ɹ�.
 *              2,����atk_qr_decode����,��������,��ͼ�����ʶ��.
 *              3,�����Ҫ��ͣ��ʶ��,���ظ���2�����輴��.
 *              4,����atk_qr_destroy����,����ʶ��,�ͷ������ڴ�.����ʶ��.
 *
 *              uint8_t atk_qr_decode(uint16_t bmp_width,uint16_t bmp_heigh,uint8_t *bmp,uint8_t btype,uint8_t* result)��������˵��:
 *              img_width,img_heigh:����ͼ��Ŀ�Ⱥ͸߶�
 *              imgbuf:ͼ�񻺴���(8λ�Ҷ�ͼ��,����RGB565!!!!)
 *              btype:0,ʶ���ά��
 *                    1,ʶ��CODE128����
 *                    2,ʶ��CODE39����
 *                    3,ʶ��I25����
 *                    4,ʶ��EAN13����
 *              result:ʶ����������.���result[0]==0,��˵��δʶ���κ�����,�������ʶ�𵽵�����(�ַ���)
 *              ����ֵ:ATK_QR_OK,ʶ�����
 *                          ����,�������
 *              �����Ҫ������֧�ֵı������ʶ��,����������btypeΪ0~4����ʵ��.
 *
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
 * V1.1 20220602
 * 1, �޸�ע�ͷ�ʽ
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 ****************************************************************************************************
 */

#include "atk_qrdecode.h"
#include "./MALLOC/malloc.h"


/**
 * @brief       �ڴ����ú���
 * @param       p               : �ڴ��׵�ַ
 * @param       c               : Ҫ���õ�ֵ
 * @param       len             : ��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
 * @retval      ��
 */
void atk_qr_memset(void *p, uint8_t c, uint32_t len)
{
    my_mem_set(p, c, len);
}

/**
 * @brief       �ڴ����뺯��
 * @param       size            : ������ڴ��С
 * @retval      0,ʧ��; ����, �ڴ��׵�ַ;
 */
void *atk_qr_malloc(uint32_t size)
{
    return mymalloc(SRAM12, size);
}

/**
 * @brief       �ڴ������뺯��
 * @param       ptr             : ���ڴ��׵�ַ
 * @param       size            : ��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
 * @retval      ��
 */
void *atk_qr_realloc(void *ptr, uint32_t size)
{
    return myrealloc(SRAM12, ptr, size);
}

/**
 * @brief       �ڴ��ͷź���
 * @param       ptr             : �ڴ��׵�ַ
 * @retval      ��
 */
void atk_qr_free(void *ptr)
{
    myfree(SRAM12, ptr);
}

/**
 * @brief       �ڴ渴�ƺ���
 * @param       des             : Ŀ�ĵ�ַ
 * @param       src             : Դ��ַ
 * @param       n               : ��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
 * @retval      ��
 */
void atk_qr_memcpy(void *des, void *src, uint32_t n)
{
    my_mem_copy((uint8_t *)des, (uint8_t *)src, n);
}


/* ���ʹ����UTF2OEM֧��,����Ҫʵ�ֽ�UTF8ת����UNICODEת���OEM */
#if ATK_QR_UTF82OEM_SUPPORT

#include "./FATFS/source/ff.h"


/**
 * @brief       ��UNICODE����ת����OEM����
 * @param       unicode         : UNICODE������ַ�����
 * @retval      OEM���뷽ʽ���ַ�����
 */
uint16_t atk_qr_convert(uint16_t unicode)
{
    return ff_uni2oem(unicode, 0);
}

#endif















