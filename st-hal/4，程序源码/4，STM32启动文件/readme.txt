1，startup_stm32f103xe.s，正点原子 修改后的STM32F103xx系列的启动文件，寄存器和HAL库版本存在差别

针对：startup_stm32f103xe.s文件，我们在官方启动文件的基础上，进行微改：
1，未用到编译器自带的内存管理(malloc,free等)，设置Heap_Szie为0
2，寄存器版本代码，因为没有用到SystemInit函数，所以注释掉部分代码为防止报错！
3，HAL库版本代码，建议加上部分代码（外部必须实现SystemInit函数），以初始化stm32时钟等
    
对于寄存器版本的例程，直接使用寄存器版本文件夹下的startup_stm32f103xe.s即可。
对于HAL库版本的例程，直接使用HAL库版本文件夹下的startup_stm32f103xe.s即可。




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