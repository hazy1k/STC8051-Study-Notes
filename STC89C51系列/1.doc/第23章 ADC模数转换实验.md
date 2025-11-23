# 第二十三章 ADC模数转换实验

## 1. 导入

51内核标准机型没有片上ADC，要采集模拟量（电位器、光敏、电压等），需外接ADC芯片。本章选用常见、接线简单的串行ADC——ADC0832（2路8位，类SPI时序），实现模拟电压采样，并将结果通过串口或数码管显示。

你将掌握：

- ADC基本概念：量程、分辨率、参考电压、采样频率。
- ADC0832的时序与三线接口（CS/CLK/DI/DO）。
- 基于GPIO“位 banging”实现ADC读数。
- 采样值转电压、简单滤波与显示。

提示：若你更偏好I²C器件，可用PCF8591，复用上一章I²C框架实现ADC输入（本章末给出简要参考）。

---

## 2. 基础概念速览

- 分辨率：ADC0832为8位，数字范围0~255。量化步进为 Vref/255。
- 参考电压（Vref）：通常等于芯片VCC（5V），也可外接基准。输出码值约为 Code ≈ Vin / Vref × 255。
- 单端/差分：本章使用单端模式（SGL=1），通道CH0或CH1相对GND测量。

---

## 3. 硬件设计

### 3.1 器件与引脚

- ADC0832 主要引脚：
  - CS：片选（低有效）
  - CLK：串行时钟
  - DI：数据输入（给ADC发送控制位）
  - DO：数据输出（ADC输出转换结果）
  - CH0/CH1：模拟输入通道
  - VCC/GND：电源与地
  - Vref：参考电压（不接时=VCC，很多模块已内部连到VCC）

### 3.2 连接示例（与51最小系统）

- 控制线（可用任意IO，以下以 P1.0~P1.3 为例）：
  - P1.0 → CS
  - P1.1 → CLK
  - P1.2 → DI
  - P1.3 ← DO
- 模拟输入：
  - 电位器中点 → CH0
  - 电位器两端 → VCC 与 GND
- 供电与参考：
  - VCC → +5V，GND → 地；Vref 默认接VCC（多数模块已默认处理）
- 去耦：ADC0832 VCC与GND处放置0.1µF电容。

注意：DO为输入到单片机的引脚；DI为单片机输出给ADC的引脚。

---

## 4. 软件设计（ADC0832驱动）

### 4.1 原理与时序要点

- 以CS拉低开始通信，CLK空闲为低。
- 发送三位控制：Start=1，SGL/DIFF=1（单端），ODD/SIGN=通道选择（CH0=0，CH1=1）。
- 然后再打一个对齐时钟，之后在接下来的8个时钟上，ADC0832在DO上输出8位数据（MSB→LSB）。
- 结束时CS拉高。

### 4.2 完整代码：ADC0832读取 + 串口打印 + 可选数码管显示

