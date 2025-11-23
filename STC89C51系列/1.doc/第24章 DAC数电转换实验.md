# 第二十四章 DAC数电转换实验

## 1. 导入

ADC把模拟量转为数字量；DAC（Digital-to-Analog Converter）则将数字量恢复为模拟电压/电流，用于音频、波形发生、偏置控制等。本章给出两种常用实现路径：

- 外接串行DAC（主讲 MCP4921，12位，SPI 时序）
- 软硬件简易替代：PWM+RC 低通滤波“伪DAC”（作为备选）

你将掌握：

- DAC量程、分辨率、参考电压与输出关系
- MCP4921的SPI时序、配置字与代码实现
- 直流电压输出、锯齿波/正弦波输出
- PWM+RC 方案的设计要点与实现

---

## 2. 基础概念

- 分辨率：MCP4921 为 12 位，数字码 0~4095。步进电压 ΔV = Vref/4096。
- 参考电压 Vref：输出近似 Vout = (D/4096) × Vref（GA=1x 时）。
- 采样速率/更新率：波形质量与更新速率相关；正弦波建议更新频率 ≥ 40×目标频率。

---

## 3. 方案一：外接串行 DAC（MCP4921）

### 3.1 硬件连接

- 器件：MCP4921（12位、SPI兼容）、0.1µF 去耦
- 单片机 IO（示例使用 P1.0~P1.2）：
  - `P1.0 → SDI`（数据输入）
  - `P1.1 → SCK`（时钟，空闲低，上升沿采样）
  - `P1.2 → CS`（片选，低有效）
  - `LDAC` 接地（低电平时 CS 上升沿即可更新输出）
  - `SHDN` 上拉到 VDD（使能）
- 电源与参考：
  - `VDD=+5V`，`GND=0V`
  - `VREF` 建议接 4.096V 基准或 5V（有源基准更稳）
- 输出：
  - `VOUT` → 示波器/万用表/后级缓冲（负载阻抗尽量高）

注意：总线较短、地线良好、近芯片放置 0.1µF 去耦。

### 3.2 数据格式（16位命令字，MSB先）

- [15] 通道选择（MCP4921固定A通道，置0）
- [14] BUF：1=缓冲参考（推荐）
- [13] GA：1=1×增益（Vout= D/4096×Vref），0=2×
- [12] SHDN：1=工作，0=关断
- [11:0] D11..D0：12位数据

推荐配置：bit15..12 = 0b0111（BUF=1, GA=1, SHDN=1） → 命令高 4 位为 0x7

### 3.3 完整示例（C51，位触发SPI，锯齿/正弦）

```c
#include <reg52.h>
#include <intrins.h>

/* ---------- 引脚定义（按需修改端口） ---------- */
sbit DAC_SDI = P1^0;   // MCP4921 SDI
sbit DAC_SCK = P1^1;   // MCP4921 SCK
sbit DAC_CS  = P1^2;   // MCP4921 CS (低有效)

/* ---------- 公共延时 ---------- */
static void tiny_delay(void){ _nop_(); _nop_(); _nop_(); _nop_(); }

/* ---------- SPI 发送16位（MSB先） ---------- */
static void mcp4921_write16(unsigned int dat16)
{
    unsigned char i;
    DAC_CS = 0;              // 选中
    for (i = 0; i < 16; i++) {
        // 发送最高位
        DAC_SDI = (dat16 & 0x8000) ? 1 : 0;
        tiny_delay();
        DAC_SCK = 1;         // 上升沿采样
        tiny_delay();
        DAC_SCK = 0;
        dat16 <<= 1;
    }
    DAC_CS = 1;              // 拉高CS，若LDAC为低则更新输出
}

/* ---------- 写入12位码值（0~4095） ---------- */
void dac_out_12b(unsigned int code12)
{
    if (code12 > 4095) code12 = 4095;
    // 高4位 0x7xxx： 0b0111 -> BUF=1, GA=1x, SHDN=1
    mcp4921_write16( (0x7 << 12) | (code12 & 0x0FFF) );
}

/* ---------- 简单延时ms ---------- */
void delay_ms(unsigned int ms){
    unsigned int i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<125;j++);
}

/* ---------- 示例1：直流扫阶 & 锯齿波 ---------- */
void demo_sawtooth(void)
{
    unsigned int v;
    // 直流阶梯：每步 ~50ms，0→满量程
    for (v = 0; v <= 4095; v += 128) {
        dac_out_12b(v);
        delay_ms(50);
    }

    // 连续锯齿波（快速循环）
    while (1) {
        for (v = 0; v <= 4095; v += 8) { // 步进越小，波形越细腻
            dac_out_12b(v);
            // 控制更新速率（决定输出频率），按需微调
            tiny_delay(); tiny_delay(); tiny_delay(); tiny_delay();
        }
    }
}

/* ---------- 示例2：正弦波查表 ---------- */
// 256点正弦表（0~4095），幅度≈(4095/2)，直流偏置≈(4095/2)
unsigned int code sine_lut[256] = {
    // 可用脚本生成，下面仅示例前16项，实际请填满256点
    2048,2098,2149,2199,2249,2299,2349,2398,
    2447,2496,2544,2591,2638,2684,2729,2774,
    // ……（请补齐256项）……
};

void demo_sine(void)
{
    unsigned char i = 0;
    while (1) {
        dac_out_12b(sine_lut[i++]);
        // 控制输出频率：Fout ≈ Fs/256，其中 Fs 为本循环更新频率
        // 例如 i++ 每次后 tiny_delay 若 ~2us，则 Fs≈500kSPS，Fout≈~1953Hz
        tiny_delay(); tiny_delay();
    }
}

/* ---------- 主程序 ---------- */
void main(void)
{
    // 缺省电平
    DAC_CS = 1; DAC_SCK = 0; DAC_SDI = 0;

    // 任选其一：锯齿/正弦
    // demo_sawtooth();
    demo_sine();
}
```

