# 第十九章 I2C-EEPROM实验

## 1. 导入

    这一章我们来学习如何使用 51 单片机的 IO 口模拟 I2C 时序， 并实现与AT24C02（ EEPROM） 之间的双向通信。 开发板板载了 1 个 EEPROM 模块， 可实现IIC 通信。 

    本章要实现的功能是： 系统运行时， 数码管右 3 位显示 0， 按 K1 键将数据写入到 EEPROM 内保存， 按 K2 键读取 EEPROM 内保存的数据， 按 K3 键显示数据加 1， 按 K4 键显示数据清零， 最大能写入的数据是 255。

## 2. I2C介绍

    I2C（ Inter－ Integrated Circuit） 总线是由 PHILIPS 公司开发的两线式串行总线， 用于连接微控制器及其外围设备。 是微电子通信控制领域广泛采用的一种总线标准。 它是同步通信的一种特殊形式， 具有接口线少， 控制方式简单，器件封装形式小， 通信速率较高等优点。 I2C 总线只有两根双向信号线。 一根是数据线 SDA， 另一根是时钟线 SCL。 由于其管脚少， 硬件实现简单， 可扩展性强等特点， 因此被广泛的使用在各大集成芯片内。

    目前阶段我们不需要了解更多。可以参考：

[I2C总线协议详解（特点、通信过程、典型I2C时序）-CSDN博客](https://blog.csdn.net/zhangduang_KHKW/article/details/121953275#:~:text=I2C%E6%80%BB%E7%BA%BF%E6%98%AFPhilips%E5%85%AC%E5%8F%B8%E5%9C%A8%E5%85%AB%E5%8D%81%E5%B9%B4%E4%BB%A3%E5%88%9D%E6%8E%A8%E5%87%BA%E7%9A%84%E4%B8%80%E7%A7%8D%20%E4%B8%B2%E8%A1%8C%20%E3%80%81%20%E5%8D%8A%E5%8F%8C%E5%B7%A5%20%E7%9A%84%E6%80%BB%E7%BA%BF%EF%BC%8C%E4%B8%BB%E8%A6%81%E7%94%A8%E4%BA%8E,%E8%BF%91%E8%B7%9D%E7%A6%BB%E3%80%81%E4%BD%8E%E9%80%9F%20%E7%9A%84%E8%8A%AF%E7%89%87%E4%B9%8B%E9%97%B4%E7%9A%84%E9%80%9A%E4%BF%A1%EF%BC%9BI2C%E6%80%BB%E7%BA%BF%E6%9C%89%E4%B8%A4%E6%A0%B9%E5%8F%8C%E5%90%91%E7%9A%84%E4%BF%A1%E5%8F%B7%E7%BA%BF%EF%BC%8C%E4%B8%80%E6%A0%B9%E6%95%B0%E6%8D%AE%E7%BA%BF%20SDA%20%E7%94%A8%E4%BA%8E%E6%94%B6%E5%8F%91%E6%95%B0%E6%8D%AE%EF%BC%8C%E4%B8%80%E6%A0%B9%E6%97%B6%E9%92%9F%E7%BA%BF%20SCL%20%E7%94%A8%E4%BA%8E%E9%80%9A%E4%BF%A1%E5%8F%8C%E6%96%B9%E6%97%B6%E9%92%9F%E7%9A%84%E5%90%8C%E6%AD%A5%EF%BC%9BI2C%E6%80%BB%E7%BA%BF%E7%A1%AC%E4%BB%B6%E7%BB%93%E6%9E%84%E7%AE%80%E5%8D%95%EF%BC%8C%E7%AE%80%E5%8C%96%E4%BA%86PCB%E5%B8%83%E7%BA%BF%EF%BC%8C%E9%99%8D%E4%BD%8E%E4%BA%86%E7%B3%BB%E7%BB%9F%E6%88%90%E6%9C%AC%EF%BC%8C%E6%8F%90%E9%AB%98%E4%BA%86%E7%B3%BB%E7%BB%9F%E5%8F%AF%E9%9D%A0%E6%80%A7%EF%BC%8C%E5%9B%A0%E6%AD%A4%E5%9C%A8%E5%90%84%E4%B8%AA%E9%A2%86%E5%9F%9F%E5%BE%97%E5%88%B0%E4%BA%86%E5%B9%BF%E6%B3%9B%E5%BA%94%E7%94%A8%E3%80%82)

[【51单片机快速入门指南】4： 软件 I2C-电子工程世界 (eeworld.com.cn)](https://news.eeworld.com.cn/mcu/ic616567.html#:~:text=I2C%20%E4%BB%8B%E7%BB%8D%20I2C%EF%BC%88Inter%EF%BC%8DIntegrated%20Circuit%EF%BC%89%20%E6%80%BB%E7%BA%BF%20%E6%98%AF%E7%94%B1%20PHILIPS%20%E5%85%AC%E5%8F%B8%E5%BC%80%E5%8F%91%E7%9A%84%E4%B8%A4%E7%BA%BF%E5%BC%8F%E4%B8%B2%E8%A1%8C,%EF%BC%8C%E7%94%A8%E4%BA%8E%E8%BF%9E%E6%8E%A5%E5%BE%AE%20%E6%8E%A7%E5%88%B6%E5%99%A8%20%E5%8F%8A%E5%85%B6%E5%A4%96%E5%9B%B4%E8%AE%BE%E5%A4%87%E3%80%82%20%E6%98%AF%E5%BE%AE%E7%94%B5%E5%AD%90%E9%80%9A%E4%BF%A1%E6%8E%A7%E5%88%B6%E9%A2%86%E5%9F%9F%E5%B9%BF%E6%B3%9B%E9%87%87%E7%94%A8%E7%9A%84%E4%B8%80%E7%A7%8D%20%E6%80%BB%E7%BA%BF%20%E6%A0%87%E5%87%86%E3%80%82%20%E5%AE%83%E6%98%AF%E5%90%8C%E6%AD%A5%E9%80%9A%E4%BF%A1%E7%9A%84%E4%B8%80%E7%A7%8D%E7%89%B9%E6%AE%8A%E5%BD%A2%E5%BC%8F%EF%BC%8C%E5%85%B7%E6%9C%89%E6%8E%A5%E5%8F%A3%E7%BA%BF%E5%B0%91%EF%BC%8C%E6%8E%A7%E5%88%B6%E6%96%B9%E5%BC%8F%E7%AE%80%E5%8D%95%EF%BC%8C%20%E5%99%A8%E4%BB%B6%E5%B0%81%E8%A3%85%E5%BD%A2%E5%BC%8F%E5%B0%8F%EF%BC%8C%E9%80%9A%E4%BF%A1%E9%80%9F%E7%8E%87%E8%BE%83%E9%AB%98%E7%AD%89%E4%BC%98%E7%82%B9%E3%80%82)

[51单片机---I2C通信协议（含源码，小白可入）_ii2c写保护-CSDN博客](https://blog.csdn.net/m0_56399733/article/details/134191210)

## 3. AT24C02介绍

    AT24C01/02/04/08/16...是一个 1K/2K/4K/8K/16K 位串行 CMOS， 内部含有128/256/512/1024/2048 个 8 位字节， AT24C01 有一个 8 字节页写缓冲器， AT24C02/04/08/16 有一个 16 字节页写缓冲器。 该器件通过 I2C 总线接口进行操作， 它有一个专门的写保护功能。 我们开发板上使用的是 AT24C02（ EEPROM）芯片， 此芯片具有 I2C 通信接口， 芯片内保存的数据在掉电情况下都不丢失，所以通常用于存放一些比较重要的数据等。     AT24C02 芯片管脚及外观图如下图所示：

![屏幕截图 2024 06 12 090506](https://img.picgo.net/2024/06/12/-2024-06-12-090506f99e364d790d74ce.png)

    AT24C02 器件地址为 7 位， 高 4 位固定为 1010， 低 3 位由 A0/A1/A2 信号线的电平决定。 因为传输地址或数据是以字节为单位传送的， 当传送地址时，器件地址占 7 位， 还有最后一位（ 最低位 R/W） 用来选择读写方向， 它与地址无关。 其格式如下：

![屏幕截图 2024 06 12 090824](https://img.picgo.net/2024/06/12/-2024-06-12-090824067fafd8384ab3da.png)

    我们开发板已经将芯片的 A0/A1/A2 连接到 GND， 所以器件地址为1010000， 即 0x50（ 未计算最低位） 。 如果要对芯片进行写操作时， R/W 即为 0，写器件地址即为 0XA0； 如果要对芯片进行读操作时， R/W 即为 1， 此时读器件地址为 0XA1。 开发板上也将 WP 引脚直接接在 GND 上， 此时芯片允许数据正常读写。

    I2C 总线时序如下图所示：

![屏幕截图 2024 06 12 090931](https://img.picgo.net/2024/06/12/-2024-06-12-0909314bd521e17723fcfe.png)

    更多可以参考文件夹下面的芯片手册或者：[嵌入式——EEPROM（AT24C02）_at24c02时序-CSDN博客](https://blog.csdn.net/CXDNW/article/details/136143812?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-136143812-blog-103551569.235^v43^pc_blog_bottom_relevance_base7&spm=1001.2101.3001.4242.1&utm_relevant_index=1)

## 4. 硬件设计

本实验使用到硬件资源如下：

- 独立按键（ K1-K4）

- 动态数码管

- EEPROM 模块电路（ AT24C02）

独立按键和动态数码管电路在前面章节都介绍过， 这里就不再重复。 下面我们来看下开发板上 EEPROM 模块电路， 如下图所示：

![屏幕截图 2024 06 12 091143](https://img.picgo.net/2024/06/12/-2024-06-12-091143dd315275b5172b39.png)

    从上图中可以看出， 该电路是独立的， 芯片的 SCL 和 SDA 管脚接至 J4 端子上， 在介绍 IIC 总线的时候我们说过， 为了让 IIC 总线默认为高电平， 通常会在IIC 总线上接上拉电阻， 在图中可以看到 SCL 和 SDA 管脚有上拉电阻。

    由于该模块电路是独立的， 所以 24C02 芯片的 SCL 和 SDA 管脚可以使用任意单片机管脚连接， 为了与我们例程程序配套， 这里使用单片机的 P2.0 管脚连接芯片的 SDA 脚， 使用单片机的 P2.1 脚连接芯片的 SCL 脚。

## 4. 模块化工程介绍

    为了后期方便管理和维护，我们需要模块化编程，但是前面为什么一直没有提到-因为代码量小，只需要一个main.c就可以搞定，但是在实际工作中，工程结构和代码量肯定不会这么简单，所以我们需要分类也就是模块化。

    在电脑上创建一个实验文件夹， 为了与教程配套， 这里命名为“ I2C-EEPROM实验” ， 然后在该文件夹内新建 App、 Obj、 Public、 User 四个文件夹， 如下所示：

![屏幕截图 2024 06 12 091522](https://img.picgo.net/2024/06/12/-2024-06-12-091522d165f9370e854096.png)

- App文件夹：用于存放外设驱动文件，如LED、数码管、定时器等

- Obj文件夹：用于存放编译产生的 c/汇编/链接的列表清单、 调试信息、 hex文件、 预览信息、 封装库等文件。

- Public：用来存放51单片机公共的文件，如延时函数、51头文件、变量类型重定义等

- User：用于存放用户主函数文件，如main.c

[51单片机模块化编程_单片机分层设计,一个模块一个文件么-CSDN博客](https://blog.csdn.net/qq_42786221/article/details/123189883)

## 5. 软件设计

    从这一章起，我们的代码量要增多了，难度倒是没有增加多少，因为就是把以前的实验进行整合即把我们使用过的模块同时使用，以实现更强大的功能

    本章所要实现的功能是： 系统运行时， 数码管右 3 位显示 0， 按 K1 键将数据写入到 EEPROM 内保存， 按 K2 键读取 EEPROM 内保存的数据， 按 K3 键显示数据加1，按 K4 键显示数据清零， 最大能写入的数据是 255。程序框架如下：

- 编写按键检测功能（我们已经写过）

- 编写数码管显示功能（我们已经写过）

- 编写 IIC 驱动， 包括起始、 停止、 应答信号等

- 编写 AT24C02 读写功能

- 编写主函数

### 5.1 按键检测函数

```c
#ifndef _key_H
#define _key_H

#include "public.h"

//定义独立按键控制脚
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;


//使用宏定义独立按键按下的键值
#define KEY1_PRESS    1
#define KEY2_PRESS    2
#define KEY3_PRESS    3
#define KEY4_PRESS    4
#define KEY_UNPRESS    0

unsigned char key_scan(unsigned char mode); // 声明外部函数

#endif
```

```c
#include "key.h"

// 按键扫描函数
unsigned char key_scan(unsigned char mode)
{
    static unsigned key = 1;

    if(mode)key = 1; // 连续扫描按键
    if(key == 1 && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0)) // 任意按键按下
    {
        delay_10us(1000);//消抖
        key = 0;
        if(KEY1 == 0)
            return KEY1_PRESS; // 按键1按下
        else if(KEY2 == 0)
            return KEY2_PRESS; // 按键2按下
        else if(KEY3 == 0)
            return KEY3_PRESS; // 按键3按下
        else if(KEY4 == 0)
            return KEY4_PRESS; // 按键4按下
    }
    else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1) // 无按键按下
    {
        key = 1;            
    }
    return KEY_UNPRESS;        
}
```

    按键检测函数我们在前面的章节就已经讲过,应该不用再重复了，并且我也加了注释

### 5.2 数码管显示函数

```c
#ifndef _smg_H
#define _smg_H

#include "public.h"

#define SMG_A_DP_PORT    P0    //使用宏定义数码管段码口

//定义数码管位选信号控制脚
sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;

void smg_display(unsigned char dat[], unsigned char pos);

#endif
```

```c
#include "smg.h"

//共阴极数码管显示0~F的段码数据
unsigned char gsmg_code[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
                0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

// 数码管显示函数
void smg_display(unsigned char dat[],unsigned char pos)
{
    unsigned char i = 0;
    unsigned char pos_temp = pos-1;

    for(i = pos_temp; i<8; i++)
    {
           switch(7 - i) //位选
        {
            case 0: 
                LSC=1;LSB=1;LSA=1;
                break;
            case 1: 
                LSC=1;LSB=1;LSA=0;
                break;
            case 2: 
                LSC=1;LSB=0;LSA=1;
                break;
            case 3: 
                LSC=1;LSB=0;LSA=0;
                break;
            case 4: 
                LSC=0;LSB=1;LSA=1;
                break;
            case 5: 
                LSC=0;LSB=1;LSA=0;
                break;
            case 6: 
                LSC=0;LSB=0;LSA=1;
                break;
            case 7: 
                LSC=0;LSB=0;LSA=0;
                break;
        }
        SMG_A_DP_PORT = gsmg_code[dat[i-pos_temp]]; // 传送段选数据
        delay_10us(100); // 延时一段时间，等待显示稳定
        SMG_A_DP_PORT = 0x00; // 消隐
    }
} 
```

    数码管显示函数也是老朋友了，段选位选是核心，还有不要忘记消隐。

### 5.3 I2C读写函数

```c
#include "iic.h"

// i2c产生起始信号
void iic_start(void)
{
    IIC_SDA = 1; //如果把该条语句放在SCL后面，第二次读写会出现问题
    delay_10us(1);
    IIC_SCL = 1;
    delay_10us(1);
    IIC_SDA = 0; // 当SCL为高电平时，SDA由高变为低
    delay_10us(1);
    IIC_SCL = 0; //钳住I2C总线，准备发送或接收数据
    delay_10us(1);
}

// 产生i2c停止信号
void iic_stop(void)
{    
    IIC_SDA = 0; // 如果把该条语句放在SCL后面，第二次读写会出现问题
    delay_10us(1);
    IIC_SCL = 1;
    delay_10us(1);
    IIC_SDA = 1; // 当SCL为高电平时，SDA由低变为高
    delay_10us(1);            
}

// 产生ACK应答
void iic_ack(void)
{
    IIC_SCL = 0;
    IIC_SDA = 0; // SDA为低电平
    delay_10us(1);     
    IIC_SCL = 1;
    delay_10us(1);
    IIC_SCL = 0;
}

// 产生NACK非应答  
void iic_nack(void)
{
    IIC_SCL = 0;
    IIC_SDA = 1; // SDA为高电平
    delay_10us(1);
    IIC_SCL = 1;
    delay_10us(1);
    IIC_SCL = 0;    
}

// 等待应答信号到来   
unsigned char iic_wait_ack(void)
{
    unsigned char time_temp = 0;

    IIC_SCL = 1;
    delay_10us(1);
    while(IIC_SDA)    //等待SDA为低电平
    {
        time_temp++;
        if(time_temp>100) // 超时则强制结束IIC通信
        {    
            iic_stop();
            return 1;    
        }            
    }
    IIC_SCL = 0;
    return 0;    
}

// IIC发送一个字节 
void iic_write_byte(unsigned char dat)
{                        
    unsigned char i=0; 

    IIC_SCL = 0;
    for(i = 0; i  <8; i++) // 循环8次将一个字节传出，先传高再传低位
    {              
        if((dat&0x80)>0) 
            IIC_SDA=1;
        else
            IIC_SDA=0;
        dat<<=1;       
        delay_10us(1);  
        IIC_SCL=1;
        delay_10us(1); 
        IIC_SCL=0;    
        delay_10us(1);
    }     
}

// IIC读一个字节 
unsigned char iic_read_byte(unsigned char ack)
{
    unsigned char i=0,receive=0;

    for(i=0;i<8;i++ )    //循环8次将一个字节读出，先读高再传低位
    {
        IIC_SCL=0; 
        delay_10us(1);
        IIC_SCL=1;
        receive<<=1;
        if(IIC_SDA)receive++;   
        delay_10us(1); 
    }                     
    if (!ack)
        iic_nack();
    else
        iic_ack();  

    return receive;
}
```

### 5.4 AT24C02读写字节函数

```c
#ifndef _24c02_H
#define _24c02_H

#include "public.h"

void at24c02_write_one_byte(unsigned char addr, unsigned char dat); // AT24C02指定地址写数据
unsigned char at24c02_read_one_byte(unsigned char addr); // AT24C02指定地址读数据

#endif
```

```c
#include "24c02.h"
#include "iic.h"
// 在AT24CXX指定地址写入一个数据
void at24c02_write_one_byte(unsigned char addr,unsigned char dat)
{                                                                                                  
    iic_start();  
    iic_write_byte(0XA0);// 发送写命令              
    iic_wait_ack();       
    iic_write_byte(addr);// 发送写地址   
    iic_wait_ack();                                                           
    iic_write_byte(dat); // 发送字节                                   
    iic_wait_ack();                     
    iic_stop();          // 产生一个停止条件
    delay_ms(10);     
}

// 在AT24CXX指定地址读出一个数据
unsigned char at24c02_read_one_byte(unsigned char addr)
{                  
    unsigned char temp=0;                                                                                   
    iic_start();  
    iic_write_byte(0XA0); // 发送写命令       
    iic_wait_ack(); 
    iic_write_byte(addr); // 发送写地址  
    iic_wait_ack();        
    iic_start();              
    iic_write_byte(0XA1); // 进入接收模式                        
    iic_wait_ack();     
    temp=iic_read_byte(0);// 读取字节           
    iic_stop();

    return temp;            //返回读取的数据
}
```

### 5.5 主函数

```c
#include "public.h"
#include "24c02.h"
#include "key.h"
#include "smg.h"

#define EEPROM_ADDRESS    0    // 定义数据存入EEPROM的起始地址

void main()
{    
    unsigned char key_temp = 0;
    unsigned char save_value = 0;
    unsigned char save_buf[3];

    while(1)
    {            
        key_temp = key_scan(0);
        if(key_temp == KEY1_PRESS)
        {
            at24c02_write_one_byte(EEPROM_ADDRESS, save_value);
        }
        else if(key_temp == KEY2_PRESS)
        {
            save_value = at24c02_read_one_byte(EEPROM_ADDRESS);
        }
        else if(key_temp == KEY3_PRESS)
        {
            save_value++;
            if(save_value == 255)save_value = 255;
        }
        else if(key_temp == KEY4_PRESS)
        {
            save_value  =0;    
        }
        save_buf[0] = save_value/100;
        save_buf[1] = save_value%100/10;
        save_buf[2] = save_value%100%10;
        smg_display(save_buf,6);
    }        
}
```

## 6. 小结

- 按键检测还有数码管显示函数不必多说，下面我们要重点分析的是i2c配置和AT24C02读写字节函数

- 先了解一下i2c配置函数：

    首先就是i2c起始信号了

```c
// iic起始信号
void iic_start(void)
{
    IIC_SDA = 1; // 如果把该条语句放在SCL后面，第二次读写会出现问题
    delay_10us(1);

    IIC_SCL = 1;
    delay_10us(1);

    IIC_SDA = 0; // 当SCL为高电平时，SDA由高变为低
    delay_10us(1);

    IIC_SCL = 0; // 钳住I2C总线，准备发送或接收数据
    delay_10us(1);
}
```

无法理解？如果你有数电的基础，看一下下面的时序图就懂了：

![](https://doc.embedfire.com/mcu/stm32/f103mini/std/zh/latest/_images/I2C008.jpg)

当 SCL 线是高电平时 SDA 线从高电平向低电平切换，这个情况表示通讯的起始。

既然有开始就有停止信号：

```c
// iic停止信号
void iic_stop(void)
{    
    IIC_SDA = 0; // 如果把该条语句放在SCL后面，第二次读写会出现问题
    delay_10us(1);

    IIC_SCL = 1;
    delay_10us(1);

    IIC_SDA = 1; // 当SCL为高电平时，SDA由低变为高
    delay_10us(1);            
}
```

分析同开始信号一样，不过是先拉低SDA（0）再高电平（1）代表停止发送信号，而开始信息就是1->0啦。（SDA）

那么如何让i2c产生ACK应答呢？我们不妨把产生非应答拉过来一起分析

```c
// iic产生ACK应答
void iic_ack(void)
{
    IIC_SCL = 0;
    IIC_SDA = 0; // SDA为低电平
    delay_10us(1);    

       IIC_SCL = 1;
    delay_10us(1);

    IIC_SCL = 0;
}
```

```c
// 产生NACK非应答  
void iic_nack(void)
{
    IIC_SCL = 0;
    IIC_SDA = 1; // SDA为高电平
    delay_10us(1);

       IIC_SCL = 1;
    delay_10us(1);

    IIC_SCL = 0;    
}
```

可以看到区别就是IIC_SDA是否为1即高低电平的区别，SDA即是串行数据线，在我们写的应答函数里，SDA如果为低电平就代表产生ACK应答咯

接着我们再看I2C等待应答

```c
unsigned char iic_wait_ack(void)
{
    unsigned char time_temp = 0;

    IIC_SCL = 1; // 拉高SCL，准备发送应答-串行时钟线
    delay_10us(1);
    while(IIC_SDA) // 等待SDA为低电平
    {
        time_temp++; // 如果SDA不为低电平，计时器加1
        if(time_temp > 100) // 超时则强制结束IIC通信
        {    
            iic_stop(); // 调用信号停止函数
            return 1; // 超时返回1-异常    
        }            
    }
    IIC_SCL = 0; // 拉低SCL，准备接收应答
    return 0; // 正常返回0
    // 此时返回0代表正常，那么可以准备收发数据了
}
```

最后来到I2C收发字节函数：

```c
// iic发送一个字节
void iic_write_byte(unsigned char dat)
{                        
    unsigned char i = 0; 

    IIC_SCL = 0;  // 将时钟线拉低，准备发送数据

    for(i = 0; i < 8; i++) // 循环8次将一个字节传出，先传高位再传低位
    {              
        if((dat & 0x80) > 0) // 检查dat的最高位是否为1
            IIC_SDA = 1;     // 如果最高位为1，则数据线拉高（发送逻辑1）
        else
            IIC_SDA = 0;     // 如果最高位为0，则数据线拉低（发送逻辑0）

        dat <<= 1;  // 将数据dat向左移动一位，准备发送下一个位
        delay_10us(1);  // 稍作延时，保证时序满足要求

        IIC_SCL = 1;  // 将时钟线拉高，通知接收方可以读取数据
        delay_10us(1);  // 稍作延时，保证时序满足要求

        IIC_SCL = 0;  // 将时钟线再次拉低，为发送下一位数据做准备
        delay_10us(1);  // 稍作延时，保证时序满足要求
    }     
}
```

```c
// IIC读一个字节 ack=1时，发送ACK，ack=0，发送nACK 
unsigned char iic_read_byte(unsigned char ack)
{
    unsigned char i = 0, receive = 0;

    for(i =0; i < 8; i++ ) // 循环8次将一个字节读出，先读高再传低位
    {
        IIC_SCL = 0; 
        delay_10us(1);

        IIC_SCL = 1;
        receive <<= 1;

        if(IIC_SDA)
            receive++;   
        delay_10us(1); 
    }                     
    if (!ack)
        iic_nack(); // 发送nACK
    else
        iic_ack(); // 发送ACK

    return receive; // 返回读出的字节
}
```

- 熟悉I2C的配置，接下来AT24C02写/读数据函数就更简单了

```c
// AT24C02的写入数据的函数
void at24c02_write_one_byte(unsigned char addr, unsigned char dat)
{                                                                                                  
    iic_start(); // iic开始信号 
    iic_write_byte(0XA0); // 发送写命令 1010 0000

    iic_wait_ack(); // iic等待应答                   
    iic_write_byte(addr); // 发送写地址

    iic_wait_ack();                                                           
    iic_write_byte(dat); // 发送字节

    iic_wait_ack();                     
    iic_stop(); // 产生一个停止条件
    delay_ms(10);     
}
```

这个还是很简单明了，首先利用我们写好的I2C开始信号函数代表I2C已经准备好可以开始发送信号，之后就利用I2C发送字节函数发送写命令，接着就是等待应答咯，要是没有返回错误就继续发送写地址和字节了，最后发送I2C停止信号。

读数据函数大同小异，看看注释得了。

```c
// AT24C02的读取数据的函数
unsigned char at24c02_read_one_byte(unsigned char addr)
{                  
    unsigned char temp = 0; // 定义一个临时变量存储读取的数据          

    iic_start(); // iic开始信号  
    iic_write_byte(0XA0); // 发送写命令

    iic_wait_ack(); // iic等待应答                   
    iic_write_byte(addr); // 发送写地址

    iic_wait_ack();        
    iic_start();              
    iic_write_byte(0XA1); // 进入接收模式 

    iic_wait_ack();     
    temp = iic_read_byte(0); // 读取字节

    iic_stop();    // 产生一个停止条件

    return temp; // 返回读取的数据
}
```

该章重点其实是学会模块化工程，关于I2C配置不会，其实只要拿来用就行了，EEPROM也是一样，如果需要具体学习，后面会有一个扩展章节：关于51单片机配置I2C

---

2024.7.21 第一次修订

2024.8.22 第二次修订，后期不在维护
