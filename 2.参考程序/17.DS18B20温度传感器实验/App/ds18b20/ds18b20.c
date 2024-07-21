#include "ds18b20.h"
#include "intrins.h"

// 复位DS18B20  
void ds18b20_reset(void)
{
	DS18B20_PORT = 0; // 拉低DQ
	delay_10us(75); // 拉低750us

	DS18B20_PORT = 1; // DQ=1
	delay_10us(2); // 20US
}

// 检测DS18B20是否存在,1:未检测到DS18B20的存在，0:存在
unsigned char ds18b20_check(void)
{
	unsigned char time_temp = 0; // 记录时间

	while(DS18B20_PORT && time_temp < 20) // 等待DQ为低电平并且不能超时
	{
		time_temp++; // 计数器加1
		delay_10us(1);	
	}

	if(time_temp >= 20)
		return 1; // 如果超时则强制返回1
	else 
		time_temp = 0;
	while((!DS18B20_PORT) && time_temp < 20) // 等待DQ为高电平
	{
		time_temp++;
		delay_10us(1);
	}
	if(time_temp>=20)
	{
		return 1; // 如果超时则强制返回1
	}
	return 0;
}

// 从DS18B20读取一个位
unsigned char ds18b20_read_bit(void)
{
	unsigned char dat = 0; // 临时数据变量
	
	DS18B20_PORT = 0; // 拉低DQ
	_nop_();_nop_();

	DS18B20_PORT = 1;	
	_nop_();_nop_(); //该段时间不能过长，必须在15us内读取数据

	if(DS18B20_PORT)
		dat = 1; // 如果总线上为1则数据dat为1，否则为0
	else 
		dat=0;
	delay_10us(5);
	return dat;
} 

// ds18b20读取一个字节
unsigned char ds18b20_read_byte(void)
{
	unsigned char i = 0; // 位数
	unsigned char dat = 0; // 数据变量
	unsigned char temp = 0; // 临时变量

	for(i = 0; i < 8; i++) // 循环8次，每次读取一位，且先读低位再读高位
	{
		temp = ds18b20_read_bit(); // 读取一位数据
		dat = (temp << 7) | (dat >> 1); // 合并数据
	}
	return dat;	
}

// 写一个字节到DS18B20
void ds18b20_write_byte(unsigned char dat)
{
	unsigned char i=0;
	unsigned char temp=0;

	for(i = 0; i < 8; i++)//循环8次，每次写一位，且先写低位再写高位
	{
		temp = dat & 0x01; // 选择低位准备写入
		dat >>=1; // 将次高位移到低位
		if(temp)
		{
			DS18B20_PORT=0;
			_nop_();_nop_();

			DS18B20_PORT=1;	
			delay_10us(6);
		}
		else
		{
			DS18B20_PORT=0;
			delay_10us(6); 

			DS18B20_PORT=1;
			_nop_();_nop_();	
		}	
	}	
}

// 开始温度转换
void ds18b20_start(void)
{
	ds18b20_reset(); // 复位
	ds18b20_check(); // 检查DS18B20
	ds18b20_write_byte(0xcc); // SKIP ROM
    ds18b20_write_byte(0x44); // 转换命令	
}

// 初始化DS18B20的IO口 DQ 同时检测DS的存在 
unsigned char ds18b20_init(void)
{
	ds18b20_reset(); // 首先复位

	return ds18b20_check();	// 返回检查结果
}

// 从ds18b20得到温度值
float ds18b20_read_temperture(void)
{
	float temp;
	unsigned char dath = 0; // 高字节
	unsigned char datl = 0; // 低字节
	unsigned int value = 0; // 合并数据

	ds18b20_start(); // 开始转换
	ds18b20_reset(); // 复位
	ds18b20_check(); // 检查DS18B20
	ds18b20_write_byte(0xcc); // SKIP ROM
    ds18b20_write_byte(0xbe); // 读存储器

	datl = ds18b20_read_byte(); // 低字节
	dath = ds18b20_read_byte(); // 高字节
	value = (dath << 8) + datl; // 合并为16位数据

	if((value&0xf800) == 0xf800) // 判断符号位，如果为负温度
	{
		value=(~value) + 1; // 数据取反再加1
		temp=value * (-0.0625); // 乘以精度	
	}
	else // 如果为正温度
	{
		temp = value*0.0625; // 乘以精度
	}

	return temp; // 返回显示结果
}
