/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：DAC 输出正弦波 实验
 * 实验平台：正点原子 精英F103开发板
 * 实验目的：学习STM32 DAC DMA输出正弦波的使用

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED灯
     DS0（RED）     : LED0 - PB5
     DS1（GREEN）   : LED1 - PE5
 * 2 独立按键
     KEY0    - PE4
     KEY1    - PE3
     KEY_UP  - PA0  (程序中的宏名:WK_UP)
 * 3 串口1 (PA9/PA10连接在板载USB转串口芯片CH340上面)
 * 4 正点原子2.8/3.5/4.3/7/10寸TFTLCD模块(仅限MCU屏，16位8080并口驱动)
 * 5 ADC3  : 通道1  - PA1
 * 6 DAC1  : 通道1  - PA4

 ***************************************************************************************************
 * 实验现象
 * 1 本实验使用DAC输出正弦波，通过KEY0/KEY1两个按键，控制DAC1的通道1输出两种正弦波，需要通过示波器
     接PA4进行观察。TFTLCD显示DAC转换值、电压值和ADC的电压值。
 * 2 LED0闪烁 ,提示程序运行。

 ***************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 P3的PA9/PA10必须通过跳线帽连接在RXD/TXD上
 * 6 本例程仅支持MCU屏，不支持RGB屏
 * 7 4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).

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