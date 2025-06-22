/**
 ****************************************************************************************************
 * @file        netplay.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-26
 * @brief       APP-������� ����
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
 ****************************************************************************************************
 */

#include "netplay.h"
#include "gui.h"
#include "common.h"
#include "./T9INPUT/t9input.h"
//#include "audioplay.h"
#include "./BSP/TPAD/tpad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* CH395Q�������� */
#define CH395Q_HANDLER_TASK_PRIO    5       /* �����������ȼ� */
#define CH395Q_HANDLER_STK_SIZE     64      /* ���������ջ��С */
OS_STK *CH395Q_HANDLER_TASK_STK;            /* �����ջ�������ڴ����ķ�ʽ�������� */

void ch395q_handler_task(void *p_arg);      /* ������ */

/* ��̬������Ϣ����(DHCPʧ��ʱʹ��) */
static uint8_t static_ip[4] = {192, 168, 1, 100};
static uint8_t static_gwip[4] = {192, 168, 1, 1};
static uint8_t static_mask[4] = {255, 255, 252, 0};

static struct
{
    uint8_t phy_status;                                             /* PHY����״̬ */
    uint8_t dhcp_status;                                            /* DHCP״̬ */
    uint8_t tcp_conn_status[NET_TCP_SERVER_MAX_CONN + 1];           /* TCP(Server/Client)����״̬ */
    
    uint8_t socket_send_busy_status[NET_TCP_SERVER_MAX_CONN + 1];   /* Socket����æ״̬ */
    
    uint8_t *socket_recv_buf[NET_TCP_SERVER_MAX_CONN + 1];          /* Socket���ջ����� */
    uint8_t socket_recv_status[NET_TCP_SERVER_MAX_CONN + 1];        /* Socket������Ч״̬ */
    uint16_t socket_recv_len[NET_TCP_SERVER_MAX_CONN + 1];          /* Socket������Ч���ݴ�С */
    
    uint8_t mac[6];                                                 /* MAC��ַ */
    uint8_t ip[4];                                                  /* IP��ַ */
    uint8_t gwip[4];                                                /* ����IP��ַ */
    uint8_t mask[4];                                                /* �������� */
    uint8_t dns1[4];                                                /* DNS������1��ַ */
    uint8_t dns2[4];                                                /* DNS������2��ַ */
    
    uint8_t des_ip[NET_TCP_SERVER_MAX_CONN + 1][4];                 /* Ŀ��IP��ַ */
    uint16_t des_port[NET_TCP_SERVER_MAX_CONN + 1];                 /* Ŀ��˿ں� */
} netdev = {0};

/* netplay��ʾ��Ϣ */
uint8_t *const netplay_remindmsg_tbl[5][GUI_LANGUAGE_NUM] =
{
    {"���������!���ڳ�ʼ������...", "Ո����W��!���ڳ�ʼ���W��...", "Pls insert cable!Ethernet Initing..",},
    {"δ�ҵ�MO395Qģ��!����...", "δ�ҵ�MO395Qģ�K,Ո�z��...", "No MO395Q Module!Pls Check...",},
    {"����DHCP��ȡIP...", "����DHCP�@ȡIP...", "DHCP IP configing...",},
    {"DHCP��ȡIP�ɹ�!", "DHCP�@ȡIP�ɹ�!", "DHCP IP config OK!",},
    {"DHCP��ȡIPʧ��,ʹ��Ĭ��IP!", "DHCP�@ȡIPʧ��,ʹ��Ĭ�JIP!", "DHCP IP config fail!Use default IP",},
};
/* netplay IP��Ϣ */
uint8_t *const netplay_ipmsg[5][GUI_LANGUAGE_NUM] =
{
    {"����MAC��ַ:", "���CMAC��ַ:", "Local MAC Addr:",},
    {" Զ��IP��ַ:", " �h��IP��ַ:", "Remote IP Addr:",},
    {" ����IP��ַ:", " ����IP��ַ:", " Local IP Addr:",},
    {"   ��������:", "   �ӾW�ڴa:", "   Subnet MASK:",},
    {"       ����:", "       �W�P:", "       Gateway:",},
};
/* ������ʾ */
uint8_t *const netplay_netspdmsg[GUI_LANGUAGE_NUM] = {"   �����ٶ�:", "   �W�j�ٶ�:", "Ethernet Speed:"};
/* netplay ������ʾ��Ϣ */
uint8_t *const netplay_testmsg_tbl[GUI_LANGUAGE_NUM] = {"�ɼ������״̬.", "�əz���B�Ӡ�B.", "to check the connection."};
/* netplay memo��ʾ��Ϣ */
uint8_t *const netplay_memoremind_tb[2][GUI_LANGUAGE_NUM] =
{
    {"������:", "���Յ^:", "Receive:",},
    {"������:", "�l�ͅ^:", "Send:",},
};
/* netplay ���԰�ť���� */
uint8_t *const netplay_tbtncaption_tb[GUI_LANGUAGE_NUM] = {"��ʼ����", "�_ʼ�yԇ", "Start Test",};
/* netplay Э����� */
uint8_t *const netplay_protcaption_tb[GUI_LANGUAGE_NUM] = {"Э��", "�f�h", "PROT",};
/* netplay Э������ */
uint8_t *const netplay_protname_tb[3] = {"TCP Server", "TCP Client", "UDP",};
/* netplay �˿ڱ��� */
uint8_t *const netplay_portcaption_tb[GUI_LANGUAGE_NUM] = {"�˿�:", "�˿�:", "Port:",};
/* netplay IP��ַ���� */
uint8_t *const netplay_ipcaption_tb[2][GUI_LANGUAGE_NUM] =
{
    {"Ŀ��IP:", "Ŀ��IP:", "Target IP:",},
    {"����IP:", "���CIP:", " Local IP:",},
};
/* netplay ��ť���� */
uint8_t *const netplay_btncaption_tbl[5][GUI_LANGUAGE_NUM] =
{
    {"Э��ѡ��", "�f�h�x��", "PROT SEL",},
    {"����", "�B��", "Conn",},
    {"�Ͽ�", "���_", "Dis Conn",},
    {"�������", "�������", "Clear",},
    {"����", "�l��", "Send",},
};
/* ����ģʽѡ�� */
uint8_t *const netplay_mode_tbl[3] = {"TCP Server", "TCP Client", "UDP"};
/* ����������ʾ��Ϣ */
uint8_t *const netplay_connmsg_tbl[4][GUI_LANGUAGE_NUM] =
{
    {"��������...", "�����B��...", "Connecting...",},
    {"����ʧ��!", "�B��ʧ��!", "Connect fail!",},
    {"���ӳɹ�!", "�B�ӳɹ�!", "Connect OK!",},
    {"���ӶϿ�!", "�B�Ӕ��_!", "Disconnect!",},
};

/**
 * @brief       PHY���ӻص�
 * @param       phy_status: PHY״̬
 * @retval      ��
 */
static void phy_conn_cb(uint8_t phy_status)
{
    netdev.phy_status = phy_status;
}

/**
 * @brief       PHY�Ͽ��ص�
 * @param       ��
 * @retval      ��
 */
static void phy_disconn_cb(void)
{
    netdev.phy_status = CH395Q_CMD_PHY_DISCONN;
}

/**
 * @brief       DHCP�ɹ��ص�
 * @param       ip  : IP��ַ
 *              gwip: ����IP��ַ
 *              mask: ��������
 *              dns1: DNS������1��ַ
 *              dns2: DNS������2��ַ
 * @retval      ��
 */
static void dhcp_success_cb(uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *dns1, uint8_t *dns2)
{
    uint8_t i;
    
    netdev.dhcp_status = 1;
    for (i=0; i<sizeof(netdev.ip); i++)
    {
        netdev.ip[i] = ip[i];
    }
    for (i=0; i<sizeof(netdev.gwip); i++)
    {
        netdev.gwip[i] = gwip[i];
    }
    for (i=0; i<sizeof(netdev.mask); i++)
    {
        netdev.mask[i] = mask[i];
    }
    for (i=0; i<sizeof(netdev.dns1); i++)
    {
        netdev.dns1[i] = dns1[i];
    }
    for (i=0; i<sizeof(netdev.dns2); i++)
    {
        netdev.dns2[i] = dns2[i];
    }
}

/**
 * @brief       Socket���ͻ��������лص�
 * @param       socket: Socket���
 * @retval      ��
 */
static void socket_send_buf_free_cb(ch395q_socket_t *socket)
{
    netdev.socket_send_busy_status[socket->socket_index] = 0;   /* ��Ƿ��ͻ��������У����Լ����������� */
}

/**
 * @brief       Socket���ջ������ǿջص�
 * @param       socket: Socket���
 * @retval      ��
 */
