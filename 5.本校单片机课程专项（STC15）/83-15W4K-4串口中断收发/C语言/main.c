
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


/*********************************************************/
#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟

#include	"STC15Fxxxx.H"


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
#define	RX1_Length	128		/* 接收缓冲长度 */
#define	RX2_Length	128		/* 接收缓冲长度 */
#define	RX3_Length	128		/* 接收缓冲长度 */
#define	RX4_Length	128		/* 接收缓冲长度 */

#define	UART_BaudRate1	115200UL	 /* 波特率 */
#define	UART_BaudRate2	 57600UL	 /* 波特率 */
#define	UART_BaudRate3	 38400UL	 /* 波特率 */
#define	UART_BaudRate4	 19200UL	 /* 波特率 */


/*************	本地变量声明	**************/
u8	xdata	RX1_Buffer[RX1_Length];	//接收缓冲
u8	xdata	RX2_Buffer[RX2_Length];	//接收缓冲
u8	xdata	RX3_Buffer[RX3_Length];	//接收缓冲
u8	xdata	RX4_Buffer[RX4_Length];	//接收缓冲

u8	TX1_read,RX1_write;	//读写索引(指针).
u8	TX2_read,RX2_write;	//读写索引(指针).
u8	TX3_read,RX3_write;	//读写索引(指针).
u8	TX4_read,RX4_write;	//读写索引(指针).

bit	B_TX1_Busy,B_TX2_Busy,B_TX3_Busy,B_TX4_Busy;	// 发送忙标志


/*************	本地函数声明	**************/
void	UART1_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void	UART2_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void	UART3_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
void	UART4_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
void 	PrintString1(u8 *puts);
void 	PrintString2(u8 *puts);
void 	PrintString3(u8 *puts);
void 	PrintString4(u8 *puts);




/**********************************************/
void main(void)
{

	P0n_standard(0xff);	//设置为准双向口
	P1n_standard(0xff);	//设置为准双向口
	P2n_standard(0xff);	//设置为准双向口
	P3n_standard(0xff);	//设置为准双向口
	P4n_standard(0xff);	//设置为准双向口
	P5n_standard(0xff);	//设置为准双向口
	
	UART1_config(1);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	UART2_config(2);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	UART3_config(3);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
	UART4_config(4);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
	
	EA = 1;

	PrintString1("STC15F4K60S4 USART1 Test Prgramme!\r\n");
	PrintString2("STC15F4K60S4 USART2 Test Prgramme!\r\n");
	PrintString3("STC15F4K60S4 USART3 Test Prgramme!\r\n");
	PrintString4("STC15F4K60S4 USART4 Test Prgramme!\r\n");

	while (1)
	{
		if((TX1_read != RX1_write) && !B_TX1_Busy)	//收到过数据, 并且发送空闲
		{
			B_TX1_Busy = 1;		//标志发送忙
			SBUF = RX1_Buffer[TX1_read];	//发一个字节
			if(++TX1_read >= RX1_Length)	TX1_read = 0;	//避免溢出处理
		}

		if((TX2_read != RX2_write) && !B_TX2_Busy)	//收到过数据, 并且发送空闲
		{
			B_TX2_Busy = 1;		//标志发送忙
			S2BUF = RX2_Buffer[TX2_read];	//发一个字节
			if(++TX2_read >= RX2_Length)	TX2_read = 0;	//避免溢出处理
		}

		if((TX3_read != RX3_write) && !B_TX3_Busy)	//收到过数据, 并且发送空闲
		{
			B_TX3_Busy = 1;		//标志发送忙
			S3BUF = RX3_Buffer[TX3_read];	//发一个字节
			if(++TX3_read >= RX3_Length)	TX3_read = 0;	//避免溢出处理
		}

		if((TX4_read != RX4_write) && !B_TX4_Busy)	//收到过数据, 并且发送空闲
		{
			B_TX4_Busy = 1;		//标志发送忙
			S4BUF = RX4_Buffer[TX4_read];	//发一个字节
			if(++TX4_read >= RX4_Length)	TX4_read = 0;	//避免溢出处理
		}
	}
}


//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	SetTimer2Baudraye(u16 dat)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = dat / 256;
	TL2 = dat % 256;
	IE2  &= ~(1<<2);	//禁止中断
	AUXR |=  (1<<4);	//Timer run enable
}


//========================================================================
// 函数: void	UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	UART1_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
	u8	i;
	/*********** 波特率使用定时器2 *****************/
	if(brt == 2)
	{
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate1);
	}

	/*********** 波特率使用定时器1 *****************/
	else
	{
		TR1 = 0;
		AUXR &= ~0x01;		//S1 BRT Use Timer1;
		AUXR |=  (1<<6);	//Timer1 set as 1T mode
		TMOD &= ~(1<<6);	//Timer1 set As Timer
		TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		TH1 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate1) / 256;
		TL1 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate1) % 256;
		ET1 = 0;	//禁止中断
		INT_CLKO &= ~0x02;	//不输出时钟
		TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | (1<<6);	// 8位数据, 1位起始位, 1位停止位, 无校验
//	PS  = 1;	//高优先级中断
	ES  = 1;	//允许中断
	REN = 1;	//允许接收
	P_SW1 = P_SW1 & 0x3f;	//切换到 P3.0 P3.1
//	P_SW1 = (P_SW1 & 0x3f) | (1<<6);	//切换到P3.6 P3.7
//	P_SW1 = (P_SW1 & 0x3f) | (2<<6);	//切换到P1.6 P1.7 (必须使用内部时钟)

	for(i=0; i<RX1_Length; i++)		RX1_Buffer[i] = 0;
	B_TX1_Busy  = 0;
	TX1_read    = 0;
	RX1_write   = 0;
}


