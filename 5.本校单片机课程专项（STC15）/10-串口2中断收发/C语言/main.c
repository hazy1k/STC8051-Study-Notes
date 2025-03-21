
/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/


#define MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"

#define		Baudrate2	115200L



/*************	功能说明	**************

双串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/


#define	UART2_BUF_LENGTH	32



u8	TX2_Cnt;	//发送计数
u8	RX2_Cnt;	//接收计数
bit	B_TX2_Busy;	//发送忙标志

u8 	idata RX2_Buffer[UART2_BUF_LENGTH];	//接收缓冲


void	UART2_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void 	PrintString2(u8 *puts);



//========================================================================
// 函数: void main(void)
// 描述: 主函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void main(void)
{
	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口


	UART2_config(2);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
	EA = 1;	//允许全局中断
	
	PrintString2("STC15F2K60S2 UART2 Test Prgramme!\r\n");	//SUART2发送一个字符串

	while (1)
	{
		if((TX2_Cnt != RX2_Cnt) && (!B_TX2_Busy))	//收到数据, 发送空闲
		{
			S2BUF = RX2_Buffer[TX2_Cnt];
			B_TX2_Busy = 1;
			if(++TX2_Cnt >= UART2_BUF_LENGTH)	TX2_Cnt = 0;
		}
	}
}


//========================================================================
// 函数: void PrintString2(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)   	//遇到停止符0结束
	{
		S2BUF = *puts;
		B_TX2_Busy = 1;
		while(B_TX2_Busy);
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
	/*********** 波特率固定使用定时器2 *****************/
	if(brt == 2)
	{
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

		S2CON &= ~(1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
		IE2   |= 1;			//允许中断
		S2CON |= (1<<4);	//允许接收
		P_SW2 &= ~0x01;	
		P_SW2 |= 1;			//UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7

		B_TX2_Busy = 0;
		TX2_Cnt = 0;
		RX2_Cnt = 0;
	}
}


//========================================================================
// 函数: void UART2_int (void) interrupt UART2_VECTOR
// 描述: UART2中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_int (void) interrupt UART2_VECTOR
{
	if((S2CON & 1) != 0)
	{
		S2CON &= ~1;	//Clear Rx flag
		RX2_Buffer[RX2_Cnt] = S2BUF;
		if(++RX2_Cnt >= UART2_BUF_LENGTH)	RX2_Cnt = 0;
	}

	if((S2CON & 2) != 0)
	{
		S2CON &= ~2;	//Clear Tx flag
		B_TX2_Busy = 0;
	}

}


