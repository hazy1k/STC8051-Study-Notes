
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


#include "USART.h"


COMx_Define	COM1,COM2,COM3,COM4;
u8 	xdata RX1_Buffer[RX_Length];	//接收缓冲
u8 	xdata RX2_Buffer[RX_Length];	//接收缓冲
u8 	xdata RX3_Buffer[RX_Length];	//接收缓冲
u8 	xdata RX4_Buffer[RX_Length];	//接收缓冲


//========================================================================
// 函数: void	InitUartValue(COMx_Define *COMx)
// 描述: 串口初始化变量函数。
// 参数: *COMx: 初始化参数指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void	InitUartValue(COMx_Define *COMx)
{
	u8	i;
	COMx->TX_read  = 0;
	COMx->TX_Busy  = 0;
	COMx->RX_write = 0;
	for(i=0; i<RX_Length; i++)	COMx->RX_Buffer[i] = 0;
}


//========================================================================
// 函数: u8 USART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
// 描述: 串口初始化函数。
// 参数: UARTx: 要初始化的串口, 取值为 USART1, USART2, USART3, USART4.
//       *COMx: 初始化参数指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
u8 USART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
{
	u32	j;
	
	if(UARTx == USART1)
	{
		COM1.id = 1;
		COM1.RX_Buffer = RX1_Buffer;
		InitUartValue(&COM1);

		if(COMx->UART_Mode > UART_9bit_BRTx)	return 2;	//模式错误
	//	if(COMx->UART_Polity == PolityHigh)		PS = 1;	//高优先级中断
	//	else									PS = 0;	//低优先级中断
		SCON = (SCON & 0x3f) | COMx->UART_Mode;
		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			if(COMx->UART_BRT_Use == BRT_Timer1)
			{
				TR1 = 0;
				AUXR &= ~0x01;		//S1 BRT Use Timer1;
				TMOD &= ~(1<<6);	//Timer1 set As Timer
				TMOD &= ~0x30;		//Timer1_16bitAutoReload;
				AUXR |=  (1<<6);	//Timer1 set as 1T mode
				TH1 = (u8)(j>>8);
				TL1 = (u8)j;
				ET1 = 0;	//禁止中断
				INT_CLKO &= ~0x02;	//不输出时钟
				TR1  = 1;
			}
			else if(COMx->UART_BRT_Use == BRT_Timer2)
			{
				AUXR &= ~(1<<4);	//Timer stop
				AUXR |= 0x01;		//S1 BRT Use Timer2;
				AUXR &= ~(1<<3);	//Timer2 set As Timer
				AUXR |=  (1<<2);	//Timer2 set as 1T mode
				TH2 = (u8)(j>>8);
				TL2 = (u8)j;
				IE2  &= ~(1<<2);	//禁止中断
				AUXR |=  (1<<4);	//Timer run enable
			}
			else return 2;	//错误
		}
		else if(COMx->UART_Mode == UART_ShiftRight)
		{
			AUXR &= ~(1<<5);	//固定波特率SysClk/12
		}
		else if(COMx->UART_Mode == UART_9bit)	//固定波特率SysClk*2^SMOD/64
		{
			PCON &= ~(1<<7);	//固定波特率SysClk/64
		}
		if(COMx->UART_Interrupt == ENABLE)	ES = 1;	//允许中断
		else								ES = 0;	//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	REN = 1;	//允许接收
		else								REN = 0;	//禁止接收
		P_SW1 = (P_SW1 & 0x3f) | (COMx->UART_P_SW & 0xc0);	//切换IO
		return	0;
	}

	if(UARTx == USART2)
	{
		COM2.id = 2;
		COM2.RX_Buffer = RX2_Buffer;
		InitUartValue(&COM2);

		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			if(COMx->UART_Mode == UART_9bit_BRTx)	S2CON |=  (1<<7);	//9bit
			else									S2CON &= ~(1<<7);	//8bit
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			AUXR &= ~(1<<4);	//Timer stop
			AUXR &= ~(1<<3);	//Timer2 set As Timer
			AUXR |=  (1<<2);	//Timer2 set as 1T mode
			TH2 = (u8)(j>>8);
			TL2 = (u8)j;
			IE2  &= ~(1<<2);	//禁止中断
			AUXR |=  (1<<4);	//Timer run enable
		}
		else	return 2;	//模式错误
		if(COMx->UART_Interrupt == ENABLE)	IE2   |=  1;		//允许中断
		else								IE2   &= ~1;		//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	S2CON |=  (1<<4);	//允许接收
		else								S2CON &= ~(1<<4);	//禁止接收
		P_SW2 = (P_SW2 & ~1) | (COMx->UART_P_SW & 0x01);	//切换IO
		return	0;
	}

	if(UARTx == USART3)
	{
		COM3.id = 3;
		COM3.RX_Buffer = RX3_Buffer;
		InitUartValue(&COM3);

		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			S3CON &= ~(1<<5);	//禁止多机通讯方式
			if(COMx->UART_Mode == UART_9bit_BRTx)	S3CON |=  (1<<7);	//9bit
			else									S3CON &= ~(1<<7);	//8bit
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			if(COMx->UART_BRT_Use == BRT_Timer2)
			{
				S3CON &= ~(1<<6);	//BRT select Timer2
				AUXR &= ~(1<<4);	//Timer stop
				AUXR &= ~(1<<3);	//Timer2 set As Timer
				AUXR |=  (1<<2);	//Timer2 set as 1T mode
				TH2 = (u8)(j>>8);
				TL2 = (u8)j;
				IE2  &= ~(1<<2);	//禁止中断
				AUXR |=  (1<<4);	//Timer run enable
			}
			else if(COMx->UART_BRT_Use == BRT_Timer3)
			{
				S3CON |= (1<<6);	//BRT select Timer3
				T4T3M &= 0xf0;		//停止计数, 清除控制位
				IE2  &= ~(1<<5);	//禁止中断
				T4T3M |=  (1<<1);	//1T
				T4T3M &= ~(1<<2);	//定时
				T4T3M &= ~1;		//不输出时钟
				TH3 = (u8)(j >> 8);
				TL3 = (u8)j;
				T4T3M |=  (1<<3);	//开始运行
			}
			else	return 2;
		}
		else	return 2;	//模式错误
		if(COMx->UART_Interrupt == ENABLE)	IE2   |=  (1<<3);	//允许中断
		else								IE2   &= ~(1<<3);	//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	S3CON |=  (1<<4);	//允许接收
		else								S3CON &= ~(1<<4);	//禁止接收
		P_SW2 = (P_SW2 & ~2) | (COMx->UART_P_SW & 0x02);	//切换IO
		return	0;
	}

	if(UARTx == USART4)
	{
		COM4.id = 4;
		COM4.RX_Buffer = RX4_Buffer;
		InitUartValue(&COM4);

		if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))	//可变波特率
		{
			S4CON &= ~(1<<5);	//禁止多机通讯方式
			if(COMx->UART_Mode == UART_9bit_BRTx)	S4CON |=  (1<<7);	//9bit
			else									S4CON &= ~(1<<7);	//8bit
			j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;	//按1T计算
			if(j >= 65536UL)	return 2;	//错误
			j = 65536UL - j;
			if(COMx->UART_BRT_Use == BRT_Timer2)
			{
				S4CON &= ~(1<<6);	//BRT select Timer2
				AUXR &= ~(1<<4);	//Timer stop
				AUXR &= ~(1<<3);	//Timer2 set As Timer
				AUXR |=  (1<<2);	//Timer2 set as 1T mode
				TH2 = (u8)(j>>8);
				TL2 = (u8)j;
				IE2  &= ~(1<<2);	//禁止中断
				AUXR |=  (1<<4);	//Timer run enable
			}
			else if(COMx->UART_BRT_Use == BRT_Timer4)
			{
				S4CON |= (1<<6);	//BRT select Timer4
				T4T3M &= 0x0f;		//停止计数, 清除控制位
				IE2   &= ~(1<<6);	//禁止中断
				T4T3M |=  (1<<5);	//1T
				T4T3M &= ~(1<<6);	//定时
				T4T3M &= ~(1<<4);	//不输出时钟
				TH4 = (u8)(j >> 8);
				TL4 = (u8)j;
				T4T3M |=  (1<<7);	//开始运行
			}
			else	return 2;
		}
		else	return 2;	//模式错误
		if(COMx->UART_Interrupt == ENABLE)	IE2   |=  (1<<4);	//允许中断
		else								IE2   &= ~(1<<4);	//禁止中断
		if(COMx->UART_RxEnable == ENABLE)	S4CON |=  (1<<4);	//允许接收
		else								S4CON &= ~(1<<4);	//禁止接收
		P_SW2 = (P_SW2 & ~4) | (COMx->UART_P_SW & 0x04);	//切换IO
		return	0;
	}

	return	3;	//其它错误
}


