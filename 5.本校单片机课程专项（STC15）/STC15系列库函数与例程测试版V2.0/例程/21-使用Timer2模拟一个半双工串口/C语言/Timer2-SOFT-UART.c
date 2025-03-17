/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-755-82905966 -------------------------------------------*/
/* --- Tel: 86-755-82948412 -------------------------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

使用STC15系列的Timer2做的模拟串口. P3.0接收, P3.1发送, 半双工.

假定测试芯片的工作频率为22118400Hz. 时钟为5.5296MHZ ~ 35MHZ. 

波特率高，则时钟也要选高, 优先使用 22.1184MHZ, 11.0592MHZ.

测试方法: 上位机发送数据, MCU收到数据后原样返回.

串口固定设置: 1位起始位, 8位数据位, 1位停止位,  波特率在范围如下.

1200 ~ 115200 bps @ 33.1776MHZ
 600 ~ 115200 bps @ 22.1184MHZ
 600 ~  76800 bps @ 18.4320MHZ
 300 ~  57600 bps @ 11.0592MHZ
 150 ~  19200 bps @  5.5296MHZ

******************************************/

#include	<reg52.h>

#define MAIN_Fosc		22118400UL	//定义主时钟
#define	UART3_Baudrate	115200UL	//定义波特率
#define	RX_Lenth		32			//接收长度

#define	UART3_BitTime	(MAIN_Fosc / UART3_Baudrate)

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

sfr IE2  = 0xAF;
sfr	AUXR = 0x8E;
sfr INT_CLKO = 0x8F;
sfr	T2H  = 0xD6;
sfr	T2L  = 0xD7;

u8	Tx3_read;			//发送读指针
u8	Rx3_write;			//接收写指针
u8 	idata	buf3[RX_Lenth];	//接收缓冲

u16	RxTimeOut;
bit	B_RxOk;		//接收结束标志


//===================== 模拟串口相关===========================
sbit P_RX3 = P3^0;	//定义模拟串口接收IO
sbit P_TX3 = P3^1;	//定义模拟串口发送IO

u8  Tx3_DAT;		// 发送移位变量, 用户不可见
u8  Rx3_DAT;		// 接收移位变量, 用户不可见
u8  Tx3_BitCnt;		// 发送数据的位计数器, 用户不可见
u8  Rx3_BitCnt;		// 接收数据的位计数器, 用户不可见
u8	Rx3_BUF;		// 接收到的字节, 用户读取
u8	Tx3_BUF;		// 要发送的字节, 用户写入
bit	Rx3_Ring;		// 正在接收标志, 低层程序使用, 用户程序不可见
bit	Tx3_Ting;		// 正在发送标志, 用户置1请求发送, 底层发送完成清0
bit	RX3_End;		// 接收到一个字节, 用户查询 并清0
//=============================================================

void	UART_Init(void);


/******************** 主函数 **************************/
void main(void)
{
	
	UART_Init();	//PCA初始化
	EA = 1;
	
	while (1)		//user's function
	{
		if (RX3_End)		// 检测是否收到一个字节
		{
			RX3_End = 0;	// 清除标志
			buf3[Rx3_write] = Rx3_BUF;	// 写入缓冲
			if(++Rx3_write >= RX_Lenth)	Rx3_write = 0;	// 指向下一个位置,  溢出检测
			RxTimeOut = 1000;	//装载超时时间
		}
		if(RxTimeOut != 0)		// 超时时间是否非0?
		{
			if(--RxTimeOut == 0)	// (超时时间  - 1) == 0?
			{
				B_RxOk = 1;
				AUXR &=  ~(1<<4);	//Timer2 停止运行
				INT_CLKO &= ~(1 << 6);	//禁止INT4中断
				T2H = (65536 - UART3_BitTime) / 256;	//数据位
				T2L = (65536 - UART3_BitTime) % 256;	//数据位
				AUXR |=  (1<<4);	//Timer2 开始运行
			}
		}
		
		if(B_RxOk)		// 检测是否接收OK?
		{
			if (!Tx3_Ting)		// 检测是否发送空闲
			{
				if (Tx3_read != Rx3_write)	// 检测是否收到过字符
				{
					Tx3_BUF = buf3[Tx3_read];	// 从缓冲读一个字符发送
					Tx3_Ting = 1;				// 设置发送标志
					if(++Tx3_read >= RX_Lenth)	Tx3_read = 0;	// 指向下一个位置,  溢出检测
				}
				else
				{
					B_RxOk = 0;
					AUXR &=  ~(1<<4);		//Timer2 停止运行
					INT_CLKO |=  (1 << 6);	//允许INT4中断
				}
			}
		}
	}
}


