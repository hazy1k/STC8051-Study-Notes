# 第三十五章 了解 PWM、频率与占空比

## 1. 导入

PWM（Pulse Width Modulation，脉宽调制）用固定频率的“高低脉冲”来等效控制模拟量（亮度、速度、功率）。核心三要素：

- 频率 f（Hz）
- 周期 T（s），与频率互为倒数：T=f1​
- 占空比 D（0~~1 或 0%~~100%），一周期内高电平所占比例

作用直观：

- 驱动 LED：Vavg​≈D⋅Vin​，人眼积分效果呈亮度渐变
- 直流电机：平均电压升降 → 转速可调（频率太低会抖，太高开关损耗大）
- 有源/无源蜂鸣器：以音频频率开关 → 出声
- 舵机：严格来说是“脉宽控制”而非固定占空比 PWM，但可用定时策略实现

8051 标准内核无硬件 PWM，常见做法：

- 软件 PWM（定时器中断 + 比较）
- 增强型 51（如 STC8）自带 PWM 外设（进阶再述）

---

## 2. 基本概念与公式

- 频率与周期：f=T1​
- 占空比：D=Tthigh​​
- 平均电压（慢变化、经负载/RC 滤波）：Vavg​≈D⋅Vin​
- 分辨率与步距（N 位 PWM）：步长 ΔD=2N1​

软件 PWM 的资源消耗来自“中断频率”：

- 若将一周期离散成 `STEPS` 份，每步中断一次，则中断频率 fISR​=fPWM​×STEPS
- 例：想要 LED 200 Hz 且 64 级亮度 → fISR​=200×64=12.8 kHz（在 11.0592MHz 8051 上可行）
- 想要 1 kHz 且 256 级 → 256 kHz ISR（在标准 8051 上不可行）

结论：软件 PWM 要在“频率”和“分辨率”间折中；电机驱动高频 PWM（>10kHz）更适合硬件 PWM 或专用驱动。

---

## 3. 经典 8051 软件 PWM 方案

### 3.1 固定频率比较型（多通道 LED 调光）

- 用定时器周期中断作为“时间片”
- 设一个递增计数 `slot`（0..STEPS-1），每通道若 `slot < duty` 则输出高，否则低
- 优点：占空比直观、频率稳定；缺点：ISR 频率=频率×步数

下例配置：Fosc=11.0592MHz，目标 `f_PWM=200 Hz`，`STEPS=64`（≈12.8k 中断/秒，可驱动多通道 LED，肉眼无闪）

```c
#include <reg52.h>
/* 目标: PWM 200Hz, 64级; Fosc=11.0592MHz
   中断频率 f_ISR = 200*64 = 12800 Hz
   Timer0方式1 16位:
   Tmc = 12/Fosc ≈ 1.085us
   中断间隔 Ts = 1/f_ISR ≈ 78.125us
   计数 = Ts/Tmc ≈ 72 → Reload = 65536-72 = 0xFFB8 (TH0=0xFF, TL0=0xB8) */

sbit PWM1 = P1^0;   // 高电平=点亮（按接法可反相）
sbit PWM2 = P1^1;
sbit PWM3 = P1^2;
sbit PWM4 = P1^3;

#define PWM_STEPS 64

volatile unsigned char slot = 0;       // 0..63
volatile unsigned char d1=0, d2=0, d3=0, d4=0; // 各通道占空(0..64)

void timer0_init_pwm_isr(void){
    TMOD = (TMOD & 0xF0) | 0x01; // T0方式1
    TH0 = 0xFF; TL0 = 0xB8;      // 约 78us
    ET0 = 1; EA = 1; TR0 = 1;
}

void set_duty(unsigned char ch, unsigned char duty){ // duty:0..64
    if(duty > PWM_STEPS) duty = PWM_STEPS;
    switch(ch){
        case 1: d1 = duty; break;
        case 2: d2 = duty; break;
        case 3: d3 = duty; break;
        case 4: d4 = duty; break;
    }
}

void t0_isr(void) interrupt 1 {
    TH0 = 0xFF; TL0 = 0xB8;

    // 比较输出
    PWM1 = (slot < d1) ? 1 : 0;
    PWM2 = (slot < d2) ? 1 : 0;
    PWM3 = (slot < d3) ? 1 : 0;
    PWM4 = (slot < d4) ? 1 : 0;

    // 递增时间片
    slot++;
    if(slot >= PWM_STEPS) slot = 0;
}

/* 简易延时 */
void delay_ms(unsigned int ms){ unsigned int i,j; for(i=0;i<ms;i++) for(j=0;j<125;j++); }

void main(void){
    PWM1 = PWM2 = PWM3 = PWM4 = 0;
    timer0_init_pwm_isr();

    // 呼吸渐变演示（四路相位错开）
    while(1){
        unsigned char x;
        for(x=0; x<=PWM_STEPS; x++){
            set_duty(1, x);
            set_duty(2, (x+16) % (PWM_STEPS+1));
            set_duty(3, (x+32) % (PWM_STEPS+1));
            set_duty(4, (x+48) % (PWM_STEPS+1));
            delay_ms(20);
        }
    }
}
```