static void socket_recv_cb(ch395q_socket_t *socket)
{
    uint16_t recv_len;
    
    recv_len = ch395q_cmd_get_recv_len_sn(socket->socket_index);                        /* ��ȡ���������ݳ��� */
    if ((recv_len != 0) && (netdev.socket_recv_status[socket->socket_index] == 0))      /* ���������ݳ��Ȳ�Ϊ0���ѽ��������ѱ����� */
    {
        if (recv_len > (socket->recv.size - 1))                                         /* ���������ݳ��ȳ������ջ�������С(Ϊ������Ԥ��1�ֽڿռ�) */
        {
            recv_len = socket->recv.size;
        }
        ch395q_cmd_read_recv_buf_sn(socket->socket_index, recv_len, socket->recv.buf);  /* �������� */
        netdev.socket_recv_status[socket->socket_index] = 1;                            /* ��ǽ���������Ч�������� */
        netdev.socket_recv_len[socket->socket_index] = recv_len;                        /* �����ѽ������ݳ��� */
    }
}

/**
 * @brief       Socket TCP���ӻص�
 * @param       socket: Socket���
 * @retval      ��
 */
static void socket_tcp_conn_cb(ch395q_socket_t *socket)
{
    netdev.tcp_conn_status[socket->socket_index] = 1;           /* ���TCP������ */
    netdev.des_ip[socket->socket_index][0] = socket->des_ip[0]; /* ����Ŀ��IP��ַ�Ͷ˿ں� */
    netdev.des_ip[socket->socket_index][1] = socket->des_ip[1];
    netdev.des_ip[socket->socket_index][2] = socket->des_ip[2];
    netdev.des_ip[socket->socket_index][3] = socket->des_ip[3];
    netdev.des_port[socket->socket_index] = socket->des_port;
}

/**
 * @brief       Socket TCP�Ͽ��ص�
 * @param       socket: Socket���
 * @retval      ��
 */
static void socket_tcp_disconn_cb(ch395q_socket_t *socket)
{
    netdev.tcp_conn_status[socket->socket_index] = 0;           /* ���TCPδ���� */
}

/**
 * @brief       CH395Q��������
 * @param       p_arg: δʹ��
 * @retval      ��
 */
static void ch395q_handler_task(void *p_arg)
{
    while (1)
    {
        ch395q_handler();
        delay_ms(10);
    }
}

/**
 * @brief       ����CH395Q��������
 * @param       ��
 * @retval      0   : �ɹ�
 *              ����: �������
 */
static uint8_t ch395q_handler_task_creat(void)
{
    OS_CPU_SR cpu_sr;
    uint8_t res;
    
    CH395Q_HANDLER_TASK_STK = (OS_STK *)gui_memin_malloc(CH395Q_HANDLER_STK_SIZE * sizeof(OS_STK));         /* ΪCH395Q������������ջ�ռ� */
    if (CH395Q_HANDLER_TASK_STK == NULL)                                                                    /* �ڴ�����ʧ�� */
    {
        return 1;
    }
    
    OS_ENTER_CRITICAL();                                                                                    /* �����ٽ��� */
    res = OSTaskCreate(ch395q_handler_task, (void *)0, (OS_STK *)&CH395Q_HANDLER_TASK_STK[CH395Q_HANDLER_STK_SIZE - 1], CH395Q_HANDLER_TASK_PRIO);
    OS_EXIT_CRITICAL();                                                                                     /* �˳��ٽ��� */
    
    return res;
}

/**
 * @brief       ɾ��CH395Q��������
 * @param       ��
 * @retval      ��
 */
static void ch395q_handler_task_delete(void)
{
    OSTaskDel(CH395Q_HANDLER_TASK_PRIO);        /* ɾ��CH395Q�������� */
    gui_memin_free(CH395Q_HANDLER_TASK_STK);    /* �ͷ��ڴ� */
}

/**
 * @brief       ������·����������
 * @param       ��
 * @retval      ��
 */
static void net_load_ui(void)
{
    uint8_t *buf;
    uint8_t fsize = 0;
    uint16_t length;
    
    buf = gui_memin_malloc(100);
    app_filebrower((uint8_t *)APP_MFUNS_CAPTION_TBL[9][gui_phy.language], 0x05);                                                                   /* ��ʾ���� */
    
    if (lcddev.width == 240)                                                                                                                        /* ���������С */
    {
        fsize = 12;
    }
    else
    {
        fsize = 16;
    }
    
    length = strlen((char *)netplay_ipmsg[0][gui_phy.language]);                                                                                    /* ��ȡ�ַ������ȣ����������ַ����Ķ��� */
    gui_fill_rectangle(0, gui_phy.tbheight, lcddev.width, lcddev.height - gui_phy.tbheight, LGRAY);                                                 /* ����������ɫ */
    gui_show_string(netplay_ipmsg[0][gui_phy.language], 10, gui_phy.tbheight + 5, lcddev.width, fsize, fsize, BLUE);                                /* ����MAC��ַ */
    gui_show_string(netplay_ipmsg[2][gui_phy.language], 10, (4 + fsize) * 1 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, BLUE);              /* ����IP��ַ */
    gui_show_string(netplay_ipmsg[3][gui_phy.language], 10, (4 + fsize) * 2 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, BLUE);              /* �������� */
    gui_show_string(netplay_ipmsg[4][gui_phy.language], 10, (4 + fsize) * 3 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, BLUE);              /* ���� */
    gui_show_string(netplay_netspdmsg[gui_phy.language], 10, (4 + fsize) * 4 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, BLUE);             /* �����ٶ� */
    sprintf((char *)buf, "%02X-%02X-%02X-%02X-%02X-%02X", netdev.mac[0], netdev.mac[1], netdev.mac[2], netdev.mac[3], netdev.mac[4], netdev.mac[5]);
    gui_show_string(buf, 10 + length * fsize / 2, gui_phy.tbheight + 5, lcddev.width, fsize, fsize, RED);                                           /* ��ʾ����MAC��ַ */
    sprintf((char *)buf, "%d.%d.%d.%d", netdev.ip[0], netdev.ip[1], netdev.ip[2], netdev.ip[3]);
    gui_show_string(buf, 10 + length * fsize / 2, (4 + fsize) * 1 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, RED);                         /* ��ʾ����IP��ַ */
    sprintf((char *)buf, "%d.%d.%d.%d", netdev.mask[0], netdev.mask[1], netdev.mask[2], netdev.mask[3]);
    gui_show_string(buf, 10 + length * fsize / 2, (4 + fsize) * 2 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, RED);                         /* ��ʾ�������� */
    sprintf((char *)buf, "%d.%d.%d.%d", netdev.gwip[0], netdev.gwip[1], netdev.gwip[2], netdev.gwip[3]);
    gui_show_string(buf, 10 + length * fsize / 2, (4 + fsize) * 3 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, RED);                         /* ��ʾ����IP��ַ */
    switch (netdev.phy_status)
    {
        case CH395Q_CMD_PHY_10M_FLL:
        {
            sprintf((char *)buf, "10Mbps Full");
            break;
        }
        case CH395Q_CMD_PHY_10M_HALF:
        {
            sprintf((char *)buf, "10Mbps Half");
            break;
        }
        case CH395Q_CMD_PHY_100M_FLL:
        {
            sprintf((char *)buf, "100Mbps Full");
            break;
        }
        case CH395Q_CMD_PHY_100M_HALF:
        {
            sprintf((char *)buf, "100Mbps Half");
            break;
        }
        case CH395Q_CMD_PHY_AUTO:
        {
            sprintf((char *)buf, "Auto");
            break;
        }
    }
    gui_show_string(buf, 10 + length * fsize / 2, (4 + fsize) * 4 + gui_phy.tbheight + 5, lcddev.width, fsize, fsize, RED);                         /* ��ʾ�����ٶ� */
    sprintf((char *)buf, "Ping %d.%d.%d.%d %s", netdev.ip[0], netdev.ip[1], netdev.ip[2], netdev.ip[3], netplay_testmsg_tbl[gui_phy.language]);
    gui_show_string(buf, 10, (4 + fsize) * 6 + gui_phy.tbheight + 5, lcddev.width - 20, 2 * fsize, fsize, BLUE);                                    /* ��ʾPing������ʾ */
    
    gui_memin_free(buf);
}

/**
 * @brief       ��ʾ��ʾ��Ϣ
 * @param       y     : Y���꣨X����㶨��0��ʼ��
 * @param       height: ����߶�
 * @param       fsize : �����С
 * @param       tx    : �����ֽ���
 * @param       rx    : �����ֽ���
 * @param       prot  : Э������
 *   @arg       0: TCP Server
 *   @arg       1: TCP Client
 *   @arg       2: UDP
 * @param       flag  : ���±��
 *   @arg       bit0: 0: ������,1: ����tx����
 *   @arg       bit1: 0: ������,1: ����rx����
 *   @arg       bit2: 0: ������,1: ����prot����
 * @retval      ��
 */
