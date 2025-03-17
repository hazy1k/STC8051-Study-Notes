
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


#include	"config.h"
#include	"USART.h"


/*************	功能说明	**************

4串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

默认参数:
所有设置均为 1位起始位, 8位数据位, 1位停止位, 无校验.
每个串口可以使用不同的波特率.
串口1(P3.0 P3.1): 115200bps.
串口2(P1.0 P1.1):  57600bps.
串口3(P0.0 P0.1):  38400bps.
串口4(P0.2 P0.3):  19200bps.

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/




/***************** 串口配置函数 *****************************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2,USART3,USART4

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_8bit_BRTx,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 57600ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//切换端口,   UART2_SW_P10_P11, UART2_SW_P46_P47
	USART_Configuration(USART2, &COMx_InitStructure);		//初始化串口2 USART1,USART2,USART3,USART4

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_8bit_BRTx,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer3;			//使用波特率, BRT_Timer3, BRT_Timer2
	COMx_InitStructure.UART_BaudRate  = 38400ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_P_SW      = UART3_SW_P00_P01;	//切换端口,   UART3_SW_P00_P01, UART3_SW_P50_P51
	USART_Configuration(USART3, &COMx_InitStructure);		//初始化串口2 USART1,USART2,USART3,USART4

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_8bit_BRTx,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer4;			//使用波特率, BRT_Timer4, BRT_Timer2
	COMx_InitStructure.UART_BaudRate  = 19200ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_P_SW      = UART4_SW_P02_P03;	//切换端口,   UART4_SW_P02_P03, UART4_SW_P52_P53
	USART_Configuration(USART4, &COMx_InitStructure);		//初始化串口4 USART1,USART2,USART3,USART4

}


/***************** 主程序 *****************************/
void main(void)
{
	P0n_standard(0xff);	//设置为准双向口
	P1n_standard(0xff);	//设置为准双向口
	P2n_standard(0xff);	//设置为准双向口
	P3n_standard(0xff);	//设置为准双向口
	P4n_standard(0xff);	//设置为准双向口
	P5n_standard(0xff);	//设置为准双向口
	
	UART_config();	//串口初始化
	EA = 1;

	PrintString(USART1, "STC15F4K60S4 USART1 Test Prgramme!\r\n");	//USART1发送一个字符串
	PrintString(USART2, "STC15F4K60S4 USART2 Test Prgramme!\r\n");	//USART2发送一个字符串
	PrintString(USART3, "STC15F4K60S4 USART3 Test Prgramme!\r\n");	//USART3发送一个字符串
	PrintString(USART4, "STC15F4K60S4 USART4 Test Prgramme!\r\n");	//USART4发送一个字符串

	while (1)
	{
		if((COM1.TX_read != COM1.RX_write) && (COM1.TX_Busy == 0))	//收到过数据, 并且发送空闲
		{
			COM1.TX_Busy = 1;		//标志发送忙
			SBUF = COM1.RX_Buffer[COM1.TX_read];	//发一个字节
			if(++COM1.TX_read >= RX_Length)	COM1.TX_read = 0;	//避免溢出处理
		}

		if((COM2.TX_read != COM2.RX_write) && (COM2.TX_Busy == 0))	//收到过数据, 并且发送空闲
		{
			COM2.TX_Busy = 1;		//标志发送忙
			S2BUF = COM2.RX_Buffer[COM2.TX_read];	//发一个字节
			if(++COM2.TX_read >= RX_Length)	COM2.TX_read = 0;	//避免溢出处理
		}

		if((COM3.TX_read != COM3.RX_write) && (COM3.TX_Busy == 0))	//收到过数据, 并且发送空闲
		{
			COM3.TX_Busy = 1;		//标志发送忙
			S3BUF = COM3.RX_Buffer[COM3.TX_read];	//发一个字节
			if(++COM3.TX_read >= RX_Length)	COM3.TX_read = 0;	//避免溢出处理
		}

		if((COM4.TX_read != COM4.RX_write) && (COM4.TX_Busy == 0))	//收到过数据, 并且发送空闲
		{
			COM4.TX_Busy = 1;		//标志发送忙
			S4BUF = COM4.RX_Buffer[COM4.TX_read];	//发一个字节
			if(++COM4.TX_read >= RX_Length)	COM4.TX_read = 0;	//避免溢出处理
		}
	}
}



