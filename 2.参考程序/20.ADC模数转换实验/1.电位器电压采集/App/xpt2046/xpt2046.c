#include "xpt2046.h"
#include "intrins.h"

// xpt2046写入数据
void xpt2046_wirte_data(unsigned char dat)
{
	unsigned char i;

	CLK = 0; // 先拉低时钟
	_nop_(); // 延时100ns

	for(i = 0; i < 8; i++) // 循环8次，每次传输一位，共一个字节
	{
		DIN = dat >> 7; // 先传高位再传低位
		dat <<= 1; // 将低位移到高位

		CLK = 0; // CLK由低到高产生一个上升沿，从而写入数据
		_nop_();

		CLK = 1;
		_nop_();
	}
}

// XPT2046读数据
unsigned int xpt2046_read_data(void)
{
	unsigned char i;
	unsigned int dat = 0;

	CLK = 0; // 先拉低时钟
	_nop_();

	for(i = 0; i < 12; i++) // 循环12次，每次读取一位，大于一个字节数，所以返回值类型是u16
	{
		dat <<= 1; // 将低位移到高位

		CLK = 1;
		_nop_();
		CLK = 0; // CLK由高到低产生一个下降沿，从而读取数据
		_nop_();

		dat |= DOUT; // 从DOUT读取数据
	}
	return dat;	
}

// XPT2046读AD数据
unsigned int xpt2046_read_adc_value(unsigned char cmd)
{
	unsigned char i;
	unsigned int adc_value = 0;

	CLK = 0; // 先拉低时钟
	CS  = 0; // 使能XPT2046
	xpt2046_wirte_data(cmd); // 发送命令字
	for(i = 6; i > 0; i--); // 延时等待转换结果

	CLK = 1;
	_nop_();
	CLK = 0; // 发送一个时钟，清除BUSY
	_nop_();

	adc_value = xpt2046_read_data(); // 读取ADC数据

	CS = 1; // 关闭XPT2046
	
	return adc_value;
}