```c
#include <reg52.h>
#include <intrins.h>

/* --------- 引脚定义（按需调整端口） --------- */
sbit ADC_CS  = P1^0;
sbit ADC_CLK = P1^1;
sbit ADC_DI  = P1^2;
sbit ADC_DO  = P1^3;

sbit LED = P1^7;   // 指示灯（可选）

/* --------- 公用延时与串口 --------- */
void delay_us_inline() { _nop_(); _nop_(); _nop_(); _nop_(); }
void delay_ms(unsigned int ms){
    unsigned int i, j;
    for(i=0;i<ms;i++) for(j=0;j<125;j++);
}

void uart_init(){
    TMOD |= 0x20;           // T1方式2
    TH1 = 0xFD; TL1 = 0xFD; // 9600bps @11.0592MHz
    TR1 = 1;
    SCON = 0x50;            // 8N1，允许接收
    EA = 1; ES = 0;         // 此处不使用串口中断
}
void uart_send_byte(unsigned char ch){ SBUF = ch; while(!TI); TI=0; }
void uart_send_str(const char* s){ while(*s) uart_send_byte(*s++); }
void uart_send_uint(unsigned int v){
    char buf[6]; char i=0;
    if(v==0){ uart_send_byte('0'); return; }
    while(v){ buf[i++] = (v%10)+'0'; v/=10; }
    while(i--) uart_send_byte(buf[i]);
}

/* --------- ADC0832 基础IO时序 --------- */
static void adc_clk_pulse(){
    ADC_CLK = 1; delay_us_inline();
    ADC_CLK = 0; delay_us_inline();
}

/* 发送一位到DI（在CLK上升沿采样） */
static void adc_send_bit(bit b){
    ADC_DI = b ? 1 : 0; delay_us_inline();
    ADC_CLK = 1; delay_us_inline();
    ADC_CLK = 0; delay_us_inline();
}

/* 读取一位（MSB->LSB），在CLK上升沿后读DO */
static unsigned char adc_read_bit(){
    unsigned char bitv;
    ADC_CLK = 1; delay_us_inline();
    bitv = ADC_DO ? 1 : 0;
    ADC_CLK = 0; delay_us_inline();
    return bitv;
}

/* 读取指定通道（0或1），返回0~255 */
unsigned char adc0832_read(unsigned char ch){
    unsigned char i, val = 0;

    /* 开始通信 */
    ADC_CS  = 1; ADC_CLK = 0; ADC_DI = 1;  // 默认空闲
    ADC_CS  = 0; delay_us_inline();

    /* 控制三位：Start=1, SGL=1, ODD=ch */
    adc_send_bit(1);     // Start
    adc_send_bit(1);     // SGL=1 单端
    adc_send_bit(ch?1:0);// ODD/SIGN=通道选择

    /* 对齐时钟（丢弃一个空位） */
    adc_clk_pulse();

    /* 读取8位数据（MSB->LSB） */
    for(i=0;i<8;i++){
        val <<= 1;
        val |= adc_read_bit();
    }

    /* 结束通信 */
    ADC_CS = 1;
    return val;
}

/* 简单多次平均（抗抖动） */
unsigned char adc0832_read_avg(unsigned char ch, unsigned char n){
    unsigned int acc = 0;
    unsigned char i;
    if(n==0) n=1;
    for(i=0;i<n;i++){
        acc += adc0832_read(ch);
        delay_ms(2);
    }
    return (unsigned char)(acc / n);
}

/* --------- 可选：共阴数码管显示0~255（三位） --------- */
/* 段码（0~9） */
unsigned char code seg_code[10] = {
  0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F
};
/* 显示一个三位数，P0为段选，P2.0~P2.2为位选低有效 */
void seg_show_3d(unsigned char val){
    unsigned char b = val % 10;
    unsigned char t = (val / 10) % 10;
    unsigned char h = (val / 100) % 10;

    // 百位
    P0 = (h==0)?0x00:seg_code[h];   // 百位为0时可熄灭，避免前导0
    P2 = (P2 & 0xF8) | 0x06;        // 0b110 选择位0（示例）
    delay_ms(1);

    // 十位
    P0 = (h==0 && t==0)?0x00:seg_code[t];
    P2 = (P2 & 0xF8) | 0x05;        // 选择位1
    delay_ms(1);

    // 个位
    P0 = seg_code[b];
    P2 = (P2 & 0xF8) | 0x03;        // 选择位2
    delay_ms(1);
}

/* --------- 主程序：读取并显示 --------- */
void main(){
    unsigned char raw;
    unsigned int mV;

    uart_init();
    uart_send_str("ADC0832 Demo Start\r\n");

    ADC_CS = 1; ADC_CLK = 0; ADC_DI = 1;

    while(1){
        /* 读取CH0，取8次平均 */
        raw = adc0832_read_avg(0, 8);

        /* 转电压：5V系统 → mV = raw * 5000 / 255 */
        mV = (unsigned int)raw * 5000 / 255;

        /* 串口输出 */
        uart_send_str("RAW=");
        uart_send_uint(raw);
        uart_send_str("  U=");
        uart_send_uint(mV);
        uart_send_str(" mV\r\n");

        /* 可选：三位数码管显示原始值0~255 */
        // 快速扫描多次以提升稳定性
        {
            unsigned char k;
            for(k=0;k<60;k++) seg_show_3d(raw);
        }

        /* 指示灯闪烁 */
        LED = !LED;
    }
}
```

要点说明：

- 代码按“相邻时钟上升沿读DO”的方式取数，确保时序满足；如遇个别模块对时序更敏感，可在`delay_us_inline()`中增加若干`_nop_()`。
- 若电位器全行程对应0~~5V，原始值一般覆盖0~~255；若Vref≠5V（或模块内接不同参考），换算电压时将5000替换为实际mV。
- 简单平均能抑制抖动；更稳定可用滑动平均/中位数滤波。

---

## 5. 测试与应用

- 接好电位器，转动观察串口的RAW与电压变化，数码管显示0~255动态变化。
- 应用拓展：
  - 阈值开关：raw > 200 打开风扇/蜂鸣器。
  - 映射PWM：raw映射为占空比，实现旋钮控亮度/转速。
  - 双通道：将传感器接CH1，调用`adc0832_read_avg(1, n)`。

---

## 6. 误差与优化

- 参考误差：Vref漂移直接影响读数，建议稳定供电或用基准源。
- 地线与噪声：模拟地尽量干净，采样线短且靠近地回路。
- 采样阻抗：信号源阻抗过高会导致充采时间不足，必要时在输入端并小电容（与源阻抗构成RC）或用缓冲运放。
- 标定：用已知电压点做两点或多点标定，修正线性误差。

---

## 7. 可选方案：PCF8591（I²C ADC）简述

若你已完成I²C章节，可用PCF8591替代ADC0832：

- 硬件：SDA/SCL上拉4.7kΩ；A0~~A2设地址；AIN0~~AIN3为四路单端输入。
- 关键流程：写控制字（选择通道与模式）→ 读一个“前导无效字节”→ 再读有效转换数据。
- 读单通道伪码：
  - I²C Start → 写设备地址(写) → 写控制字(0x40 | 通道) → ReStart → 写设备地址(读)
  - 读丢弃字节 → 读有效字节并NACK → Stop

这样可与AT24C02共用I²C总线，方便多外设并联。

---

## 8. 小结

- 硬件：ADC0832三线接口简单、IO占用少；电位器作为可变电压源测试直观。
- 软件：位banging发送Start/SGL/通道选择，读8位数据；加入平均滤波与显示。
- 能力：完成从模拟量获取→数字化→显示/控制的完整链路，可扩展到多传感器与闭环控制。

常见问题与排查

- 读数固定0或255：CS/CLK/DI/DO接错或接反；DO未设为输入；Vref/AGND异常。
- 抖动大：加平均滤波、检查电源与地、缩短模拟线。
- 电压不准：Vref非5V但仍按5V换算；供电不稳；缺少标定。
- 数码管串扰：加入“消隐”与位选关闭步骤，或放入定时器中断扫描。

---
