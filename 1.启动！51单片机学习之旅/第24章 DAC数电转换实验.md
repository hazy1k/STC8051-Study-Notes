# 第二十四章 DAC数电转换实验

## 1. 导入

    在前面章节， 我们介绍了如何使用 51 单片机来采集外部模拟信号， 这一章来介绍下如何使用 51 单片机输出模拟信号， 要让 51 单片机输出模拟信号， 同样需要相应的转换器， 这种转换器我们称之为 DAC 数模转换器， 但 DAC 专用芯片价格是比较高的， 在实际应用中通常使用 PWM 技术来模拟 DAC 输出。 我们开发板上集成了一个 DAC(PWM)模块电路， 通过单片机 IO 口生成 PWM 波来模拟 DAC 输出。

## 2. DAC介绍

    DAC（ Digital to analog converter） 即数字模拟转换器， 它可以将数字信号转换为模拟信号。 它的功能与 ADC 相反。 在常见的数字信号系统中， 大部分传感器信号被转化成电压信号， 而 ADC 把电压模拟信号转换成易于计算机存储、处理的数字编码， 由计算机处理完成后， 再由 DAC 输出电压模拟信号， 该电压模拟信号常常用来驱动某些执行器件， 使人类易于感知。 如音频信号的采集及还原就是这样一个过程。

    关于更多DAC介绍：[数模转换器的基本原理及DAC类型简介 | 贸泽工程师社区 (eetrend.com)](https://mouser.eetrend.com/content/2017/100006118.html)

    [关于DAC的原理_dac工作原理-CSDN博客](https://blog.csdn.net/always_one/article/details/104560604)

## 3. PWM介绍

    出于成本考虑， 在实际开发应用中， 使用较多的是通过 PWM 来模拟 DAC 输出。下面就先来了解下 PWM 相关概念：

    PWM 是 Pulse Width Modulation 的缩写， 中文意思就是脉冲宽度调制， 简称脉宽调制。 它是利用微处理器的数字输出来对模拟电路进行控制的一种非常有效的技术， 其控制简单、 灵活和动态响应好等优点而成为电力电子技术最广泛应用的控制方式， 其应用领域包括测量， 通信， 功率控制与变换， 电动机控制、 伺服控制、 调光、 开关电源， 甚至某些音频放大器。

    关于PWM更多介绍：[STM32——PWM原理及应用（附代码）_pwm控制代码-CSDN博客](https://blog.csdn.net/m0_60503814/article/details/136163178)

    [PWM的基本原理及如何产生PWM_pwm csdn-CSDN博客](https://blog.csdn.net/weixin_45237963/article/details/122033273)

## 4. 硬件设计

本实验使用到硬件资源如下：

- DAC(PWM)模块

    下面来看下开发板上 DAC(PWM)模块电路， 如下图所示：

![屏幕截图 2024 06 20 092539](https://img.picgo.net/2024/06/20/-2024-06-20-092539ab6109daa9f80531.png)

    从上图中可知， 该模块是独立的， 由 J50 端子输入 PWM 信号， 可使用单片机P2.1 管脚输出 PWM 信号至 J50 端子， DAC1 为 PWM 输出信号， 将其连接一个 LED，这样可以通过指示灯的状态直观的反映出 PWM 输出电压值变化。 LM358 芯片与这些电容电阻构成了一个跟随电路， 即输入是多少， 输出即为多大电压， 输出电压范围是 0-5V。 输出信号由 J52 端子的 DAC1 引出， 在其端子上还有一个 AIN3 脚，它是上一章介绍 ADC 时的外部模拟信号输入通道。 如果使用短接片将 DAC1 和AIN3 短接， 这样就可以使用 XPT2046 芯片采集检测 PWM 输出信号。

## 5. 软件设计

本章所要实现的功能是： DAC(PWM)模块上的指示灯 DA1 呈呼吸灯效果， 由暗变亮再由亮变暗。

    程序框架如下：

- 编写 PWM 函数

- 编写主函数

```c
#ifndef _pwm_H
#define _pwm_H

#include "public.h"

//管脚定义
sbit PWM = P2^1;

//变量声明
extern unsigned char gtim_scale;

//函数声明
void pwm_init(unsigned char tim_h, unsigned char tim_l, unsigned int tim_scale, unsigned char duty);
void pwm_set_duty_cycle(unsigned char duty);

#endif
```

```c
#include "pwm.h"

//全局变量定义
unsigned char gtim_h = 0; // 保存定时器初值高8位
unsigned char gtim_l = 0; // 保存定时器初值低8位
unsigned char gduty = 0;  // 保存PWM占空比
unsigned char gtim_scale = 0; // 保存PWM周期=定时器初值*tim_scale

// PWM初始化函数
void pwm_init(unsigned char tim_h, unsigned char tim_l, unsigned int tim_scale, unsigned char duty)
{
    gtim_h = tim_h; // 将传入的初值保存在全局变量中，方便中断函数继续调用
    gtim_l = tim_l;
    gduty = duty;
    gtim_scale = tim_scale;

    TMOD |= 0X01; // 选择为定时器0模式，工作方式1
    TH0 = gtim_h; // 定时初值设置 
    TL0 = gtim_l;    
    ET0 = 1;      // 打开定时器0中断允许
    EA = 1;       // 打开总中断
    TR0 = 1;      // 打开定时器
}

// 设置占空比
void pwm_set_duty_cycle(unsigned char duty)
{
    gduty = duty;    
}

void pwm(void) interrupt 1 // 定时器0中断函数
{
    static unsigned int time = 0;

    TH0 = gtim_h; // 定时初值设置 
    TL0 = gtim_l;

    time++;
    if(time >= gtim_scale) // PWM周期=定时器初值*gtim_scale，重新开始计数
        time=0;
    if(time <= gduty) // 占空比    
        PWM=1;
    else
        PWM=0;        
}
```

这里PWM初始化函数倒是好理解，我们重点分析一下定时器中断函数：

1. **重新设置定时器初值**：

```c
TH0 = gtim_h; // 重新设置定时器高8位
TL0 = gtim_l; // 重新设置定时器低8位
```

2. **计数**：

```c
time++;
if(time >= gtim_scale) // 周期到达，重置计数
    time = 0;
```

3. **生成 PWM 信号**：

```c
if(time <= gduty) // 根据占空比控制 PWM 信号
    PWM = 1;
else
    PWM = 0;
```

- **定时器重载**：在每次中断时重新加载定时器初值，确保周期性。
- **计数与周期**：`time` 用于计算 PWM 周期，`gtim_scale` 定义了周期的总计数值。
- **PWM 控制**：通过比较 `time` 和 `gduty`，决定 `PWM` 的高低状态，从而实现占空比控制。

```c
#include "public.h"
#include "pwm.h"

void main()
{    
    unsigned char dir = 0; // 默认为0
    unsigned char duty = 0;

    pwm_init(0XFF,0XF6, 100, 0); // 定时时间为0.01ms，PWM周期是100*0.01ms=1ms，占空比为0%

    while(1)
    {
        if(dir == 0) // 当dir为递增方向
        {
            duty++; // 占空比递增
            if(duty == 70)
                dir = 1;// 当到达一定值切换方向，占空比最大能到100，但到达70左右再递增，肉眼也分辨不出亮度变化    
        }
        else
        {
            duty--;
            if(duty == 0)
                dir = 0; // 当到达一定值切换方向    
        }
        pwm_set_duty_cycle(duty); // 设置占空比
        delay_ms(1); // 短暂延时，让呼吸灯有一个流畅的效果            
    }
}
```

## 6. 小结

    从整体上简单分析一下代码：

    上述代码主要是基于定时器实现 PWM 输出， PWM 初始化实际上为定时器 0 初始化， pwm_init 函数有 4 个入口参数， tim_h 和 tim_l 为定时器定时初值， 即进入中断时间； tim_scale 参数为 PWM 的周期倍数， 使用该值乘以定时器初值可得出 PWM 的周期； duty 参数为 PWM 占空比， 即一个周期内高电平所占的时间比例。在 pwm 初始化函数内， 将函数入口参数通过全局变量保存， 方便在后续中断函数内使用。

    pwm_set_duty_cycle 函数是占空比设置函数， 该函数有一个入口参数， 用于设置 PWM 占空比， 注意， 该值不能超过初始化中的 PWM 的周期倍数值。

    最后就是定时器 0 的中断服务函数， 在中断内定义了一个静态变量用于统计进入中断的次数时间， 当进入中断次数时间大于等于 gtim_scale 周期倍数， 则重新开始计数， 表示 PWM 周期为定时器初值*gtim_scale； 然后当计数次数时间小于等于设置的占空比次数时间， 则使对应 IO 输出高电平， 否则输出低电平。

    主函数比较简单， 首先调用外设驱动头文件， 然后进入主函数初始化 PWM，将定时器设置为 0.01ms， 初值为 0XFFF6， 即每隔 0.01ms 进入一次中断。 PWM 周期倍数设置为 100， 即 PWM 周期为 1ms， 占空比设置为 0。 最后进入 while 循环，通过 dir 切换方向实现 duty 值的自增和自减来调节占空比， 将该值传入到占空比调节函数 pwm_set_duty_cycl。 为了使呼吸灯流畅， 每调节占空比短暂延时一下

---

2024.7.23 第一次修订

2024.8.23 第二次修订，后期不再维护
