
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

串口1全双工中断方式收发通讯程序。本例程使用22.1184MHZ时钟，如要改变，请修改下面的"定义主时钟"的值并重新编译。

串口设置为：115200,8,n,1.

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/

/*************	本地常量声明	**************/
#define MAIN_Fosc		22118400L	//定义主时钟
#define	RX1_Lenth		32			//串口接收缓冲长度
#define	BaudRate1		115200UL	//选择波特率


#define	Timer1_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 1 重装值， 对应300KHZ
#define	Timer2_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 2 重装值， 对应300KHZ

#include	"STC15Fxxxx.H"


/*************	本地变量声明	**************/
u8	idata RX1_Buffer[RX1_Lenth];	//接收缓冲
u8	TX1_Cnt;	//发送计数
u8	RX1_Cnt;	//接收计数
bit	B_TX1_Busy;	//发送忙标志


/*************	本地函数声明	**************/



/**********************************************/
void main(void)
{
	B_TX1_Busy = 0;
	RX1_Cnt = 0;
	TX1_Cnt = 0;

	S1_8bit();				//8位数据
	S1_USE_P30P31();		//UART1 使用P30 P31口	默认
//	S1_USE_P36P37();		//UART1 使用P36 P37口
//	S1_USE_P16P17();		//UART1 使用P16 P17口

/*
	TR1 = 0;			//波特率使用Timer1产生
	AUXR &= ~0x01;		//S1 BRT Use Timer1;
	AUXR |=  (1<<6);	//Timer1 set as 1T mode
	TH1 = (u8)(Timer1_Reload >> 8);
	TL1 = (u8)Timer1_Reload;
	TR1  = 1;
*/

	AUXR &= ~(1<<4);	//Timer stop		波特率使用Timer2产生
	AUXR |= 0x01;		//S1 BRT Use Timer2;
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = (u8)(Timer2_Reload >> 8);
	TL2 = (u8)Timer2_Reload;
	AUXR |=  (1<<4);	//Timer run enable

	REN = 1;	//允许接收
	ES  = 1;	//允许中断

	EA = 1;		//允许全局中断
	

	while (1)
	{
		if(TX1_Cnt != RX1_Cnt)		//收到过数据
		{
			if(!B_TX1_Busy)		//发送空闲
			{
				B_TX1_Busy = 1;		//标志发送忙
				SBUF = RX1_Buffer[TX1_Cnt];	//发一个字节
				if(++TX1_Cnt >= RX1_Lenth)	TX1_Cnt = 0;	//避免溢出处理
			}
		}
	}
}

/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		RX1_Buffer[RX1_Cnt] = SBUF;		//保存一个字节
		if(++RX1_Cnt >= RX1_Lenth)	RX1_Cnt = 0;	//避免溢出处理
	}

	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;		//清除发送忙标志
	}
}



