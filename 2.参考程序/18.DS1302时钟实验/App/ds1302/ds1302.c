#include "ds1302.h"
#include "intrins.h"

//---DS1302写入和读取时分秒的地址命令
//---秒分时日月周年 最低位读写位
unsigned char gREAD_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d}; 
unsigned char gWRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};

//---存储顺序是秒分时日月周年,存储格式是用BCD码
unsigned char gDS1302_TIME[7] = {0x47, 0x51, 0x13, 0x20, 0x04, 0x05, 0x21}; // 初始化2021年5月20日星期四13点51分47秒


// DS1302写单字节
void ds1302_write_byte(unsigned char addr, unsigned char dat) // 参数：地址，数据
{
	unsigned char i = 0;
	
	DS1302_RST = 0; // RST拉低
	_nop_();	

	DS1302_CLK = 0; // CLK低电平
	_nop_();

	DS1302_RST = 1; // RST由低到高变化-代表了复位结束从而可以准备开始写入数据
	_nop_();

	for(i = 0; i < 8; i++) // 循环8次，每次写1位，先写低位再写高位
	{
		DS1302_IO = addr & 0x01; // 取地址的低位
		addr >>= 1; 

		DS1302_CLK = 1; // CLK由低到高产生一个上升沿，从而写入数据
		_nop_();
		DS1302_CLK = 0; // CLK由低到高产生一个上升沿，从而写入数据
		_nop_();		
	}

	for(i = 0; i < 8; i++) // 循环8次，每次写1位，先写低位再写高位
	{
		DS1302_IO = dat & 0x01;
		dat >>= 1;

		DS1302_CLK = 1;
		_nop_();
		DS1302_CLK = 0;
		_nop_();		
	}

	DS1302_RST = 0; // RST拉低-复位
	_nop_();	
}

// DS1302读单字节
unsigned char ds1302_read_byte(unsigned char addr)
{
	unsigned char i = 0;
	unsigned char temp = 0;
	unsigned char value = 0;

	DS1302_RST=0;
	_nop_();	
	DS1302_CLK = 0; // CLK低电平
	_nop_();
	DS1302_RST = 1; // RST由低到高变化
	_nop_();
	
	for(i = 0; i < 8; i++) // 循环8次，每次写1位，先写低位再写高位
	{
		DS1302_IO = addr & 0x01;
		addr >>= 1;

		DS1302_CLK = 1;
		_nop_();
		DS1302_CLK = 0;
		_nop_();		
	}

	for(i = 0; i < 8; i++) // 循环8次，每次读1位，先读低位再读高位
	{
		temp = DS1302_IO; // 读取数据给临时变量
		value = (temp<<7) | (value>>1); // 先将value右移1位，然后temp左移7位，最后或运算-完成数据整合

		DS1302_CLK = 1;
		_nop_();
		DS1302_CLK = 0;
		_nop_();		
	}

	DS1302_RST = 0; // RST拉低
	_nop_();	
	DS1302_CLK=1; // 对于实物中，P3.4口没有外接上拉电阻的，此处代码需要添加，使数据口有一个上升沿脉冲。
	_nop_();
	DS1302_IO = 0;
	_nop_();
	DS1302_IO = 1;
	_nop_();	
	return value;		
}

// DS1302初始化时间
void ds1302_init(void)
{
	unsigned char i = 0;
	ds1302_write_byte(0x8E, 0X00); // 写入初始时间
	for(i = 0; i < 7; i++)
	{
		ds1302_write_byte(gWRITE_RTC_ADDR[i], gDS1302_TIME[i]);	
	}
	ds1302_write_byte(0x8E, 0X80);	
}

// DS1302读取时间
void ds1302_read_time(void)
{
	unsigned char i = 0;
	for(i = 0; i < 7; i++)
	{
		gDS1302_TIME[i] = ds1302_read_byte(gREAD_RTC_ADDR[i]);	
	}	
}
