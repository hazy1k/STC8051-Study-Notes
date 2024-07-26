#include <REGX52.H>

// 定义74HC165控制管脚
sbit HC165_QH = P1^7; // 串行数据输出
sbit HC165_CLK = P1^6; // 时钟输入
sbit HC165_SHLD = P1^5; // 寄存器存储控制

#define LED_PORT	P2	// LED模块控制

// 延时函数
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// 读取数据函数
// 从74HC165中读取一个字节数据
unsigned char hc165_read_data(void)
{
	unsigned char i = 0;
	unsigned char value = 0;

	HC165_SHLD = 0; // SHLD拉低，165并口数据传入到移位寄存器中
	delay_10us(5); // 短暂延时
	HC165_SHLD=1; // SHLD拉高，禁止165并口数据传入到移位寄存器中

	for(i = 0; i < 8; i++) // 循环8次读取一个字节
	{
		value <<= 1; // 左移1位，先读取的数据是高位，经过8次循环则将第一次读取的位放到字节高位
		HC165_CLK = 0; // CLK INH为低电平开始读取数据
		delay_10us(5);

		value |= HC165_QH; // 读取数据，并将数据放到value的最高位
		HC165_CLK = 1;//上升沿读取数据
		delay_10us(5);				
	}	
	return value; // 返回读取到的数据
}

void main()
{	
	unsigned char value = 0;

	LED_PORT = 0xff; // 保证LED默认是熄灭的
	while(1)
	{		
		value = hc165_read_data();
		
		if(value != 0xff)
			LED_PORT = value;								
	}		
}