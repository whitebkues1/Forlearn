/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：无线通信 实验
 * 实验平台：正点原子 精英F103开发板
 * 实验目的：学习NRF24L01 2.4G无线模块和SPI的使用

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED灯
     DS0（RED）     : LED0 - PB5
     DS1（GREEN）   : LED1 - PE5
 * 2 串口1 (PA9/PA10连接在板载USB转串口芯片CH340上面)
 * 3 正点原子2.8/3.5/4.3/7/10寸TFTLCD模块(仅限MCU屏，16位8080并口驱动)
 * 4 NRF24L01模块(SPI2(PB13/PB14/PB15)/IRQ(PG6)/CS(PG7)/CE(PG8))

 ***************************************************************************************************
 * 实验现象
 * 1 本实验开机后,先检测NRF24L01模块是否存在，在检测到NRF24L01模块之后，通过KEY0和KEY1
     设置模块的工作模式，在设定好工作模式之后，就会不停的发送/接收数据。
 * 2 LED0闪烁 ,提示程序运行。

 ***************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 USART1的PA9/PA10必须通过跳线帽连接在RXD/TXD上
 * 6 本例程仅支持MCU屏，不支持RGB屏
 * 7 需自备NRF24L01模块2个，MINI PRO STM32F103开发板 2块
 * 8 NRF24L01模块插在开发板P2位置(朝外插)
 * 9 本实验需要2套ALIENTEK开发板才可以正常测试，一块做发送,一块做接收

 ***********************************************************************************************************
 * 公司名称：广州市星翼电子科技有限公司（正点原子）
 * 电话号码：020-38271790
 * 传真号码：020-36773971
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 最新资料：www.openedv.com/docs/index.html
 *
 * 在线视频：www.yuanzige.com
 * B 站视频：space.bilibili.com/394620890
 * 公 众 号：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * 抖    音：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 ***********************************************************************************************************
 */