//========================================================================
// 函数: void PrintString(u8 UARTx, u8 *puts)
// 描述: 字符串发送函数。
// 参数: UARTx: 要发往的串口, 取值为 USART1, USART2, USART3, USART4.
//       *puts: 要发送的字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString(u8 UARTx, u8 *puts)
{
	for (; *puts != 0;	puts++)
	{
		if(UARTx == USART1)
		{
			COM1.TX_Busy = 1;			//标志发送忙
			SBUF = *puts;				//发一个字节
			while(COM1.TX_Busy > 0);	//等待发送完成
		}
		else if(UARTx == USART2)
		{
			COM2.TX_Busy = 1;			//标志发送忙
			S2BUF = *puts;				//发一个字节
			while(COM2.TX_Busy > 0);	//等待发送完成
		}
		else if(UARTx == USART3)
		{
			COM3.TX_Busy = 1;			//标志发送忙
			S3BUF = *puts;				//发一个字节
			while(COM3.TX_Busy > 0);	//等待发送完成
		}
		else if(UARTx == USART4)
		{
			COM4.TX_Busy = 1;			//标志发送忙
			S4BUF = *puts;				//发一个字节
			while(COM4.TX_Busy > 0);	//等待发送完成
		}
	}
}



/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		RX1_Buffer[COM1.RX_write] = SBUF;
		if(++COM1.RX_write >= RX_Length)	COM1.RX_write = 0;
	}

	if(TI)
	{
		TI = 0;
		COM1.TX_Busy = 0;
	}
}

/********************* UART2中断函数************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if(RI2)
	{
		CLR_RI2();
		RX2_Buffer[COM2.RX_write] = S2BUF;
		if(++COM2.RX_write >= RX_Length)	COM2.RX_write = 0;
	}

	if(TI2)
	{
		CLR_TI2();
		COM2.TX_Busy = 0;
	}

}

/********************* UART3中断函数************************/
void UART3_int (void) interrupt UART3_VECTOR
{
	if(RI3)
	{
		CLR_RI3();
		RX3_Buffer[COM3.RX_write] = S3BUF;
		if(++COM3.RX_write >= RX_Length)	COM3.RX_write = 0;
	}

	if(TI3)
	{
		CLR_TI3();
		COM3.TX_Busy = 0;
	}

}

/********************* UART4中断函数************************/
void UART4_int (void) interrupt UART4_VECTOR
{
	if(RI4)
	{
		CLR_RI4();
		RX4_Buffer[COM4.RX_write] = S4BUF;
		if(++COM4.RX_write >= RX_Length)	COM4.RX_write = 0;
	}

	if(TI4)
	{
		CLR_TI4();
		COM4.TX_Busy = 0;
	}

}