小贴士：

- 接法决定“高/低有效”。若 LED 用“灌电流”（P1→LED→电阻→VCC，低电平亮），把输出逻辑取反即可。
- 多通道越多、STEPS 越大，中断时间越紧；必要时减小 STEPS 或降低频率。

### 3.2 误差累加（Δ-Σ）“伪 PWM”（低中断率，视觉级调光）

- 每次系统节拍（如 1ms）做一次：`acc += duty; if(acc>=MAX){输出=1; acc-=MAX;} else 输出=0;`
- 平均占空比接近 `duty/MAX`，但瞬时周期不固定（抖动分散在时间上）
- 适合 LED 视觉调光；不适合电机等要求固定频率负载

示例（单通道，1ms 节拍，MAX=255）：

```c
#include <reg52.h>
sbit LED = P1^0; // 高电平亮(按接法调)
volatile unsigned char duty = 128; // 0..255
volatile unsigned int  ms = 0;
static unsigned int acc = 0;

void t0_init_1ms(void){ // 11.0592MHz -> TH0/TL0=0xFC66
    TMOD = (TMOD & 0xF0) | 0x01;
    TH0=0xFC; TL0=0x66;
    ET0=1; EA=1; TR0=1;
}
void t0_isr(void) interrupt 1{
    TH0=0xFC; TL0=0x66;
    ms++;
    acc += duty;
    if(acc >= 256){ LED=1; acc -= 256; }
    else          { LED=0; }
}
void delay_ms(unsigned int d){ unsigned int t=ms; while((unsigned int)(ms-t)<d); }

void main(void){
    LED=0; t0_init_1ms();
    while(1){
        unsigned char x;
        for(x=0;x<255;x++){ duty=x; delay_ms(10); }
        for(x=255;x>0;x--){ duty=x; delay_ms(10); }
    }
}
```

优点：ISR 仅 1kHz，CPU 负担极小；缺点：瞬时闪烁频率不恒定（但人眼通常不敏感）。

---

## 4. 常见应用

### 4.1 LED 调光

- 推荐 150~~500 Hz（避免肉眼闪烁），32~~128 级即可平滑
- 多路时注意总电流，优先“灌电流”接法或用 ULN2003/三极管

### 4.2 直流电机调速

- 频率高于可闻频段（>18~20 kHz）更安静
- 纯软件 PWM 很难到 20 kHz×高分辨率；建议：
  - 用增强型 51 硬件 PWM，或
  - 用带 PWM 的驱动芯片（如 TB6612、L298N 用外部占空波形），或
  - 牺牲分辨率（如 20 kHz × 8 级仍要 160 kHz ISR，依然吃紧）

### 4.3 蜂鸣器/有源蜂鸣器“音调”

- 无源蜂鸣器：直接用定时器翻转引脚得到方波频率 f，非占空比意义上的 PWM
- 有源蜂鸣器：只需拉低/拉高即可响，PWM 可用于“音量”粗调（有源的不太适合）

示例（Timer1 方式2，自动重装，输出方波到 P3.7）：

