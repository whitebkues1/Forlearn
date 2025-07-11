/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：光敏传感器 实验
 * 实验平台：正点原子 精英F103开发板
 * 实验目的：学习光敏传感器的使用,并进一步熟悉STM32内部ADC的使用

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED灯
     DS0（RED）     : LED0 - PB5
     DS1（GREEN）   : LED1 - PE5
 * 2 串口1 (PA9/PA10连接在板载USB转串口芯片CH340上面)
 * 3 正点原子2.8/3.5/4.3/7/10寸TFTLCD模块(仅限MCU屏，16位8080并口驱动)
 * 4 ADC(STM32内部ADC3,通道6,即:ADC3_CH6,连接在PF8上面)
 * 5 光敏传感器 - (连接在PF8).

 ***************************************************************************************************
 * 实验现象
 * 1 本实验通过ADC3的通道6(PF8)读取光敏传感器（LS1）的电压值，并转换为0~100的光线强度值，显示在LCD
     模块上面。光线越亮，值越大；光线越暗，值越小。大家可以用手指遮挡LS1和用手电筒照射LS1，来查看光
     强变化。
 * 2 LED0闪烁 ,提示程序运行。

 ***************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 USART1的PA9/PA10必须通过跳线帽连接在RXD/TXD上
 * 6 4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).

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