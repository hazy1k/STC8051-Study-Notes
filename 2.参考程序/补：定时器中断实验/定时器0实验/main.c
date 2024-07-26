#include <REGX52.H>

// 定义LED1管脚
sbit LED1 = P2^0;

// 定时器0中断配置函数，通过设置TH和TL即可确定定时时间
void time0_init() // 定时器初始化函数
{
	// 1.选择为定时器0模式，工作方式1
	TMOD |= 0X01;

	// 2.给定时器赋初值，定时1ms
	TH0 = 0XFC;
	TL0 = 0X18;

	// 3.中断配置	
	ET0 = 1; // 打开定时器0中断允许
	EA = 1; // 打开总中断
	TR0 = 1; // 打开定时器		
}

// 定时器0中断函数
void time0() interrupt 1 
{
	static unsigned int i; // 定义静态变量i

	TH0 = 0XFC; // 给定时器赋初值，定时1ms
	TL0 = 0X18;
	i++; // 计时器

	if(i == 1000) // 计时1秒
	{
		i = 0; // 重新开始计时
		LED1 = !LED1; // LED状态翻转
	}						
}

// 主函数
int main()
{	
	time0_init(); // 定时器0中断配置

	while(1)
	{			
							
	}		
}