static void net_msg_show(uint16_t y, uint16_t height, uint8_t fsize, uint32_t tx, uint32_t rx, uint8_t prot, uint8_t flag)
{
    uint16_t xdis;
    uint8_t *pbuf;
    uint8_t i;
    uint32_t max;
    
    pbuf = gui_memin_malloc(100);
    if (pbuf == NULL)
    {
        return;
    }
    
    if (prot > 2)                                                                                                                               /* prot�ķ�ΧΪ0~2 */
    {
        prot = 2;
    }
    
    xdis = (lcddev.width - (35 * fsize / 2)) / 3;
    
    if (flag & (1 << 0))                                                                                                                        /* ����tx���� */
    {
        gui_fill_rectangle(xdis / 2, y + (height - fsize) / 2, 10 * fsize / 2 + xdis, fsize, NET_MSG_BACK_COLOR);
        for (max=1, i=0; i<(10 + (2 * xdis / fsize)) - 3; i++)
        {
            max *= 10;
        }
        sprintf((char *)pbuf, "TX:%d%c", (tx <= (max - 1) ? tx : (max - 1)), (tx <= (max - 1) ? ' ' : '+'));
        gui_show_string(pbuf, xdis / 2, y + (height - fsize) / 2, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);
    }
    
    if (flag & (1 << 1))                                                                                                                        /* ����rx���� */
    {
        gui_fill_rectangle(xdis / 2 + 10 * fsize / 2 + xdis, y + (height - fsize) / 2, 10 * fsize / 2 + xdis, fsize, NET_MSG_BACK_COLOR);
        for (max=1, i=0; i<(10 + (2 * xdis / fsize)) - 3; i++)
        {
            max *= 10;
        }
        sprintf((char *)pbuf, "RX:%d%c", (rx <= (max - 1) ? rx : (max - 1)), (rx <= (max - 1) ? ' ' : '+'));
        gui_show_string(pbuf, xdis / 2 + 10 * fsize / 2 + xdis, y + (height - fsize) / 2, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);
    }
    
    if (flag & (1 << 2))                                                                                                                        /* ����prot���� */
    {
        gui_fill_rectangle(xdis / 2 + 20 * fsize / 2 + xdis * 2, y + (height - fsize) / 2, 15 * fsize / 2, fsize, NET_MSG_BACK_COLOR);
        sprintf((char *)pbuf, "%s:%s", netplay_protcaption_tb[gui_phy.language], netplay_protname_tb[prot]);
        gui_show_string(pbuf, xdis / 2 + 20 * fsize / 2 + xdis * 2, y + (height - fsize) / 2, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);
    }
    
    gui_memin_free(pbuf);
}

/**
 * @brief       ���ñ༭����ɫ
 * @param       ipx    : IP�༭��
 * @param       portx  : Port�༭��
 * @param       prot   : Э��
 *   @arg       0: TCP Server
 *   @arg       1: TCP Client
 *   @arg       2: UDP
 * @param       connsta: ����״̬
 *   @arg       0: δ����
 *   @arg       1: ������
 * @retval      ��
 */
static void net_edit_colorset(_edit_obj *ipx, _edit_obj *portx, uint8_t prot, uint8_t connsta)
{
    if (connsta == 1)                       /* �����ӣ����ɱ༭ */
    {
        ipx->textcolor = WHITE;
        portx->textcolor = WHITE;
    }
    else                                    /* δ���ӣ��ɱ༭ */
    {
        switch (prot)
        {
            case 0:                         /* TCP Server��Э��ɱ༭��IP��ַ���ɱ༭ */
            {
                portx->textcolor = GREEN;
                ipx->textcolor = WHITE;
                break;
            }
            case 1:                         /* TCP Client��Э���IP��ַ�ɱ༭ */
            case 2:                         /* UDP��Э���IP��ַ�ɱ༭ */
            {
                portx->textcolor = GREEN;
                ipx->textcolor = GREEN;
                break;
            }
        }
    }
    
    edit_draw(ipx);                         /* �ػ�༭�� */
    edit_draw(portx);
}

/**
 * @brief       ���ַ�����ʽ�Ķ˿ں�ת��Ϊ������ʽ�Ķ˿ں�
 * @param       str: �ַ�����ʽ�Ķ˿ں�
 * @retval      ������ʽ�Ķ˿ں�
 */
static uint16_t net_get_port(uint8_t *str)
{
    uint16_t port;
    
    port = atoi((char*)str);    /* �ַ���ת���� */
    
    return port;
}

/**
 * @brief       ���ַ�����ʽ��IP��ַת��Ϊ������ʽ��IP��ַ
 * @param       str: �ַ�����ʽ��IP��ַ
 * @param       ip : ������ʽ��IP��ַ
 * @retval      ת�����
 *    @arg      0: ת���ɹ�
 *    @arg      1: �ַ�����ʽ��IP��ַ����ת��ʧ��
 */
static uint8_t net_get_ip(uint8_t *str, uint8_t *ip)
{
    uint8_t *p1;
    uint8_t *p2;
    uint8_t *ipstr;
    
    ipstr = gui_memin_malloc(30);
    if(ipstr == NULL)
    {
        return 1;
    }
    strcpy((char *)ipstr, (char *)str);
    
    p1 = ipstr;                                     /* ��ȡ��һ��ֵ */
    p2 = (uint8_t *)strstr((const char *)p1, ".");
    if(p2 == NULL)
    {
        gui_memin_free(ipstr);
        return 1;
    }
    p2[0] = 0;
    ip[0] = atoi((char *)p1);
    
    p1 = p2 + 1;                                    /* ��ȡ�ڶ���ֵ */
    p2 = (uint8_t *)strstr((const char *)p1,".");
    if(p2 == NULL)
    {
        gui_memin_free(ipstr);
        return 1;
    }
    p2[0] = 0;
    ip[1] = atoi((char *)p1);
    
    p1 = p2 + 1;                                    /* ��ȡ������ֵ */
    p2 = (uint8_t *)strstr((const char *)p1,".");
    if(p2 == NULL)
    {
        gui_memin_free(ipstr);
        return 1;
    }
    p2[0] = 0;
    ip[2] = atoi((char *)p1);
    
    p1 = p2 + 1;                                    /* ��ȡ���ĸ�ֵ */
    ip[3] = atoi((char *)p1);
    
    gui_memin_free(ipstr);
    
    return 0;
}

/**
 * @brief       �������������
 * @param       ��
 * @retval      ��
 */
