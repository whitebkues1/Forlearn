/**
 ***************************************************************************************************
 * 实验简介
 * 实验名称：电容触摸按键 实验
 * 实验平台：正点原子 精英F103开发板
 * 实验目的：进一步学习定时器的输入捕获功能

 ***************************************************************************************************
 * 硬件资源及引脚分配
 * 1 LED灯
     DS0（RED）     : LED0 - PB5
     DS1（GREEN）   : LED1 - PE5
 * 2 TPAD电容触摸按键(右下角LOGO,即TPAD,连接在PA1)
 * 3 定时器5(TIM5),TIM5的通道2(TIM5_CH2,连接在PA1上面).

 ***************************************************************************************************
 * 实验现象
 * 1 本实验,利用开发板板载的电容触摸按键(右下角白色LOGO,即TPAD)，
     通过TIM5_CH2（PA1）对电容触摸按键的检测，实现对DS1的控制,
     下载本代码后，我们通过按压开发板右下角的TPAD按钮，就可以控制DS1的亮灭了。
 * 2 LED0闪烁 ,提示程序运行。

 ***************************************************************************************************
 * 注意事项
 * 1 本实验需要将P7端子用跳线帽连接ADC&TPAD.
 

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