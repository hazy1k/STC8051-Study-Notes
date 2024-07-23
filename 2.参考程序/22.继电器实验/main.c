#include <REGX52.H>

//定义继电器控制管脚
sbit Relay = P1^4;

#define RELAY_OEPN_TIME	5000 // 定义继电器吸合时间为5000ms

// 延时函数
void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

void main()
{	
	// 继电器开关过程
	// 1.首先吸合继电器
	// 2.延时
	// 3.然后断开继电器
	
	Relay = 0; // 继电器吸合
	delay_ms(RELAY_OEPN_TIME); // 延时
	Relay = 1; // 继电器断开

	while(1)
	{			
								
	}		
}