#include <REGX52.H>

// 配置串口通信初始化
void uart_init(unsigned int baud)
{
	TMOD |= 0X20; // 设置计数器工作方式2
	SCON = 0X50; // 设置为工作方式1
	PCON = 0X80; // 波特率加倍
	TH1 = baud; // 计数器初始值设置
	TL1 = baud;

	// 配置中断器
	ES = 1; // 打开接收中断
	EA = 1; // 打开总中断
	TR1 =1 ; // 打开计数器		
}
// 上面代码理解最好，其实真正使用的时候我们往往都是复制粘贴，然后修改一些参数，比如波特率，数据位，校验位等等。

// 串口通信中断函数
void uart() interrupt 4
{
	unsigned int rec_data; // 接收到的数据
										  
	RI = 0;	// 清除接收中断标志位

	rec_data = SBUF; // 存储接收到的数据
	SBUF = rec_data; // 将接收到的数据放入到发送寄存器

	while(!TI);	// 等待发送数据完成
	TI = 0; // 清除发送完成标志位				
}

void main()
{	
	uart_init(0XFA); // 波特率为9600

	while(1)
	{			
							
	}		
}