```c
#include <reg52.h>
sbit BUZZ = P3^7;  // 连接无源蜂鸣器

/* 设定方波频率f（Hz），自动计算TH1/TL1（方式2 8位自动重装）
   Fosc=11.0592MHz → Tmc=1.085us → T1计数为 256*(12/Fosc)每溢出
   半周期计数 N = (Fosc/12) / (2*f*256)，求 TH1=256-N */
void buzzer_set_freq(unsigned int f){
    unsigned int N;
    if(f==0) { TR1=0; BUZZ=0; return; }
    N = (11059200UL/12) / (2UL * f * 256UL); // 近似
    if(N>255) N=255; if(N<1) N=1;
    TH1 = 256 - (unsigned char)N;
    TL1 = TH1;
    TR1 = 1;
}
void t1_isr(void) interrupt 3 { BUZZ = !BUZZ; } // 每溢出翻转一次 → 方波

void buzzer_init(void){
    TMOD = (TMOD & 0x0F) | 0x20; // T1方式2
    ET1 = 1; EA = 1;
}
void delay_ms(unsigned int ms){ unsigned int i,j; for(i=0;i<ms;i++) for(j=0;j<125;j++); }

void main(void){
    buzzer_init();
    // 简单音阶
    const unsigned int notes[] = {262,294,330,349,392,440,494,523};
    unsigned char i;
    while(1){
        for(i=0;i<8;i++){ buzzer_set_freq(notes[i]); delay_ms(300); }
        buzzer_set_freq(0); delay_ms(500);
    }
}
```

### 4.4 舵机（50Hz，1~2ms 脉宽）

- 标准 50Hz（周期 20ms），高电平脉宽 1.0~~2.0ms 对应 0~~180°
- 实现要点：每 20ms 输出一次高脉冲，脉宽按角度计算

简洁实现（独立工程使用 Timer0 微秒级计数，阻塞式演示）：

```c
#include <reg52.h>
sbit SERVO = P1^0;

/* T0方式1，1us tick（近似，需12MHz更易匹配；11.0592MHz误差可接受演示） */
void t0_init_1us(void){  // 每次装载使中断间隔≈1us
    TMOD = (TMOD & 0xF0) | 0x01;
    TR0 = 0;
}
void delay_us(unsigned int us){
    while(us--){
        TH0 = 0xFF; TL0 = 0xF0; // 约16计数 → ~1us（粗略，按Fosc微调）
        TF0 = 0; TR0 = 1;
        while(!TF0);
        TR0 = 0;
    }
}
void delay_ms(unsigned int ms){ while(ms--) delay_us(1000); }

/* 角度→脉宽(us)：1000~2000 */
unsigned int angle_to_us(unsigned char angle){
    return 1000 + (unsigned int)angle * 1000 / 180;
}

void servo_write(unsigned char angle){
    unsigned int pw = angle_to_us(angle);
    // 周期20ms：先高 pw 微秒，再低 (20000-pw) 微秒
    SERVO = 1; delay_us(pw);
    SERVO = 0; delay_us(20000 - pw);
}

void main(void){
    t0_init_1us();
    while(1){
        unsigned char a;
        for(a=0;a<=180;a+=5)  servo_write(a);
        for(a=180;a>0; a-=5)  servo_write(a);
    }
}
```

说明：此法为“阻塞式”，适合简单演示。工程中建议用定时器中断分时调度 20ms 周期与高电平段。

---

## 5. 频率、分辨率与负载建议

- LED：200~~500 Hz，32~~128 级，追求“稳态亮度”即可
- 电机：>10 kHz 避免啸叫；软件 PWM 难以胜任，尽量用硬件 PWM/驱动器
- 加热丝/电磁阀：频率可低（几十~几百 Hz），但注意电磁噪声与平均功耗
- 舵机：固定 50 Hz、脉宽 1~2ms，非“占空比”意义上的普通 PWM

---

## 6. 调试与排错

- 闪烁/抖动明显：频率太低；ISR 间隔抖动；多任务打断太多
- 串音/毛刺：同口多位同时切换、电源去耦不足；增加串联电阻/缓速；驱动能力不足
- CPU 占用高：STEPS 过大、通道太多；改用 Δ-Σ、降低分辨率或用硬件 PWM
- 舵机乱跳：脉宽不稳、周期不准；供电不足（舵机电源须独立稳压与地汇接）

---

## 7. 小结

- 掌握 PWM 三要素与软件实现权衡：fISR​=fPWM​×STEPS
- 给出两种软件实现：固定频率比较型（多路 LED 调光）与 Δ-Σ 误差累加（低中断率）
- 覆盖蜂鸣器方波与舵机脉宽控制的实用范式
- 对于高频/大功率负载，优先选用硬件 PWM 或外部驱动

---
