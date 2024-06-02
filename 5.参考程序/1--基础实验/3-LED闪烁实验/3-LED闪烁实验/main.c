#include <REGX52.H>

sbit LED1=P2^0;	//将P2.0管脚定义为LED1


void delay_us(unsigned int set_us)
{
	while(set_us--);	
}

void main()
{	
	while(1)
	{
		LED1 = 0; // 低电平点亮
		delay_us(50000); //大约延时450ms

		LED1=1;	//熄灭
		delay_us(50000); 	
	}		
}