要点：

- 输出电压计算（GA=1x）：Vout ≈ (code/4096) × Vref
- 若使用 5.000V 作为 Vref，code=2048 时约 2.5V。
- 正弦表可用脚本离线生成（0.5×FS 振幅 + 0.5×FS 直流偏置），避免实时浮点。

---

## 4. 方案二：PWM + RC 低通“伪 DAC”（备选）

当无专用 DAC 芯片时，可用较高频率 PWM，经 RC 低通滤波得到近似直流电压/慢变波形。

### 4.1 硬件

- 单片机 PWM 引脚 → RC 一阶低通 → 测试点
- 典型取值：R=10kΩ、C=0.1µF（fc≈1/(2πRC)≈159 Hz）
- PWM 频率需远高于 fc（建议 5~20 kHz）

### 4.2 软件（定时器产生 PWM，占空比映射输出电压）

```c
#include <reg52.h>

/* 以 T0 方式2 或方式1 产生 ~10kHz PWM 的思路：
   简化处理：软件PWM（中断中计数） */
sbit PWM_PIN = P3^7;

volatile unsigned char duty = 128; // 0~255 -> 0~100%占空比
volatile unsigned char cnt  = 0;

void timer0_init_10kHz(void){
    // 11.0592MHz，方式2（8位自动重装）计算略，可按实际微调
    TMOD = (TMOD & 0xF0) | 0x02;
    TH0  = 0x9C;  // 约10kHz
    TL0  = 0x9C;
    ET0  = 1; EA = 1; TR0 = 1;
}

void t0_isr(void) interrupt 1 {
    cnt++;
    if (cnt < duty) PWM_PIN = 1; else PWM_PIN = 0;
    // 周期结束
    if (cnt == 255) cnt = 0;
}

void main(){
    PWM_PIN = 0;
    timer0_init_10kHz();

    while(1){
        // 扫占空比：0→100%
        unsigned char d;
        for(d=0; d<255; d+=8){
            duty = d;
            // 经过RC后，输出近似 Vout ≈ Vcc * duty/255
            // 上升/下降受时间常数τ=RC影响，变化较缓
            // 观察万用表/示波器输出即可
            // 简单停顿
            {
                unsigned int i,j;
                for(i=0;i<200;i++) for(j=0;j<125;j++);
            }
        }
    }
}
```

要点：

- 低频慢变电压可行；若要音频/高保真波形，需更高 PWM 频率与更高阶滤波。
- PWM+RC 的输出阻抗较高、纹波受负载影响较大，工程上常用专用 DAC 替代。

---

## 5. 测试与应用

- 直流输出：依次输出 0%、50%、100%（或 0、2048、4095），用万用表测电压是否接近理论值。
- 波形输出：锯齿/正弦，示波器观察波形形状与幅值；增大查表点数/提高更新率可改善失真。
- 驱动/缓冲：若需带载，建议在 VOUT 后加运放缓冲（电压跟随器），减小源阻抗与负载效应。

---

## 6. 误差与优化

- 参考误差：Vref 稳定度直接影响绝对精度，建议用高精度基准源。
- 线性度与微分非线性（INL/DNL）：取决于 DAC 芯片，选择与应用需求相匹配的器件。
- 更新抖动：中断/轮询抖动会引入相位噪声；可用定时器中断固定节拍更新。
- 滤波：对正弦/音频应用，使用FIR/IIR数字生成+模拟低通配合，效果更好。
- 噪声与地：模拟与数字地分区、单点汇接，减少地弹与串扰。

---

## 7. 小结

- 掌握了两种DAC实现：专用串行DAC（MCP4921，主推）与 PWM+RC 替代方案。
- 给出完整代码：SPI位操作、12位码输出、锯齿与正弦查表。
- 了解输出电压与参考的关系、更新率与波形质量的权衡。
- 为后续“任意波形发生器”“音频播放”“偏置控制”等应用奠定基础。

常见问题与排查：

- 无输出/固定电平：检查 CS/SCK/SDI 连接与时序（上升沿采样）、LDAC/SHDN 管脚。
- 电压不准：Vref 实际值与计算不符；负载过重；未使用缓冲。
- 波形畸变：更新率低、查表点稀疏；长导线与地回路引入噪声。
- PWM方案纹波大：提高 PWM 频率、优化 RC 值、增加阶数或改用专用 DAC。

---
