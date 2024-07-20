#include <REGX52.H>

//定义K1按键控制脚
sbit KEY1 = P3^0;

//定义LED1控制脚
sbit LED1 = P2^0;

// 延时函数
void delay_us(unsigned int ten_us)
{
	while(ten_us--);	
}

int main()
{
	while(1)
	{
		if(KEY1 == 0) // 检测端口是否为低电平-开关按下
		{
			delay_us(20); // 消抖
			LED1 = !LED1; // 模拟灯的亮灭-翻转状态
		}
	}
}