#include "reg52.h"

typedef unsigned int u16;	//对系统默认数据类型进行重定义
typedef unsigned char u8;

// 定义LED1管脚
sbit LED1 = P2^0;

//定义独立按键K4控制脚
sbit KEY4 = P3^3;

void delay_10us(u16 ten_us)
{
	while(ten_us--);	
}

// 外部中断1初始化函数
void exti1_init(void)
{
	IT1 = 1; // 跳变沿触发方式（下降沿）
	EX1 = 1; // 打开INT1的中断允许
	EA = 1; // 打开总中断
}

void main()
{	
	exti1_init(); // 外部中断1配置

	while(1)
	{			
							
	}		
}

void exti1() interrupt 2 // 外部中断1中断函数
{
	delay_10us(1000); // 消抖
	if(KEY4 == 0) // 再次判断K4键是否按下
		LED1 =! LED1;//LED1状态翻转					
}