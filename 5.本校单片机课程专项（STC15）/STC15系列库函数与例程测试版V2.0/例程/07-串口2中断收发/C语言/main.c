
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


/*************	功能说明	**************

串口2全双工中断方式收发通讯程序。本例程使用22.1184MHZ时钟，如要改变，请修改下面的"定义主时钟"的值并重新编译。

串口设置为：115200,8,n,1.

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/

/*************	本地常量声明	**************/
#define MAIN_Fosc		22118400L	//定义主时钟
#define	RX2_Lenth		32			//串口接收缓冲长度
#define	BaudRate2		115200UL	//选择波特率


#define	Timer2_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate2))		//Timer 2 重装值， 对应300KHZ

#include	"STC15Fxxxx.H"


/*************	本地变量声明	**************/
u8	idata RX2_Buffer[RX2_Lenth];	//接收缓冲
u8	TX2_Cnt;	//发送计数
u8	RX2_Cnt;	//接收计数
bit	B_TX2_Busy;	//发送忙标志


/*************	本地函数声明	**************/



/**********************************************/
void main(void)
{
	B_TX2_Busy = 0;
	RX2_Cnt = 0;
	TX2_Cnt = 0;

	S2CON &= ~(1<<7);	//8位数据
	P_SW2 &= ~1;		//UART2 使用P1.0 P1.1口	默认
//	P_SW2 |=  1;		//UART2 使用P4.6 P4.7口

	AUXR &= ~(1<<4);	//Timer stop
	AUXR &= ~(1<<3);	//Timer2 set As Timer
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = (u8)(Timer2_Reload >> 8);
	TL2 = (u8)Timer2_Reload;
	IE2   |=  1;		//允许中断
	S2CON |=  (1<<4);	//允许接收
	AUXR |=  (1<<4);	//Timer run enable

	EA = 1;		//允许全局中断
	

	while (1)
	{
		if(TX2_Cnt != RX2_Cnt)		//收到过数据
		{
			if(!B_TX2_Busy)		//发送空闲
			{
				B_TX2_Busy = 1;		//标志发送忙
				S2BUF = RX2_Buffer[TX2_Cnt];	//发一个字节
				if(++TX2_Cnt >= RX2_Lenth)	TX2_Cnt = 0;	//避免溢出处理
			}
		}
	}
}

/********************* UART1中断函数************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if((S2CON & 1) != 0)
	{
		S2CON &= ~1;
		RX2_Buffer[RX2_Cnt] = S2BUF;		//保存一个字节
		if(++RX2_Cnt >= RX2_Lenth)	RX2_Cnt = 0;	//避免溢出处理
	}

	if((S2CON & 2) != 0)
	{
		S2CON &= ~2;
		B_TX2_Busy = 0;		//清除发送忙标志
	}
}



