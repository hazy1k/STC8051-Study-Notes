#include <REGX52.H>

// 定义LED1管脚
sbit LED1 = P2^0;

// 定义独立按键K3控制脚
sbit KEY3=P3^2;

// 延时函数
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// 外部中断0配置函数
void exti0_init()
{
	IT0 = 1; // 下降沿触发 
	EX0 = 1; // 使能外部中断0
	EA = 1; // 使能总中断
}

// 外部中断0中断函数
// 如果触发外部中断0，则执行该函数
void exti0() interrupt 0
{
	delay_10us(1000); // 消抖

	if(KEY3 == 0) // 判断K3键是否按下
		LED1 =! LED1; // LED1状态翻转					
}

void main()
{	
	exti0_init(); // 外部中断0配置

	while(1)
	{			
							
	}		
}
