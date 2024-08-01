/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 
实验名称：串口控制LED和蜂鸣器
接线说明：	
实验现象：下载程序后，由串口助手以HEX格式发送如下命令控制LED和蜂鸣器
			D1指示灯亮发送：11 0D 0A
			D1指示灯灭发送：10 0D 0A
			蜂鸣器响发送：	21 0D 0A
			蜂鸣器停发送：	20 0D 0A
注意事项：	串口助手要以HEX格式发送，并且波特率为9600																			  
***************************************************************************************/
#include "public.h"
#include "beep.h"
#include "uart.h"

//控制管脚定义
sbit LED1=P2^0;

//宏定义
#define LED1_ON_CMD		0X11
#define LED1_OFF_CMD	0X10

#define BEEP_ON_CMD		0X21
#define BEEP_OFF_CMD	0X20

/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	u8 beep_flag=0;
	
	UART_Init();

	while(1)
	{
		if(UART_RX_STA&0x8000)//判断串口是否接收完数据
		{
			UART_RX_STA=0;//清除标记，等待下次接收
			switch(UART_RX_BUF[0])//判断第一个数据
			{
				case LED1_ON_CMD: LED1=0;break;
			   	case LED1_OFF_CMD: LED1=1;break;
				case BEEP_ON_CMD: beep_flag=1;break;
				case BEEP_OFF_CMD: beep_flag=0;break;
			}	
		}
		if(beep_flag)
			beep_alarm(100,10);			
	}
}