static uint8_t net_test(void)
{
    uint8_t ip_height;              /* ���沼����ر��� */
    uint8_t ip_fsize;
    uint8_t msg_height;
    uint16_t memo_width;
    uint16_t btn_width;
    uint16_t rmemo_height;
    uint16_t smemo_height;
    uint16_t rbtn_height;
    uint8_t m_offx;
    uint8_t sm_offy;
    uint8_t rm_offy;
    uint8_t fsize;
    uint8_t sbtnfsize;
    uint16_t t9height;
    uint16_t tempx;
    uint16_t tempy;
    uint8_t *ptemp;
    uint8_t *ipcaption;
    _edit_obj *eip;                 /* IP��ַ�༭�� */
    _edit_obj *eport;               /* �˿ںű༭�� */
    _btn_obj *protbtn;              /* Э��ѡ��ť */
    _btn_obj *connbtn;              /* ���Ӱ�ť */
    _btn_obj *clrbtn;               /* ���������ť */
    _btn_obj *sendbtn;              /* ���Ͱ�ť */
    _memo_obj *rmemo;               /* �������ı��� */
    _memo_obj *smemo;               /* �������ı��� */
    _t9_obj *t9;                    /* ���뷨 */
    uint32_t rxcnt = 0;             /* ���ռ��� */
    uint32_t txcnt = 0;             /* ���ͼ��� */
    uint8_t protocol = 0;           /* Э���־
                                    * 0: TCP Server
                                    * 1: TCP Client
                                    * 2: UDP
                                    */
    uint8_t res;                    /* ��ť����� */
    uint8_t rval = 0;               /* �ؼ��ȳ�ʼ����� */
    uint16_t tport;                 /* ���ڻ�ȡ�˿ں� */
    uint8_t connstatus = 0;         /* ����״̬
                                    * 0: δ����
                                    * 1: ������
                                    */
    uint8_t oldconnstatus = 0;      /* ��¼��һ������״̬ */
    uint8_t oldaddr[4] = {0};       /* ��¼��һ�����ӵ�Ŀ��IP��ַ */
    uint16_t oldport = 0;           /* ��¼��һ�����ӵ�Ŀ��˿ں� */
    uint8_t editflag = 0;           /* �༭��־
                                    * 0: �༭�������ı���
                                    * 1: �༭IP��ַ�༭��
                                    * 2: �༭�˿ںű༭��
                                    */
    uint16_t *bkcolor;              /* ���汻��ʾ���ڵ�����ʾ���� */
    uint8_t retry;                  /* �������Լ��� */
    ch395q_socket_t socket = {0};   /* CH395Q Socket�ṹ�� */
    uint8_t conn_err;               /* Socket����״̬
                                     * 0: ���ӳɹ�
                                     * 1: ����ʧ��
                                     */
    uint8_t socket_index;           /* ���ڱ�������Socket */
    uint8_t socket_send;            /* �������Socket�������� */
    
    lcd_clear(NET_MEMO_BACK_COLOR); /* ���� */
    if (lcddev.width == 240)        /* ����LCD�ߴ����ò��� */
    {
        ip_height = 20;
        ip_fsize = 12;
        msg_height = 16;
        memo_width = 172;
        btn_width = 56;
        rmemo_height = 72;
        smemo_height = 36;
        rbtn_height = 20;
        m_offx = 4;
        sm_offy = 4;
        rm_offy = 5;
        fsize = 12;
        sbtnfsize = 16;
        t9height = 134;
    }
    else if (lcddev.width == 272)
    {
        ip_height = 20;
        ip_fsize = 12;
        msg_height = 20;
        memo_width = 180;
        btn_width = 68;
        rmemo_height = 160;
        smemo_height = 48;
        rbtn_height = 34;
        m_offx = 4;
        sm_offy = 8;
        rm_offy = 8;
        fsize = 12;
        sbtnfsize = 16;
        t9height = 176;
    }
    else if (lcddev.width == 320)
    {
        ip_height = 24;
        ip_fsize = 16;
        msg_height = 20;
        memo_width = 208;
        btn_width = 80;
        rmemo_height = 144;
        smemo_height = 48;
        rbtn_height = 40;
        m_offx = 12;
        sm_offy = 8;
        rm_offy = 10;
        fsize = 16;
        sbtnfsize = 24;
        t9height = 176;
    }
    else if (lcddev.width == 480)
    {
        ip_height = 36;
        ip_fsize = 24;
        msg_height = 28;
        memo_width = 304;
        btn_width = 100;
        rmemo_height = 336;
        smemo_height = 64;
        rbtn_height = 50;
        m_offx = 16;
        sm_offy = 10;
        rm_offy = 9;
        fsize = 16;
        sbtnfsize = 24;
        t9height = 266;
    }
    else if (lcddev.width == 600)
    {
        ip_height = 36;
        ip_fsize = 24;
        msg_height = 28;
        memo_width = 364;
        btn_width = 120;
        rmemo_height = 436;
        smemo_height = 80;
        rbtn_height = 50;
        m_offx = 16;
        sm_offy = 12;
        rm_offy = 10;
        fsize = 16;
        sbtnfsize = 24;
        t9height = 368;
    }
    else if (lcddev.width == 800)
    {
        ip_height = 60;
        ip_fsize = 32;
        msg_height = 30;
        memo_width = 500;
        btn_width = 200;
        rmemo_height = 480;
        smemo_height = 150;
        rbtn_height = 50;
        m_offx = 16;
        sm_offy = 10;
        rm_offy = 10;
        fsize = 16;
        sbtnfsize = 24;
        t9height = 488;
    }
    
    gui_fill_rectangle(0, 0, lcddev.width, ip_height, NET_IP_BACK_COLOR);                                                                                       /* ���IP��ַ���˿ں����򱳾� */
    gui_fill_rectangle(0, ip_height, lcddev.width, msg_height, NET_MSG_BACK_COLOR);                                                                             /* ���ͳ����Ϣ��Э�����򱳾� */
    gui_draw_hline(0, ip_height + msg_height - 1, lcddev.width, NET_COM_RIM_COLOR);                                                                             /* �������Ϸ��ķָ��� */
    tempy = ip_height + msg_height + rmemo_height + fsize + 2 * rm_offy;
    gui_draw_hline(0, tempy, lcddev.width, NET_COM_RIM_COLOR);                                                                                                  /* �������Ϸ��ķָ��� */
    tempx = (lcddev.width - 35 * ip_fsize / 2) / 3;
    ipcaption = netplay_ipcaption_tb[1][gui_phy.language];
    gui_show_string(ipcaption, tempx, (ip_height - ip_fsize) / 2, lcddev.width, ip_fsize, ip_fsize, WHITE);                                                     /* ��ʾIP��ַ�����ı� */
    tempx = lcddev.width - tempx - 10 * ip_fsize / 2;
    gui_show_string(netplay_portcaption_tb[gui_phy.language], tempx, (ip_height - ip_fsize) / 2, lcddev.width, ip_fsize, ip_fsize, WHITE);                      /* ��ʾ�˿ںű����ı� */
    tempy = ip_height + msg_height + rm_offy + fsize;
    gui_show_string(netplay_memoremind_tb[0][gui_phy.language], m_offx, tempy - fsize - rm_offy / 3, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);           /* ��ʾ�����������ı� */
    rmemo = memo_creat(m_offx, tempy, memo_width, rmemo_height, 0, 0, fsize, NET_RMEMO_MAXLEN);                                                                 /* �����������ı��� */
    tempx = lcddev.width - tempx - 10 * ip_fsize / 2;
    eip = edit_creat(strlen((char *)ipcaption) * ip_fsize / 2 + tempx, (ip_height - ip_fsize - 6) / 2, 15 * ip_fsize / 2 + 6, ip_fsize + 6, 0, 4, ip_fsize);    /* ����IP��ַ�༭�� */
    tempx = (lcddev.width - 35 * ip_fsize / 2) / 3;
    tempx = lcddev.width - tempx - 10 * ip_fsize / 2;
    eport = edit_creat(tempx + 5 * ip_fsize / 2, (ip_height - ip_fsize - 6) / 2, 5 * ip_fsize / 2 + 6, ip_fsize + 6, 0, 4, ip_fsize);                           /* �����˿ںű༭�� */
    tempy = ip_height + msg_height + rm_offy * 2 + rmemo_height + fsize * 2 + sm_offy;
    gui_show_string(netplay_memoremind_tb[1][gui_phy.language], m_offx, tempy - fsize - sm_offy / 3, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);           /* ��ʾ�����������ı� */
    smemo = memo_creat(m_offx, tempy, memo_width, smemo_height, 0, 1, fsize, NET_SMEMO_MAXLEN);                                                                 /* �����������ı��� */
    tempy = ip_height + msg_height + rm_offy * 2 + rmemo_height + fsize * 2 + sm_offy * 2 + smemo_height;
    t9 = t9_creat((lcddev.width % 5) / 2, tempy, lcddev.width - (lcddev.width % 5), t9height, 0);                                                               /* �������뷨 */
    tempy = ip_height + msg_height + rm_offy + fsize;
    tempx = (lcddev.width - (m_offx + memo_width + btn_width)) / 2 + m_offx + memo_width;
    protbtn = btn_creat(tempx, tempy, btn_width, rbtn_height, 0, 0);                                                                                            /* ����Э��ѡ��ť */
    memo_width = (rmemo_height - 3 * rbtn_height) / 2;
    if (memo_width > rbtn_height / 2)
    {
        memo_width = rbtn_height / 2;
    }
    connbtn = btn_creat(tempx, tempy + memo_width + rbtn_height, btn_width, rbtn_height, 0, 0);                                                                 /* �������Ӱ�ť */
    clrbtn = btn_creat(tempx, tempy + memo_width * 2 + rbtn_height * 2, btn_width, rbtn_height, 0, 0);                                                          /* ����������հ�ť */
    tempy = ip_height + msg_height + rm_offy * 2 + rmemo_height + fsize * 2 + sm_offy;
    sendbtn = btn_creat(tempx, tempy, btn_width, smemo_height, 0, 2);                                                                                           /* �������Ͱ�ť */
    ptemp = gui_memin_malloc(100);
    
    if (!rmemo || !eip || !eport || !smemo || !t9 || !protbtn || !connbtn || !clrbtn || !sendbtn || !ptemp)                                                     /* ���пؼ�����ʧ�ܣ��򷵻� */
    {
        rval = 1;
    }
    
    if (rval == 0)                                                                                      /* �ؼ������ɹ� */
    {
        protbtn->caption = netplay_btncaption_tbl[0][gui_phy.language];                                 /* ����Э��ѡ��ť����ʽ */
        protbtn->font = fsize;
        connbtn->caption = netplay_btncaption_tbl[1][gui_phy.language];                                 /* �������Ӱ�ť����ʽ */
        connbtn->font = fsize;
        clrbtn->caption = netplay_btncaption_tbl[3][gui_phy.language];                                  /* ����������հ�ť����ʽ */
        clrbtn->font = fsize;
        sendbtn->bkctbl[0] = 0x6BF6;                                                                    /* ���÷��Ͱ�ť����ʽ */
        sendbtn->bkctbl[1] = 0x545E;
        sendbtn->bkctbl[2] = 0x5C7E;
        sendbtn->bkctbl[3] = 0x2ADC;
        sendbtn->bcfucolor = WHITE;
        sendbtn->bcfdcolor = BLACK;
        sendbtn->caption = netplay_btncaption_tbl[4][gui_phy.language];
        sendbtn->font = sbtnfsize;
        eip->textbkcolor = NET_IP_BACK_COLOR;                                                           /* ����IP��ַ�༭�����ʽ */
        eip->textcolor = WHITE;
        eport->textbkcolor = NET_IP_BACK_COLOR;                                                         /* ���ö˿ںű༭�����ʽ */
        eport->textcolor = GREEN;       
        rmemo->textbkcolor = WHITE;                                                                     /* ���ý������ı������ʽ */
        rmemo->textcolor = BLACK;
        smemo->textbkcolor = WHITE;                                                                     /* ���÷������ı������ʽ */
        smemo->textcolor = BLACK;
        sprintf((char *)ptemp, "%d.%d.%d.%d", netdev.ip[0], netdev.ip[1], netdev.ip[2], netdev.ip[3]);
        strcpy((char *)eip->text, (const char *)ptemp);                                                 /* ����IP��ַ�༭����ʾ����IP��ַ */
        tport = 8088;
        sprintf((char *)ptemp, "%d", tport);
        strcpy((char *)eport->text, (const char *)ptemp);                                               /* ���ö˿ںű༭����ʾĬ�϶˿ں� */

        tempx = protocol;
NET_RESTART:
        if (tempx != protocol)                                                                          /* ѡ��������Э�� */
        {
            /* ִ�к������� */
            protocol = tempx;                                                                           /* ����Э������ */
            if (protocol != 0)                                                                          /* TCP Client��UDP������Ŀ��IP��ַ */
            {
                ipcaption = netplay_ipcaption_tb[0][gui_phy.language];
            }
            else                                                                                        /* TCP Serverֻ����ʾ����IP��ַ */
            {
                sprintf((char *)ptemp, "%d.%d.%d.%d", netdev.ip[0], netdev.ip[1], netdev.ip[2], netdev.ip[3]);
                strcpy((char *)eip->text, (const char *)ptemp);
                ipcaption = netplay_ipcaption_tb[1][gui_phy.language];
            }
            
            tempx = (lcddev.width - 35 * ip_fsize / 2) / 3;
            gui_fill_rectangle(tempx, (ip_height - ip_fsize) / 2, ip_fsize * strlen((char *)ipcaption) / 2, ip_fsize, NET_IP_BACK_COLOR);   /* �޸�IP��ַ�����ı� */
            gui_show_string(ipcaption, tempx, (ip_height - ip_fsize) / 2, lcddev.width, ip_fsize, ip_fsize, WHITE);
            net_edit_colorset(eip, eport, protocol, connstatus);                                        /* �ػ�IP��ַ�༭�� */
            net_msg_show(ip_height, msg_height, fsize, txcnt, rxcnt, protocol, 1 << 2);                 /* ����Э����Ϣ */
        }
        
        edit_draw(eip);                                                                                 /* �ػ�IP��ַ�༭�� */
        edit_draw(eport);                                                                               /* �ػ�˿ںű༭�� */
        memo_draw_memo(smemo, 0);                                                                       /* �ػ淢�����ı��� */
        memo_draw_memo(rmemo, 0);                                                                       /* �ػ�������ı��� */
        btn_draw(protbtn);                                                                              /* �ػ�Э��ѡ��ť */
        btn_draw(connbtn);                                                                              /* �ػ����Ӱ�ť */
        btn_draw(clrbtn);                                                                               /* �ػ���������ť */
        btn_draw(sendbtn);                                                                              /* �ػ淢�Ͱ�ť */
        t9_draw(t9);                                                                                    /* �ػ����뷨 */
        net_msg_show(ip_height, msg_height, fsize, txcnt, rxcnt, protocol, 0x07);                       /* ��ʼ����ʾ��ʾ��Ϣ */
    }
    
    while (rval == 0)
    {
        tp_dev.scan(0);                             /* ������ɨ�� */
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);     /* ��ȡ������Ϣ */
        delay_ms(1000 / OS_TICKS_PER_SEC);
        
        if (system_task_return)                     /* ǿ���˳���Ч */
        {
            delay_ms(15);
            if (tpad_scan(1))                       /* �ٴ�ȷ��TPAD���ų����� */
            {
                break;                              /* �˳� */
            }
            else
            {
                system_task_return = 0;             /* ���ǿ���˳���־ */
            }
        }
        
        if (connstatus == 0)                                                                                                                                    /* ��δ���ӵ�����£����Ա༭�������ı���IP��ַ�༭�򡢶˿ںű༭�� */
        {
            if ((smemo->top < in_obj.y) && (in_obj.y < (smemo->top + smemo->height)) && (smemo->left < in_obj.x) && (in_obj.x < (smemo->left + smemo->width)))
            {
                editflag = 0;                                                                                                                                   /* ��Ǳ༭�������ı��� */
                edit_show_cursor(eip, 0);                                                                                                                       /* ����ʾ��������(IP��ַ�༭�򡢶˿ںű༭��)�Ĺ�� */
                edit_show_cursor(eport, 0);
                eip->type = 0x04;                                                                                                                               /* ����IP��ַ�༭��Ĺ�겻��˸ */
                eport->type = 0x04;                                                                                                                             /* ���ö˿ںű༭��Ĺ�겻��˸ */
                smemo->type = 0x01;                                                                                                                             /* ����༭�������ı��� */
            }
            
            if ((eip->top < in_obj.y) && (in_obj.y < (eip->top + eip->height)) && (eip->left < in_obj.x) && (in_obj.x < (eip->left + eip->width)))
            {
                if (protocol == 0)                                                                                                                              /* TCP ServerЭ���²��ɱ༭IP��ַ */
                {
                    continue;
                }

                editflag = 1;                                                                                                                                   /* ��Ǳ༭IP��ַ�༭�� */
                memo_show_cursor(smemo, 0);                                                                                                                     /* ����ʾ��������(�������ı��򡢶˿ںű༭��)�Ĺ�� */
                edit_show_cursor(eport, 0);
                eip->type = 0x06;                                                                                                                               /* ����IP��ַ�༭��Ĺ����˸ */
                eport->type = 0x04;                                                                                                                             /* ���ö˿ںű༭��Ĺ�겻��˸ */
                smemo->type = 0x00;                                                                                                                             /* ������༭�������ı��� */
            }
            
            if ((eport->top < in_obj.y) && (in_obj.y < (eport->top + eport->height)) && (eport->left < in_obj.x) && (in_obj.x < (eport->left + eport->width)))
            {
                editflag = 2;                                                                                                                                   /* ��Ǳ༭�˿ںű༭�� */
                memo_show_cursor(smemo, 0);                                                                                                                     /* ����ʾ��������(�������ı���IP��ַ�༭��)�Ĺ�� */
                edit_show_cursor(eip, 0);
                eport->type = 0x06;                                                                                                                             /* ���ö˿ںű༭��Ĺ����˸ */
                eip->type = 0x04;                                                                                                                               /* ����IP��ַ�༭��Ĺ�겻��˸ */
                smemo->type = 0x00;                                                                                                                             /* ������༭�������ı��� */
            }
        }
        
        edit_check(eip, &in_obj);       /* ���IP��ַ�༭�� */
        edit_check(eport, &in_obj);     /* ���˿ںű༭�� */
        t9_check(t9, &in_obj);          /* ������뷨 */
        memo_check(smemo, &in_obj);     /* ��鷢�����ı��� */
        memo_check(rmemo, &in_obj);     /* ���������ı��� */
        
        if (t9->outstr[0] != NULL)                                                                                      /* ���뷨���ַ���� */
        {
            if (editflag == 1)                                                                                          /* �༭IP��ַ�༭�� */
            {
                if ((t9->outstr[0] <= '9' && t9->outstr[0] >= '0') || t9->outstr[0] == '.' || t9->outstr[0] == 0x08)    /* IP��ַ�༭��ֻ��������0~9����.�����˸� */
                {
                    edit_add_text(eip, t9->outstr);                                                                     /* ����ı���IP��ַ�༭�� */
                }
            }
            else if (editflag == 2)                                                                                     /* �༭�˿ںű༭�� */
            {
                if ((t9->outstr[0] <= '9' && t9->outstr[0] >= '0') || t9->outstr[0] == 0x08)                            /* �˿ںű༭��ֻ��������0~9���˸� */
                {
                    edit_add_text(eport, t9->outstr);                                                                   /* ����ı����˿ںű༭�� */
                }
            }
            else                                                                                                        /* �༭�������ı��� */
            {
                memo_add_text(smemo, t9->outstr);                                                                       /* ����ı����������ı��� */
            }
            
            t9->outstr[0] = NULL;
        }
        
        res = btn_check(protbtn, &in_obj);                                                                                                      /* ���Э��ѡ��ť */
        if (res && ((protbtn->sta & (1 << 7)) == 0) && (protbtn->sta & (1 << 6)))                                                               /* Э��ѡ��ť�����²��ɿ� */
        {
            tempx = protocol;                                                                                                                   /* ��ȡ��ǰ��ѡ���Э�� */
            res = app_items_sel((lcddev.width - 160) / 2,                                                                                             /* ����Э��ѡ�� */
                                (lcddev.height - 192) / 2,
                                160,
                                72 + 40 * 3,
                                (uint8_t **)netplay_mode_tbl,
                                3,
                                (uint8_t *)&tempx,
                                0x90,
                                (uint8_t *)netplay_btncaption_tbl[0][gui_phy.language]);
            
            if(res) /* û�е�ȷ�ϰ�ť, tempx�����Ķ� */
            {
                tempx = protocol;    
            }
            
            /* �ָ���ɫ����ؽ��� */
            gui_fill_rectangle(0, ip_height + msg_height, lcddev.width, lcddev.height - (ip_height + msg_height) , NET_MEMO_BACK_COLOR);    /* �ָ���ɫ */
            tempy = ip_height + msg_height + rmemo_height + fsize + 2 * rm_offy;
            gui_draw_hline(0, tempy, lcddev.width, NET_COM_RIM_COLOR);              /* �������Ϸ��ķָ��� */
            tempy = ip_height + msg_height + rm_offy + fsize;
            gui_show_string(netplay_memoremind_tb[0][gui_phy.language], m_offx, tempy - fsize - rm_offy / 3, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);   /* ��ʾ�����������ı� */
            tempy = ip_height + msg_height + rm_offy * 2 + rmemo_height + fsize * 2 + sm_offy;
            gui_show_string(netplay_memoremind_tb[1][gui_phy.language], m_offx, tempy - fsize - sm_offy / 3, lcddev.width, fsize, fsize, NET_MSG_FONT_COLOR);   /* ��ʾ�����������ı� */
            /* end */
            
            goto NET_RESTART;
        }
        
        res = btn_check(connbtn, &in_obj);                                                                  /* ������Ӱ�ť */
        if (res && ((connbtn->sta & (1 << 7)) == 0) && (connbtn->sta & (1 << 6)))                           /* ���Ӱ�ť�����²��ɿ� */
        {
            connstatus = 1 - connstatus;                                                                    /* �л����ӱ�־ */
            
            if (connstatus == 1)                                                                            /* ��Ҫ�������� */
            {
                bkcolor = gui_memex_malloc(150 * 70 * 2);
                if (bkcolor == NULL)
                {
                    connstatus = 0;
                    printf("netplay ex outof memory\r\n");
                    continue;
                }
                app_read_bkcolor((lcddev.width - 150) / 2, (lcddev.height - 70) / 2, 150, 70, bkcolor);     /* Ԥ�ȶ�ȡ������ʾ��סͼ��ʾ���� */
                window_msg_box((lcddev.width - 150) / 2, (lcddev.height - 70) / 2,                          /* ��ʾ����������ʾ�� */
                               150, 70,
                               (uint8_t *)netplay_connmsg_tbl[0][gui_phy.language],
                               (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                               12, 0, 0, 0);
                
                conn_err = 0;
                netdev.socket_recv_buf[0] = gui_memex_malloc(NET_RECV_BUF_MAX_LEN);                         /* ΪCH395Q Socket 0���ݽ��ջ����������ڴ� */
                if (netdev.socket_recv_buf[0] == NULL)                                                      /* �ڴ�����ʧ�ܣ����ܽ������� */
                {
                    conn_err = 1;                                                                           /* �������ʧ�� */
                }
                else
                {
                    switch (protocol)                                                                       /* ���ݲ�ͬЭ������Socket */
                    {
                        case 0:                                                                             /* TCP Server */
                        {
                            netdev.tcp_conn_status[0] = 0;                                                  /* ���TCPΪδ���� */
                            tport = net_get_port(eport->text);                                              /* ��ȡ�˿ں� */
                            socket.socket_index = CH395Q_SOCKET_0;                                          /* Socket 0 */
                            socket.enable = CH395Q_ENABLE;                                                  /* ʹ��Socket */
                            socket.proto = CH395Q_SOCKET_TCP_SERVER;                                        /* TCP ServerЭ�� */
                            socket.sour_port = tport;                                                       /* ���ض˿ں� */
                            socket.send.buf = smemo->text;                                                  /* ���ͻ����� */
                            socket.send.size = NET_SMEMO_MAXLEN;                                            /* ���ͻ�������С */
                            socket.recv.buf =  netdev.socket_recv_buf[0];                                   /* ���ջ����� */
                            socket.recv.size = NET_RECV_BUF_MAX_LEN;                                        /* ���ջ�������С */
                            socket.send_buf_free_cb = socket_send_buf_free_cb;                              /* ���ͻ��������лص� */
                            socket.recv_cb = socket_recv_cb;                                                /* ���ջ������ǿջص� */
                            socket.tcp_conn_cb = socket_tcp_conn_cb;                                        /* TCP���ӻص� */
                            socket.tcp_disconn_cb = socket_tcp_disconn_cb;                                  /* TCP�Ͽ��ص� */
                            ch395q_socket_config(&socket);                                                  /* ����Socket */
                            conn_err = 0;                                                                   /* ������ӳɹ� */
                            for (socket_index=1; socket_index<NET_TCP_SERVER_MAX_CONN + 1; socket_index++)  /* ����TCP Server������ */
                            {
                                netdev.tcp_conn_status[socket_index] = 0;
                                netdev.socket_recv_buf[socket_index] = gui_memex_malloc(NET_RECV_BUF_MAX_LEN);
                                if (netdev.socket_recv_buf[socket_index] == NULL)
                                {
                                    conn_err = 1;
                                    break;
                                }
                                socket.socket_index = socket_index;
                                socket.recv.buf = netdev.socket_recv_buf[socket_index];
                                ch395q_socket_config(&socket);
                            }
                            break;
                        }
                        case 1:                                                                             /* TCP Client */
                        {
                            netdev.tcp_conn_status[0] = 0;                                                  /* ���TCPΪδ���� */
                            tport = net_get_port(eport->text);                                              /* ��ȡ�˿ں� */
                            socket.socket_index = CH395Q_SOCKET_0;                                          /* Socket 0 */
                            socket.enable = CH395Q_ENABLE;                                                  /* ʹ��Socket */
                            socket.proto = CH395Q_SOCKET_TCP_CLIENT;                                        /* TCP ClientЭ�� */
                            net_get_ip(eip->text, socket.des_ip);                                           /* Ŀ��IP��ַ */
                            socket.des_port = tport;                                                        /* Ŀ��˿ں� */
                            socket.sour_port = tport;                                                       /* ���ض˿ں� */
                            socket.send.buf = smemo->text;                                                  /* ���ͻ����� */
                            socket.send.size = NET_SMEMO_MAXLEN;                                            /* ���ͻ�������С */
                            socket.recv.buf =  netdev.socket_recv_buf[0];                                   /* ���ջ����� */
                            socket.recv.size = NET_RECV_BUF_MAX_LEN;                                        /* ���ջ�������С */
                            socket.send_buf_free_cb = socket_send_buf_free_cb;                              /* ���ͻ��������лص� */
                            socket.recv_cb = socket_recv_cb;                                                /* ���ջ������ǿջص� */
                            socket.tcp_conn_cb = socket_tcp_conn_cb;                                        /* TCP���ӻص� */
                            socket.tcp_disconn_cb = socket_tcp_disconn_cb;                                  /* TCP�Ͽ��ص� */
                            ch395q_socket_config(&socket);                                                  /* ����Socket */
                            retry = 50;
                            while ((netdev.tcp_conn_status[0] == 0) && (retry != 0))                        /* �ȴ�������TCP Server */
                            {
                                delay_ms(100);
                                retry--;
                            }
                            if(netdev.tcp_conn_status[0] != 0)                                              /* TCP���ӳɹ� */
                            {
                                conn_err = 0;
                                netdev.des_ip[0][0] = socket.des_ip[0];                                     /* ����Ŀ��IP��ַ�Ͷ˿ں� */
                                netdev.des_ip[0][1] = socket.des_ip[1];
                                netdev.des_ip[0][2] = socket.des_ip[2];
                                netdev.des_ip[0][3] = socket.des_ip[3];
                                netdev.des_port[0] = socket.des_port;
                            }
                            else                                                                            /* TCP����ʧ�� */
                            {
                                conn_err = 1;                                                               /* �������ʧ�� */
                            }
                            break;
                        }
                        case 2:                                                                             /* UDP */
                        {
                            tport = net_get_port(eport->text);                                              /* ��ȡ�˿ں� */
                            socket.socket_index = CH395Q_SOCKET_0;                                          /* Socket 0 */
                            socket.enable = CH395Q_ENABLE;                                                  /* ʹ��Socket */
                            socket.proto = CH395Q_SOCKET_UDP;                                               /* UDPЭ�� */
                            net_get_ip(eip->text, socket.des_ip);                                           /* Ŀ��IP��ַ */
                            socket.des_port = tport;                                                        /* Ŀ��˿ں� */
                            socket.sour_port = tport;                                                       /* ���ض˿ں� */
                            socket.send.buf = smemo->text;                                                  /* ���ͻ����� */
                            socket.send.size = NET_SMEMO_MAXLEN;                                            /* ���ͻ�������С */
                            socket.recv.buf =  netdev.socket_recv_buf[0];                                   /* ���ջ����� */
                            socket.recv.size = NET_RECV_BUF_MAX_LEN;                                        /* ���ջ�������С */
                            socket.send_buf_free_cb = socket_send_buf_free_cb;                              /* ���ͻ��������лص� */
                            socket.recv_cb = socket_recv_cb;                                                /* ���ջ������ǿջص� */
                            ch395q_socket_config(&socket);                                                  /* ����Socket */
                            conn_err = 0;
                            netdev.des_ip[0][0] = socket.des_ip[0];                                         /* ����Ŀ��IP��ַ�Ͷ˿ں� */
                            netdev.des_ip[0][1] = socket.des_ip[1];
                            netdev.des_ip[0][2] = socket.des_ip[2];
                            netdev.des_ip[0][3] = socket.des_ip[3];
                            netdev.des_port[0] = socket.des_port;
                            break;
                        }
                    }
                }
                
                if (conn_err == 0)                                                                          /* ���ӳɹ� */
                {
                    window_msg_box((lcddev.width - 150) / 2, (lcddev.height - 70) / 2,                      /* ��ʾ���ӳɹ���ʾ�� */
                                   150, 70,
                                   (uint8_t *)netplay_connmsg_tbl[2][gui_phy.language],
                                   (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                                   12, 0, 0, 0);
                }
                else                                                                                        /* ����ʧ�� */
                {
                    connstatus = 0;                                                                         /* ���Ϊδ���� */
                    for (socket_index=0; socket_index<NET_TCP_SERVER_MAX_CONN + 1; socket_index++)
                    {
                        socket.socket_index = socket_index;
                        socket.enable = CH395Q_DISABLE;
                        ch395q_socket_config(&socket);                                                      /* �ر�Socket */
                        gui_memex_free(netdev.socket_recv_buf[socket_index]);                               /* �ͷ�Socket���ݽ��ջ����� */
                    }
                    window_msg_box((lcddev.width - 150) / 2, (lcddev.height - 70) / 2,                      /* ��ʾ����ʧ����ʾ�� */
                                   150, 70,
                                   (uint8_t *)netplay_connmsg_tbl[1][gui_phy.language],
                                   (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                                   12, 0, 0, 0);
                }
                
                delay_ms(800);
                app_recover_bkcolor((lcddev.width - 150) / 2, (lcddev.height - 70) / 2, 150, 70, bkcolor);  /* �ָ�����ʾ��ס����ʾ���� */
                gui_memex_free(bkcolor);
            }
        }
        
        res = btn_check(clrbtn, &in_obj);                                               /* �����������ť */
        if (res && ((clrbtn->sta & (1 << 7)) == 0) && (clrbtn->sta & (1 << 6)))         /* ���������ť�����²��ɿ� */
        {
            rxcnt = 0;                                                                  /* ���ý��ռ��� */
            txcnt = 0;                                                                  /* ���÷��ͼ��� */
            rmemo->text[0] = 0;                                                         /* ��ս������ı������� */
            memo_draw_memo(rmemo, 1);                                                   /* �ػ�������ı��� */
            net_msg_show(ip_height, msg_height, fsize, txcnt, rxcnt, protocol, 0x07);   /* ����ͳ����Ϣ */
        }
        
        res = btn_check(sendbtn, &in_obj);                                                                                  /* ��鷢�Ͱ�ť */
        if (res && ((sendbtn->sta & (1 << 7)) == 0) && (sendbtn->sta & (1 << 6)))                                           /* ���Ͱ�ť�����²��ɿ� */
        {
            tempx = strlen((char *)smemo->text);                                                                            /* ��ȡ�������ı����ı����� */
            if ((connstatus == 1) && (tempx != 0))                                                                          /* ��������Ϊ��������״̬�����������ݳ��Ȳ�Ϊ0 */
            {
                socket_send = 0;
                for (socket_index=0; socket_index<((protocol == 0) ? (NET_TCP_SERVER_MAX_CONN + 1) : 1); socket_index++)    /* ����Socket */
                {
                    if (netdev.socket_send_busy_status[socket_index] == 0)                                                  /* ��ǰSocket�ѷ������ */
                    {
                        netdev.socket_send_busy_status[socket_index] = 1;                                                   /* ��ǵ�ǰSocket��������Ҫ���� */
                        ch395q_cmd_write_send_buf_sn(socket_index, smemo->text, tempx);                                     /* ��ǰSocket�ķ��ͻ�����д���� */
                        if (socket_send == 0)
                        {
                            socket_send = 1;
                            txcnt += strlen((char *)smemo->text);
                            net_msg_show(ip_height, msg_height, fsize, txcnt, rxcnt, protocol, 1 << 0);
                        }
                    }
                }
            }
        }
        
        if ((connstatus == 1) && (protocol == 1))          /* ������TCP Client���ӣ����Ƿ����ӳɹ�ȡ����TCP������״̬ */
        {
            connstatus = netdev.tcp_conn_status[0];
        }
        
        if (netdev.phy_status == CH395Q_CMD_PHY_DISCONN)    /* ��PHY�Ͽ�������Ϊδ����״̬ */
        {
            connstatus = 0;
        }
        
        if (connstatus == 1)                                                                                                            /* ������״̬ */
        {
            for (socket_index=0; socket_index<((protocol == 0) ? NET_TCP_SERVER_MAX_CONN + 1 : 1); socket_index++)                      /* ����Socket */
            {
                if (netdev.socket_recv_status[socket_index] == 1)                                                                       /* ���յ����� */
                {
                    if (((netdev.des_ip[socket_index][0] != oldaddr[0]) ||
                         (netdev.des_ip[socket_index][1] != oldaddr[1]) ||
                         (netdev.des_ip[socket_index][2] != oldaddr[2]) ||
                         (netdev.des_ip[socket_index][3] != oldaddr[3])) ||                                                             /* �µ�Ŀ��IP��ַ��˿ں� */
                        (netdev.des_port[socket_index] != oldport))
                    {
                        oldaddr[0] = netdev.des_ip[socket_index][0];                                                                    /* �����µ�Ŀ��IP��ַ�Ͷ˿ں� */
                        oldaddr[1] = netdev.des_ip[socket_index][1];
                        oldaddr[2] = netdev.des_ip[socket_index][2];
                        oldaddr[3] = netdev.des_ip[socket_index][3];
                        oldport = netdev.des_port[socket_index];
                        sprintf((char *)ptemp, "[From:%d.%d.%d.%d:%d]:\r\n", oldaddr[0], oldaddr[1], oldaddr[2], oldaddr[3], oldport);  /* ׼������µ�Ŀ��IP��ַ�Ͷ˿ں� */
                        tempx = strlen((char *)rmemo->text) + strlen((char *)ptemp);
                        if (tempx > NET_RMEMO_MAXLEN)
                        {
                            rmemo->text[0] = 0;
                        }
                        strcat(((char *)rmemo->text), (char *)ptemp);
                    }
                    
                    tempx = strlen((char *)rmemo->text) + netdev.socket_recv_len[socket_index];
                    if (tempx > NET_RMEMO_MAXLEN)
                    {
                        rmemo->text[0] = 0;
                    }
                    netdev.socket_recv_buf[socket_index][netdev.socket_recv_len[socket_index]] = '\0';
                    strcat(((char *)rmemo->text), (char *)netdev.socket_recv_buf[socket_index]);
                    rxcnt += netdev.socket_recv_len[socket_index];
                    memo_draw_memo(rmemo, 1);
                    net_msg_show(ip_height, msg_height, fsize, txcnt, rxcnt, protocol, 1 << 1);                                         /* ������ʾ��Ϣ */
                    
                    netdev.socket_recv_status[socket_index] = 0;
                }
            }
        }
        
        if (oldconnstatus != connstatus)                                                                        /* ����״̬�ı� */
        {
            oldconnstatus = connstatus;                                                                         /* ���浱ǰ����״̬ */
            
            if (connstatus == 0)                                                                                /* ����״̬�����ӱ�Ϊ�Ͽ� */
            {
                oldaddr[0] = 0;
                oldaddr[1] = 0;
                oldaddr[2] = 0;
                oldaddr[3] = 0;
                oldport = 0;
                for (socket_index=0; socket_index<NET_TCP_SERVER_MAX_CONN + 1; socket_index++)
                {
                    netdev.tcp_conn_status[socket_index] = 0;
                    netdev.socket_send_busy_status[socket_index] = 0;
                    netdev.socket_recv_status[socket_index] = 0;
                    netdev.socket_recv_len[socket_index] = 0;
                    socket.socket_index = socket_index;
                    socket.enable = CH395Q_DISABLE;                                                             /* �ر�Socket */
                    ch395q_socket_config(&socket);
                    gui_memex_free(netdev.socket_recv_buf[socket_index]);                                       /* �ͷ�Socket�����ݽ��ջ����� */
                }
                bkcolor = gui_memex_malloc(200 * 80 * 2);                                                       /* ��ʾ���ӶϿ���ʾ��Ϣ */
                if (bkcolor != NULL)
                {
                    app_read_bkcolor((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, bkcolor);
                    window_msg_box((lcddev.width - 200) / 2, (lcddev.height - 80) / 2,
                                   200, 80,
                                   (uint8_t *)netplay_connmsg_tbl[3][gui_phy.language],
                                   (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                                   12, 0, 0, 0);
                    delay_ms(800);
                    app_recover_bkcolor((lcddev.width - 200) / 2, (lcddev.height - 80) / 2, 200, 80, bkcolor);
                    gui_memex_free(bkcolor);
                }
                protbtn->sta = 0;                                                                               /* ����Э��ѡ��ť */
                connbtn->caption = netplay_btncaption_tbl[1][gui_phy.language];                                 /* �������Ӱ�ť���ı� */
            }
            else                                                                                                /* ����״̬�ɶϿ���Ϊ���� */
            {
                protbtn->sta = 2;                                                                               /* ����Э��ѡ��ťΪ�Ǽ��� */
                connbtn->caption = netplay_btncaption_tbl[2][gui_phy.language];                                 /* �������Ӱ�ť���ı� */
                editflag = 0;                                                                                   /* ֻ����༭������������� */
                edit_show_cursor(eip, 0);                                                                       /* �ر�IP��ַ�༭��Ĺ�� */
                edit_show_cursor(eport, 0);                                                                     /* �رն˿ںű༭��Ĺ�� */
                eip->type = 0x04;                                                                               /* IP��ַ�༭��Ĺ�겻��˸ */
                eport->type = 0x04;                                                                             /* �˿ںű༭��Ĺ�겻��˸ */
                smemo->type = 0x01;                                                                             /* ����༭���������������˸����ʾ��� */
            }
            
            btn_draw(protbtn);                                                                                  /* �ػ�Э��ѡ��ť */
            btn_draw(connbtn);                                                                                  /* �ػ����Ӱ�ť */
            net_edit_colorset(eip, eport, protocol, connstatus);                                                /* �ػ�IP��ַ�Ͷ˿ںű༭�� */
        }
    }
    
    if(connstatus == 1)                                                                 /* �������ӣ������ȹر����� */
    {
        connstatus = 0;                                                                 /* ���Ϊδ���� */
        for (socket_index=0; socket_index<NET_TCP_SERVER_MAX_CONN + 1; socket_index++)
        {
            socket.socket_index = socket_index;
            socket.enable = CH395Q_DISABLE;                                             /* �ر�Socket */
            ch395q_socket_config(&socket);
            gui_memex_free(netdev.socket_recv_buf[socket_index]);                       /* �ͷ�Socket���ݽ��ջ����� */
        }
    }
    
    gui_memin_free(ptemp);  /* �ͷ��ڴ沢ɾ�������ؼ� */
    edit_delete(eip);
    edit_delete(eport);
    memo_delete(rmemo);
    memo_delete(smemo);
    t9_delete(t9);
    btn_delete(protbtn);
    btn_delete(connbtn);
    btn_delete(clrbtn);
    btn_delete(sendbtn);
    system_task_return = 0;
    
    return 0;
}

/**
 * @brief       ����ͨ�Ų���
 * @param       ��
 * @retval      ��
 */
uint8_t net_play(void)
{
    uint8_t res;
    uint16_t yoff = 0;
    _btn_obj *tbtn = 0;
    uint8_t retry = 0;
    uint8_t i;
    
    if (lcddev.width == 240)    /* ����LCD�ߴ����ò��� */
    {
        yoff = 170;
    }
    else
    {
        yoff = 210;
    }
    
    tbtn = btn_creat((lcddev.width - 180) / 2, yoff + (lcddev.height - yoff - 60) / 2, 180, 60, 0, 0);      /* ������׼��ť */
    if (tbtn == NULL)
    {
        return 1;
    }

//    if (audiodev.status & (1 << 7))                                                                         /* �жϵ�ǰ�Ƿ����ڲ������� */
//    {
//        audio_stop_req(&audiodev);                                                                          /* ֹͣ��Ƶ���� */
//        audio_task_delete();                                                                                /* ɾ�����ֲ������� */
//    }

    window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2,                                     /* ��ʾ���ڳ�ʼ����ʾ�� */
                   220, 100,
                   (uint8_t *)netplay_remindmsg_tbl[0][gui_phy.language],
                   (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                   12, 0, 0, 0);
    res = ch395q_init(1);                                                                                   /* ��ʼ��CH395Q */
    if (res == 0)                                                                                           /* CH395Q��ʼ���ɹ� */
    {
        ch395q_cmd_get_mac_addr(netdev.mac);
        ch395q_net_config(CH395Q_ENABLE, NULL, NULL, NULL, phy_conn_cb, phy_disconn_cb, dhcp_success_cb);   /* CH395Q�������ã�����DHCP */
        ch395q_handler_task_creat();                                                                        /* ����CH395Q�������� */
        
        window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2,                                 /* ��ʾ�ȴ�DHCP��ʾ�� */
                       220, 100,
                       (uint8_t *)netplay_remindmsg_tbl[2][gui_phy.language],
                       (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                       12, 0, 0, 0);
        retry = 100;
        while ((netdev.dhcp_status != 1) && (retry != 0))                                                   /* ��DHCP�ɹ� */
        {
            delay_ms(100);
            retry--;
        }
        
        if (netdev.dhcp_status == 1)                                                                        /* DHCP�ɹ� */
        {
            window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2,                             /* ��ʾDHCP�ɹ���ʾ�� */
                           220, 100,
                           (uint8_t *)netplay_remindmsg_tbl[3][gui_phy.language],
                           (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                           12, 0, 0, 0);
        }
        else                                                                                                /* DHCPʧ�� */
        {
            window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2,                             /* ��ʾDHCPʧ����ʾ�� */
                           220, 100,
                           (uint8_t *)netplay_remindmsg_tbl[4][gui_phy.language],
                           (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                           12, 0, 0, 0);
            ch395q_net_config(CH395Q_DISABLE,                                                               /* ���½���CH395Q�������ã��ر�DHCP */
                              static_ip, static_gwip, static_mask,
                              phy_conn_cb, phy_disconn_cb, dhcp_success_cb);
            for (i=0; i<sizeof(netdev.ip); i++)
            {
                netdev.ip[i] = static_ip[i];
            }
            for (i=0; i<sizeof(netdev.gwip); i++)
            {
                netdev.gwip[i] = static_gwip[i];
            }
            for (i=0; i<sizeof(netdev.mask); i++)
            {
                netdev.mask[i] = static_mask[i];
            }
        }
        delay_ms(1000);
        tbtn->caption = netplay_tbtncaption_tb[gui_phy.language];                                           /* ��ť���� */
        tbtn->font = 24;                                                                                    /* ��ť���� */
        net_load_ui();                                                                                      /* ������·���������� */
        btn_draw(tbtn);                                                                                     /* ���Ʋ��԰�ť */
        
        while (1)
        {
            tp_dev.scan(0);                                                     /* ������ɨ�� */
            in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);                             /* ��ȡ������Ϣ */
            delay_ms(5);
            
            if (system_task_return)                                             /* ǿ���˳���Ч */
            {
                delay_ms(15);
                if (tpad_scan(1))                                               /* �ٴ�ȷ��TPAD���ų����� */
                {
                    ch395q_handler_task_delete();                               /* ɾ��CH395Q�������� */
                    break;
                }
                else
                {
                    system_task_return = 0;                                     /* ���ǿ���˳���־ */
                }
            }
            
            res = btn_check(tbtn, &in_obj);                                     /* ��鿪ʼ���԰�ť */
            if (res && ((tbtn->sta & (1 << 7)) == 0) && (tbtn->sta & (1 << 6))) /* ��ʼ���԰�ť�����²��ɿ� */
            {
                net_test();                                                     /* ����������� */
                net_load_ui();                                                  /* ���¼�����·���������� */
                btn_draw(tbtn);                                                 /* �ػ濪ʼ���԰�ť */
            }
        }
    }
    else                                                                        /* CH395Q��ʼ��ʧ�� */
    {
        window_msg_box((lcddev.width - 220) / 2, (lcddev.height - 100) / 2,     /* ��ʾ��ʼ��ʧ����ʾ�� */
                       220, 100,
                       (uint8_t *)netplay_remindmsg_tbl[1][gui_phy.language],
                       (uint8_t *)APP_REMIND_CAPTION_TBL[gui_phy.language],
                       12, 0, 0, 0);
        delay_ms(2000);
    }
    
    ch395q_cmd_enter_sleep();                                                   /* CH395Q����͹���ģʽ */
    btn_delete(tbtn);
    system_task_return = 0;
    
    return 0;
}
