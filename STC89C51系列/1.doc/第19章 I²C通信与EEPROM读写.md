# 第十九章 I²C通信与EEPROM读写

## 1. 导入

在前面章节中，我们学习了UART和RS485等串行通信方式，主要用于设备间数据传输。本章将介绍另一种重要的**同步串行通信协议**——**I²C**（Inter-Integrated Circuit），并以**AT24C02 EEPROM**为例，实现数据的**非易失性存储与读取**。

I²C由Philips（现NXP）开发，仅需**两根线**（SDA数据线、SCL时钟线）即可实现多主多从通信，广泛应用于传感器、实时时钟（RTC）、存储芯片等外设连接。

本章目标：

- 理解I²C总线的工作原理与信号时序；
- 掌握软件模拟I²C时序的方法（51单片机无硬件I²C）；
- 实现对AT24C02 EEPROM的字节写入与读取；
- 学习地址寻址与页写入机制；
- 为后续连接OLED、RTC、传感器等I²C设备打下基础。

---

## 2. 硬件设计

### 2.1 I²C总线结构

- **SDA**：串行数据线，双向，开漏输出；
- **SCL**：串行时钟线，由主机驱动；
- 所有设备并联在SDA和SCL上；
- 必须外接**上拉电阻**（通常4.7kΩ）到VCC。

### 2.2 AT24C02 EEPROM简介

- 容量：256字节（2K bit）；
- 页大小：8字节（页写入限制）；
- 通信协议：I²C；
- 设备地址：`1010 + A2 + A1 + A0 + R/W`，其中A2/A1/A0由硬件引脚决定；
- 典型地址：若A2=A1=A0=0，则写地址为 `0xA0`，读地址为 `0xA1`。

> 注意：AT24C02地址高4位固定为1010，低3位由硬件决定，最后1位为读写位。

---

### 2.3 电路连接

| 单片机        | AT24C02 | 说明          |
| ---------- | ------- | ----------- |
| P2.0       | SDA     | 数据线（双向）     |
| P2.1       | SCL     | 时钟线         |
| VCC        | VCC     | +5V         |
| GND        | GND     | 公共地         |
| A0, A1, A2 | GND     | 设置硬件地址为0    |
| WP         | GND     | 写保护关闭（允许写入） |

> P2.0（SDA）和P2.1（SCL）需接4.7kΩ上拉电阻至VCC。

---

## 3. 软件设计

### 3.1 I²C基本信号时序

#### 1. 起始信号（Start）

- SCL高电平时，SDA由高→低

#### 2. 停止信号（Stop）

- SCL高电平时，SDA由低→高

#### 3. 应答信号（ACK）

- 每传输1字节后，接收方在第9个时钟周期拉低SDA表示应答

---

### 3.2 模拟I²C基本函数

```c
#include <reg52.h>

sbit SDA = P2^0;
sbit SCL = P2^1;

void delay_us() {
    _nop_(); _nop_();
}

// 起始信号
void i2c_start() {
    SDA = 1; SCL = 1;
    delay_us();
    SDA = 0;
    delay_us();
    SCL = 0;
}

// 停止信号
void i2c_stop() {
    SDA = 0; SCL = 1;
    delay_us();
    SDA = 1;
    delay_us();
}

// 发送一个字节
void i2c_send_byte(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        SCL = 0;
        SDA = (dat & 0x80) ? 1 : 0;
        dat <<= 1;
        delay_us();
        SCL = 1;
        delay_us();
    }
    SCL = 0;
}

// 接收一个字节（主机接收）
unsigned char i2c_read_byte() {
    unsigned char i, dat = 0;
    SDA = 1;  // 释放SDA，准备接收
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        SCL = 1;
        delay_us();
        if (SDA) dat |= 0x01;
        SCL = 0;
        delay_us();
    }
    return dat;
}

// 发送应答（ACK=0，NACK=1）
void i2c_send_ack(unsigned char ack) {
    SCL = 0;
    SDA = ack;
    delay_us();
    SCL = 1;
    delay_us();
    SCL = 0;
}
```

---

### 3.3 AT24C02写操作（字节写入）

```c
// 向指定地址写入一个字节
void eeprom_write_byte(unsigned char addr, unsigned char dat) {
    i2c_start();
    i2c_send_byte(0xA0);      // 设备写地址
    i2c_send_ack(0);
    i2c_send_byte(addr);      // 写地址
    i2c_send_ack(0);
    i2c_send_byte(dat);       // 写数据
    i2c_send_ack(0);
    i2c_stop();

    delay_us();               // 写入周期（约10ms）
    delay_ms(10);
}
```

