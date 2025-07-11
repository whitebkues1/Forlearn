/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：uC/OS-III 二值信号量实验
 * 实验平台：正点原子 精英F103开发板
 * 实验目的：学习 uC/OS-III 的二值信号量

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 串口1 （PA9/PA10连接在板载USB转串口芯片CH340上面）
 * 2 正点原子 2.8/3.5/4.3/7/10寸TFTLCD模块(仅限MCU屏，16位8080并口驱动)

 ***************************************************************************************************
 * 实验现象
 * 1 TFTLCD模块显示本实验的相关信息。
 * 2 任务1和任务2不断通过串口打印信息。

 ***************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 P3的PA9/PA10必须通过跳线帽连接在RXD/TXD上
 * 6 本例程仅支持MCU屏，不支持RGB屏

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