/**
 ****************************************************************************************************
 * @file        mp3player.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2022-10-28
 * @brief       MP3�������� ����
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
 * 2, �޸�u8/u16/u32Ϊuint8_t/uint16_t/uint32_t
 *
 ****************************************************************************************************
 */

#ifndef __MP3PLAYER_H
#define __MP3PLAYER_H

#include "./SYSTEM/sys/sys.h"


uint32_t mp3_file_seek(uint32_t pos);
uint8_t mp3_play_song(uint8_t *pname); 

#endif