//========================================================================
// 函数: void	UART_Init(void)
// 描述: UART初始化程序.
// 参数: none
// 返回: none.
// 版本: V1.0, 2013-11-22
//========================================================================
void	UART_Init(void)
{
	Tx3_read  = 0;
	Rx3_write = 0;
	Tx3_Ting  = 0;
	Rx3_Ring  = 0;
	RX3_End   = 0;
	Tx3_BitCnt = 0;
	RxTimeOut = 0;
	B_RxOk = 0;

	AUXR &=  ~(1<<4);		// Timer2 停止运行
	T2H = (65536 - UART3_BitTime) / 256;	// 数据位
	T2L = (65536 - UART3_BitTime) % 256;	// 数据位
	INT_CLKO |=  (1 << 6);	// 允许INT4中断
	IE2  |=  (1<<2);		// 允许Timer2中断
	AUXR |=  (1<<2);		// 1T
}
//======================================================================

//========================================================================
// 函数: void	timer2_int (void) interrupt 12
// 描述: Timer2中断处理程序.
// 参数: None
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	timer2_int (void) interrupt 12
{
	if(Rx3_Ring)		//已收到起始位
	{
		if (--Rx3_BitCnt == 0)		//接收完一帧数据
		{
			Rx3_Ring = 0;			//停止接收
			Rx3_BUF = Rx3_DAT;		//存储数据到缓冲区
			RX3_End = 1;
			AUXR &=  ~(1<<4);	//Timer2 停止运行
			INT_CLKO |=  (1 << 6);	//允许INT4中断
		}
		else
		{
			Rx3_DAT >>= 1;			  		//把接收的单b数据 暂存到 RxShiftReg(接收缓冲)
			if(P_RX3) Rx3_DAT |= 0x80;  	//shift RX data to RX buffer
		}
	}

	if(Tx3_Ting)					// 不发送, 退出
	{
		if(Tx3_BitCnt == 0)			//发送计数器为0 表明单字节发送还没开始
		{
			P_TX3 = 0;				//发送开始位
			Tx3_DAT = Tx3_BUF;		//把缓冲的数据放到发送的buff
			Tx3_BitCnt = 9;			//发送数据位数 (8数据位+1停止位)
		}
		else						//发送计数器为非0 正在发送数据
		{
			if (--Tx3_BitCnt == 0)	//发送计数器减为0 表明单字节发送结束
			{
				P_TX3 = 1;			//送停止位数据
				Tx3_Ting = 0;		//发送停止
			}
			else
			{
				Tx3_DAT >>= 1;		//把最低位送到 CY(益处标志位)
				P_TX3 = CY;			//发送一个bit数据
			}
		}
	}
}


/********************* INT4中断函数 *************************/
void Ext_INT4 (void) interrupt 16
{
	AUXR &=  ~(1<<4);	//Timer2 停止运行
	T2H = (65536 - (UART3_BitTime / 2 + UART3_BitTime)) / 256;	//起始位 + 半个数据位
	T2L = (65536 - (UART3_BitTime / 2 + UART3_BitTime)) % 256;	//起始位 + 半个数据位
	AUXR |=  (1<<4);	//Timer2 开始运行
	Rx3_Ring = 1;		//标志已收到起始位
	Rx3_BitCnt = 9;		//初始化接收的数据位数(8个数据位+1个停止位)
	
	INT_CLKO &= ~(1 << 6);	//禁止INT4中断
	T2H = (65536 - UART3_BitTime) / 256;	//数据位
	T2L = (65536 - UART3_BitTime) % 256;	//数据位
}




