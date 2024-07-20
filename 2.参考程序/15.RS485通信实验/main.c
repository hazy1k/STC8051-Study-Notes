#include <REGX52.H>

//定义控制引脚IO
sbit RS485_DIR = P1^0; // 发送和接收控制

void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// 串口通信中断配置函数，通过设置TH和TL即可确定定时时间
void uart_init(unsigned char baud)
{
	TMOD |= 0X20; // 设置计数器工作方式2
	SCON = 0X50; // 设置为工作方式1
	PCON = 0X80; // 波特率加倍
	TH1 = baud;	// 计数器初始值设置
	TL1 = baud;

	// 配置串口通信中断
	ES = 1;	// 打开接收中断
	EA = 1;	// 打开总中断
	TR1 = 1; // 打开计数器		
}

void uart() interrupt 4 // 串口通信中断函数
{
	unsigned char rec_data; // 接收到的数据

	RI = 0;			// 清除接收中断标志位
	rec_data = SBUF; // 存储接收到的数据
	delay_10us(100); // 延时10us
	RS485_DIR = 1; // 配置RS485为发送模式
	SBUF = rec_data; // 将接收到的数据放入到发送寄存器

	while(!TI);	 // 等待发送数据完成
	TI = 0;	// 清除发送完成标志位

	RS485_DIR = 0; // 配置RS485为接收模式				
}

void main()
{	
	uart_init(0XFA); // 波特率为9600

	RS485_DIR = 0; // 配置RS485为接收模式

	while(1)
	{			
							
	}		
}