---

### 3.4 AT24C02读操作（当前地址读 / 随机读）

```c
// 从指定地址读取一个字节
unsigned char eeprom_read_byte(unsigned char addr) {
    unsigned char dat;

    // 第一步：发送设备地址和读地址
    i2c_start();
    i2c_send_byte(0xA0);
    i2c_send_ack(0);
    i2c_send_byte(addr);
    i2c_send_ack(0);
    i2c_stop();

    delay_ms(1);

    // 第二步：读取数据
    i2c_start();
    i2c_send_byte(0xA1);      // 设备读地址
    i2c_send_ack(0);
    dat = i2c_read_byte();
    i2c_send_ack(1);          // 主机发送NACK，结束读取
    i2c_stop();

    return dat;
}
```

---

### 3.5 主程序示例：写入并读回数据

```c
void main() {
    unsigned char data;

    // 写入数据
    eeprom_write_byte(0x00, 0x55);
    eeprom_write_byte(0x01, 0xAA);

    // 读取数据
    data = eeprom_read_byte(0x00);
    // 可通过串口发送data验证
    data = eeprom_read_byte(0x01);

    while(1);
}
```

---

### 3.6 页写入（批量写入，不超过8字节）

```c
void eeprom_page_write(unsigned char addr, unsigned char *data, unsigned char len) {
    unsigned char i;
    i2c_start();
    i2c_send_byte(0xA0);
    i2c_send_ack(0);
    i2c_send_byte(addr);
    i2c_send_ack(0);
    for (i = 0; i < len; i++) {
        i2c_send_byte(data[i]);
        i2c_send_ack(0);
    }
    i2c_stop();
    delay_ms(10);
}
```

> 注意：AT24C02每页8字节，跨页写入会回卷。

---

### 3.7 连续读取（顺序读）

```c
void eeprom_read_page(unsigned char addr, unsigned char *buf, unsigned char len) {
    unsigned char i;
    i2c_start();
    i2c_send_byte(0xA0);
    i2c_send_ack(0);
    i2c_send_byte(addr);
    i2c_send_ack(0);
    i2c_start();  // 重复起始
    i2c_send_byte(0xA1);
    i2c_send_ack(0);
    for (i = 0; i < len; i++) {
        buf[i] = i2c_read_byte();
        if (i < len - 1)
            i2c_send_ack(0);  // 中间字节发ACK
        else
            i2c_send_ack(1);  // 最后字节发NACK
    }
    i2c_stop();
}
```

---

### 3.8 编译与下载

- Keil中创建工程；
- 确保SDA、SCL接4.7kΩ上拉电阻；
- 编译生成HEX；
- 下载至单片机；
- 通过串口或LED验证读写结果。

> 若通信失败：
> 
> - 检查上拉电阻；
> - 确认A0~A2接地；
> - 使用逻辑分析仪抓取I²C信号。

---

##4. 小结

本章通过实现I²C通信与EEPROM读写，掌握了同步串行总线与非易失存储技术，主要内容包括：

- **硬件连接**：学会使用上拉电阻构建I²C总线；
- **软件模拟**：掌握起始、停止、应答等时序控制；
- **EEPROM操作**：实现字节写入、随机读、页写、连续读；
- **数据持久化**：断电后数据不丢失，可用于保存配置；
- **扩展能力**：为连接OLED、DS1307、MPU6050等I²C设备奠定基础。

### 4.1 常见问题与解决

| 问题      | 原因              | 解决方法        |
| ------- | --------------- | ----------- |
| 无应答     | 地址错误或SDA/SCL未上拉 | 检查设备地址和上拉电阻 |
| 写入失败    | 未等待写入周期完成       | 每次写后延时10ms  |
| 读取为0xFF | 未先写地址           | 随机读需先发送地址   |
| 信号异常    | 引脚驱动能力弱         | 使用强上拉或驱动芯片  |

### 4.2 下一步学习建议

- 学习使用I²C驱动 **OLED显示屏**；
- 连接 **DS1307实时时钟** 实现时间存储；
- 构建 **多I²C设备系统**；
- 实现 **配置记忆功能**（如保存亮度、时间等）。

本章标志着你已掌握**同步串行通信与数据存储能力**，具备了开发复杂嵌入式系统的关键技能。

---