//========================================================================
// 函数: void	UART2_config(u8 brt)
// 描述: UART2初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	UART2_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
	u8	i;
	/*********** 波特率固定使用定时器2 *****************/
	if(brt == 2)	SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate2);

	S2CON &= ~(1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
	IE2   |= 1;			//允许中断
	S2CON |= (1<<4);	//允许接收
	P_SW2 &= ~1;		//切换到 P1.0 P1.1
//	P_SW2 |= 1;			//切换到 P4.6 P4.7

	for(i=0; i<RX2_Length; i++)		RX2_Buffer[i] = 0;
	B_TX2_Busy  = 0;
	TX2_read    = 0;
	RX2_write   = 0;
}

//========================================================================
// 函数: void	UART3_config(u8 brt)
// 描述: UART3初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	UART3_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
{
	u8	i;
	/*********** 波特率固定使用定时器2 *****************/
	if(brt == 2)
	{
		S3CON &= ~(1<<6);	//BRT select Timer2
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate3);
	}
	/*********** 波特率使用定时器3 *****************/
	else
	{
		S3CON |= (1<<6);	//BRT select Timer3
		T4T3M &= 0xf0;		//停止计数, 清除控制位
		IE2  &= ~(1<<5);	//禁止中断
		T4T3M |=  (1<<1);	//1T
		T4T3M &= ~(1<<2);	//定时
		T4T3M &= ~1;		//不输出时钟
		TH3 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate3) / 256;
		TL3 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate3) % 256;
		T4T3M |=  (1<<3);	//开始运行
	}
	
	S3CON &= ~(1<<5);	//禁止多机通讯方式
	S3CON &= ~(1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
	IE2   |=  (1<<3);	//允许中断
	S3CON |=  (1<<4);	//允许接收
	P_SW2 &= ~2;		//切换到 P0.0 P0.1
//	P_SW2 |= 2;			//切换到 P5.0 P5.1

	for(i=0; i<RX3_Length; i++)		RX3_Buffer[i] = 0;
	B_TX3_Busy  = 0;
	TX3_read    = 0;
	RX3_write   = 0;
}

//========================================================================
// 函数: void	UART4_config(u8 brt)
// 描述: UART4初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	UART4_config(u8 brt)	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
{
	u8	i;
	/*********** 波特率固定使用定时器2 *****************/
	if(brt == 2)
	{
		S4CON &= ~(1<<6);	//BRT select Timer2
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / UART_BaudRate4);
	}
	/*********** 波特率使用定时器3 *****************/
	else
	{
		S4CON |= (1<<6);	//BRT select Timer4
		T4T3M &= 0x0f;		//停止计数, 清除控制位
		IE2   &= ~(1<<6);	//禁止中断
		T4T3M |=  (1<<5);	//1T
		T4T3M &= ~(1<<6);	//定时
		T4T3M &= ~(1<<4);	//不输出时钟
		TH4 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate4) / 256;
		TL4 = (65536UL - (MAIN_Fosc / 4) / UART_BaudRate4) % 256;
		T4T3M |=  (1<<7);	//开始运行
	}
	
	S4CON &= ~(1<<5);	//禁止多机通讯方式
	S4CON &= ~(1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
	IE2   |=  (1<<4);	//允许中断
	S4CON |=  (1<<4);	//允许接收
	P_SW2 &= ~4;		//切换到 P0.2 P0.3
//	P_SW2 |= 4;			//切换到 P5.2 P5.3

	for(i=0; i<RX4_Length; i++)		RX4_Buffer[i] = 0;
	B_TX4_Busy  = 0;
	TX4_read    = 0;
	RX4_write   = 0;
}


void PrintString1(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX1_Busy = 1;		//标志发送忙
		SBUF = *puts;		//发一个字节
		while(B_TX1_Busy);	//等待发送完成
	}
}

void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX2_Busy = 1;		//标志发送忙
		S2BUF = *puts;		//发一个字节
		while(B_TX2_Busy);	//等待发送完成
	}
}

void PrintString3(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX3_Busy = 1;		//标志发送忙
		S3BUF = *puts;		//发一个字节
		while(B_TX3_Busy);	//等待发送完成
	}
}

void PrintString4(u8 *puts)
{
    for (; *puts != 0;	puts++)
	{
		B_TX4_Busy = 1;		//标志发送忙
		S4BUF = *puts;		//发一个字节
		while(B_TX4_Busy);	//等待发送完成
	}
}



/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		RX1_Buffer[RX1_write] = SBUF;
		if(++RX1_write >= RX1_Length)	RX1_write = 0;
	}

	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;
	}
}

/********************* UART2中断函数************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if(RI2)
	{
		CLR_RI2();
		RX2_Buffer[RX2_write] = S2BUF;
		if(++RX2_write >= RX2_Length)	RX2_write = 0;
	}

	if(TI2)
	{
		CLR_TI2();
		B_TX2_Busy = 0;
	}

}

/********************* UART3中断函数************************/
void UART3_int (void) interrupt UART3_VECTOR
{
	if(RI3)
	{
		CLR_RI3();
		RX3_Buffer[RX3_write] = S3BUF;
		if(++RX3_write >= RX3_Length)	RX3_write = 0;
	}

	if(TI3)
	{
		CLR_TI3();
		B_TX3_Busy = 0;
	}

}

/********************* UART4中断函数************************/
void UART4_int (void) interrupt UART4_VECTOR
{
	if(RI4)
	{
		CLR_RI4();
		RX4_Buffer[RX4_write] = S4BUF;
		if(++RX4_write >= RX4_Length)	RX4_write = 0;
	}

	if(TI4)
	{
		CLR_TI4();
		B_TX4_Busy = 0;
	}

}



