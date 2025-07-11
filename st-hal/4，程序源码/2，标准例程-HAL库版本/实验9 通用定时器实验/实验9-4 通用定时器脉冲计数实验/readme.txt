/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：通用定时器脉冲计数实验 实验
 * 实验平台：正点原子 精英F103开发板
 * 实验目的：学习通用定时器输入脉冲计数

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED灯
     DS0（RED）     : LED0 - PB5
     DS1（GREEN）   : LED1 - PE5
 * 2 独立按键
     KEY0    - PE4
     KEY1    - PE3
     KEY_UP  - PA0  (程序中的宏名:WK_UP)
 * 3 定时器2,使用TIM2通道1,将TIM2_CH1连接到PA0。

 ***************************************************************************************************
 * 实验现象
 * 1 利用TIM2_CH1来做输入捕获，
     我们将捕获PA0上的上升沿，通过按WK_UP按键读取CNT和溢出次数, 经过简单计算, 可以得到当前的计数值, 从而实现脉冲计数
     通过串口打印出来当前的计数值，大家可以通过按KEY0重设当前计数
 * 2 LED0用来指示程序运行,200ms变更一次状态。

 ***************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 排针P3的PA9/PA10必须通过跳线帽连接在RXD/TXD上

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