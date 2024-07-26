#include "reg52.h"

typedef unsigned int u16;	//对系统默认数据类型进行重定义
typedef unsigned char u8;

//定义LED1管脚
sbit LED1=P2^0;

void delay_10us(u16 ten_us)
{
	while(ten_us--);	
}

void time1_init(void)
{
	TMOD |= 0X10; // 选择为定时器0模式，工作方式1
	TH1 = 0XFC;	// 给定时器赋初值，定时1ms
	TL1=0X18;

	ET1 = 1; // 打开定时器1中断允许
	EA = 1; // 打开总中断
	TR1 = 1; // 打开定时器		
}

void main()
{	
	time1_init();//定时器1中断配置

	while(1)
	{			
							
	}		
}

void time1() interrupt 3 // 定时器1中断函数
{
	static u16 i; // 定义静态变量i
	TH1 = 0XFC;	// 给定时器赋初值，定时1ms
	TL1 = 0X18;
	i++;
	
	if(i == 1000)
	{
		i = 0;
		LED1 =! LED1;	
	}						
}