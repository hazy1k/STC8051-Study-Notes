#include <REGX52.H>

//定义直流电机控制管脚
sbit DC_Motor = P1^0; // 定义DC_Motor为P1.0口

//ms延时函数，ms=1时，大约延时1ms
void delay_ms(unsigned int ms)
{
	int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

int main()
{	
	DC_Motor = 1; // 給高电平-开启电机
	delay_ms(5000); // 设定工作时间
	DC_Motor = 0;// 给低电平-关闭电机
	while(1)
	{			
								
	}		
}
