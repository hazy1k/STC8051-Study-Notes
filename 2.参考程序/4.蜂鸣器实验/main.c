#include <REGX52.H>

sbit BEEP = P2^5; // 将P2.5管脚定义为BEEP-即蜂鸣器

// 延时函数-已经很熟悉了，不必解释
void delay_us(unsigned int ten_us)
{
	while(ten_us--);	
}

void main()
{
	unsigned int i;

	BEEP = 0; // 初始化时关闭蜂鸣器

	for(i = 0; i < 1000; i++)
	{
		BEEP = !BEEP; // 切换蜂鸣器状态
		delay_us(500);
	}
	BEEP = 0; // 循环结束关闭蜂鸣器

	while(1); // 维持程序继续运行

}