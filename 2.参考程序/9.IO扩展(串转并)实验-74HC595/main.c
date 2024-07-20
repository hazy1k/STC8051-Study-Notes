#include <REGX52.H>

// 定义74HC595控制管脚
sbit SC = P3^6;	// 串行输入时钟(SCK)
sbit RC = P3^5;	// 存储寄存器时钟(RCK)
sbit SE = P3^4; // 串行数据输入(SER)

unsigned char ghc595_buf[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

// 延时函数，ten_us=1时，大约延时10us
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// ms延时函数，ms=1时，大约延时1ms
void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

/*
向74HC595写入4个字节的数据
dat1：对应74HC595(A)输出第1行-第8行
dat2：对应74HC595(B)输出第9行-第16行
dat3：对应74HC595(C)输出第1列-第8列
dat4：对应74HC595(D)输出第9列-第16列
*/
void hc595_write_data(unsigned char dat1, unsigned char dat2, unsigned char dat3, unsigned char dat4)
{
	unsigned char i = 0;
	
	// 详细分析dat4，其他同理
	for(i = 0; i < 8; i++) // 循环8次即可将一个字节写入寄存器中
	{
		SE = dat4 >> 7; // 将最高位赋给串行输入引脚
		dat4 <<= 1; // 将下一个数据移到最高位
		SC = 0; // 将串行输入时钟置0，准备将数据输入到74HC595寄存器
		delay_10us(1);
		SC = 1; // 将时钟引脚SC置为1，将数据从SE引脚输入到74HC595寄存器
		delay_10us(1); // 移位寄存器时钟上升沿将端口数据送入寄存器中	
	}
	
	// 分析同上，只是换了输出的地方
	for(i = 0; i < 8; i++)
	{
		SE = dat3 >> 7;
		dat3 <<= 1;
		SC = 0;
		delay_10us(1);
		SC = 1;
		delay_10us(1);	
	}

	for(i = 0; i < 8; i++)
	{
		SE = dat2 >> 7;
		dat2 <<= 1;
		SC = 0;
		delay_10us(1);
		SC = 1;
		delay_10us(1);	
	}

	for(i = 0; i < 8; i++)
	{
		SE = dat1 >> 7;
		dat1 <<= 1;
		SC = 0;
		delay_10us(1);
		SC = 1;
		delay_10us(1);	
	}

	// 这段代码将存储寄存器的时钟引脚RC置为1，然后立即置为0。
	// 这个操作是存储74HC595寄存器中之前输入的所有数据，使其在输出引脚上可用。
	RC = 1;
	delay_10us(1);
	RC = 0; // 存储寄存器时钟上升沿将前面写入到寄存器的数据输出	
}

// 控制一行逐渐滚动输出
void main()
{	
	unsigned char i = 0; // 计数器

	// A和B控制行，C和D控制列
	// 如果我们想一列滚动显示的，你应该知道怎么做的
	while(1)
	{		
		for(i = 0; i < 8; i++)
		{	
			hc595_write_data(ghc595_buf[i], 0, 0, 0); // 使74HC595(A)输出
			delay_ms(100); // 延时100ms	
		}
		for(i = 0; i < 8; i++)
		{	
			hc595_write_data(0,ghc595_buf[i],0,0); // 使74HC595(B)输出
			delay_ms(100);
		}									
	}		
}