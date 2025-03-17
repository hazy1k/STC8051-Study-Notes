/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-755-82905966 -------------------------------------------*/
/* --- Tel: 86-755-82948412 -------------------------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

使用STC15系列的PCA0和PCA1做的模拟串口. PCA0接收(P2.5), PCA1发送(P2.6).

假定测试芯片的工作频率为22118400Hz. 时钟为5.5296MHZ ~ 35MHZ. 

波特率高，则时钟也要选高, 优先使用 22.1184MHZ, 11.0592MHZ.

测试方法: 上位机发送数据,MCU收到数据后原样返回.

串口固定设置: 1位起始位, 8位数据位, 1位停止位,  波特率在 600 ~ 57600 bps.

1200 ~ 57600 bps @ 33.1776MHZ
 600 ~ 57600 bps @ 22.1184MHZ
 600 ~ 38400 bps @ 18.4320MHZ
 300 ~ 28800 bps @ 11.0592MHZ
 150 ~ 14400 bps @  5.5296MHZ

******************************************/

#include	<reg52.h>

#define MAIN_Fosc		22118400UL	//定义主时钟
#define	UART3_Baudrate	57600UL		//定义波特率
#define	RX_Lenth		16			//接收长度

#define	PCA_P12_P11_P10_P37	(0<<4)
#define	PCA_P34_P35_P36_P37	(1<<4)
#define	PCA_P24_P25_P26_P27	(2<<4)
#define	PCA_Mode_Capture	0
#define	PCA_Mode_SoftTimer	0x48
#define	PCA_Clock_1T		(4<<1)
#define	PCA_Clock_2T		(1<<1)
#define	PCA_Clock_4T		(5<<1)
#define	PCA_Clock_6T		(6<<1)
#define	PCA_Clock_8T		(7<<1)
#define	PCA_Clock_12T		(0<<1)
#define	PCA_Clock_ECI		(3<<1)
#define	PCA_Rise_Active		(1<<5)
#define	PCA_Fall_Active		(1<<4)
#define	PCA_PWM_8bit		(0<<6)
#define	PCA_PWM_7bit		(1<<6)
#define	PCA_PWM_6bit		(2<<6)

#define	UART3_BitTime	(MAIN_Fosc / UART3_Baudrate)

#define		ENABLE		1
#define		DISABLE		0

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

sfr AUXR1 = 0xA2;
sfr CCON = 0xD8;
sfr CMOD = 0xD9;
sfr CCAPM0 = 0xDA;		//PCA模块0的工作模式寄存器。
sfr CCAPM1 = 0xDB;		//PCA模块1的工作模式寄存器。
sfr CCAPM2 = 0xDC;		//PCA模块2的工作模式寄存器。

sfr CL     = 0xE9;
sfr CCAP0L = 0xEA;		//PCA模块0的捕捉/比较寄存器低8位。
sfr CCAP1L = 0xEB;		//PCA模块1的捕捉/比较寄存器低8位。
sfr CCAP2L = 0xEC;		//PCA模块2的捕捉/比较寄存器低8位。

sfr CH     = 0xF9;
sfr CCAP0H = 0xFA;		//PCA模块0的捕捉/比较寄存器高8位。
sfr CCAP1H = 0xFB;		//PCA模块1的捕捉/比较寄存器高8位。
sfr CCAP2H = 0xFC;		//PCA模块2的捕捉/比较寄存器高8位。

sbit CCF0  = CCON^0;	//PCA 模块0中断标志，由硬件置位，必须由软件清0。
sbit CCF1  = CCON^1;	//PCA 模块1中断标志，由硬件置位，必须由软件清0。
sbit CCF2  = CCON^2;	//PCA 模块2中断标志，由硬件置位，必须由软件清0。
sbit CR    = CCON^6;	//1: 允许PCA计数器计数，0: 禁止计数。
sbit CF    = CCON^7;	//PCA计数器溢出（CH，CL由FFFFH变为0000H）标志。PCA计数器溢出后由硬件置位，必须由软件清0。
sbit PPCA  = IP^7;		//PCA 中断 优先级设定位

u16		CCAP0_tmp;
u16		CCAP1_tmp;

u8	Tx3_read;			//发送读指针
u8	Rx3_write;			//接收写指针
u8 	idata	buf3[RX_Lenth];	//接收缓冲

//===================== 模拟串口相关===========================
sbit P_RX3 = P2^5;	//定义模拟串口接收IO
sbit P_TX3 = P2^6;	//定义模拟串口发送IO

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

void	PCA_Init(void);


/******************** 主函数 **************************/
void main(void)
{
	
	PCA_Init();	//PCA初始化
	EA = 1;
	
	Tx3_read  = 0;
	Rx3_write = 0;
	Tx3_Ting  = 0;
	Rx3_Ring  = 0;
	RX3_End   = 0;
	Tx3_BitCnt = 0;

	
	while (1)		//user's function
	{
		if (RX3_End)		// 检测是否收到一个字节
		{
			RX3_End = 0;	// 清除标志
			buf3[Rx3_write] = Rx3_BUF;	// 写入缓冲
			if(++Rx3_write >= RX_Lenth)	Rx3_write = 0;	// 指向下一个位置,  溢出检测
		}
		if (!Tx3_Ting)		// 检测是否发送空闲
		{
			if (Tx3_read != Rx3_write)	// 检测是否收到过字符
			{
				Tx3_BUF = buf3[Tx3_read];	// 从缓冲读一个字符发送
				Tx3_Ting = 1;				// 设置发送标志
				if(++Tx3_read >= RX_Lenth)	Tx3_read = 0;	// 指向下一个位置,  溢出检测
			}
		}
	}
}


//========================================================================
// 函数: void	PCA_Init(void)
// 描述: PCA初始化程序.
// 参数: none
// 返回: none.
// 版本: V1.0, 2013-11-22
//========================================================================
void	PCA_Init(void)
{
	CR = 0;
	AUXR1 = (AUXR1 & ~(3<<4)) | PCA_P24_P25_P26_P27;	//切换IO口
	CCAPM0 = (PCA_Mode_Capture | PCA_Fall_Active | ENABLE);	//16位下降沿捕捉中断模式

	CCAPM1    = PCA_Mode_SoftTimer | ENABLE;
	CCAP1_tmp = UART3_BitTime;
	CCAP1L    = (u8)CCAP1_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
	CCAP1H    = (u8)(CCAP1_tmp >> 8);	//后写CCAP0H

	CH = 0;
	CL = 0;
	CMOD  = (CMOD  & ~(7<<1)) | PCA_Clock_1T;			//选择时钟源
	PPCA  = 1;	// 高优先级中断
	CR    = 1;	// 运行PCA定时器
}
//======================================================================

//========================================================================
// 函数: void	PCA_Handler (void) interrupt 7
// 描述: PCA中断处理程序.
// 参数: None
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	PCA_Handler (void) interrupt 7
{
	if(CCF0)		//PCA模块0中断
	{
		CCF0 = 0;			//清PCA模块0中断标志
		if(Rx3_Ring)		//已收到起始位
		{
			if (--Rx3_BitCnt == 0)		//接收完一帧数据
			{
				Rx3_Ring = 0;			//停止接收
				Rx3_BUF = Rx3_DAT;		//存储数据到缓冲区
				RX3_End = 1;
				CCAPM0 = (PCA_Mode_Capture | PCA_Fall_Active | ENABLE);	//16位下降沿捕捉中断模式
			}
			else
			{
				Rx3_DAT >>= 1;			  		//把接收的单b数据 暂存到 RxShiftReg(接收缓冲)
				if(P_RX3) Rx3_DAT |= 0x80;  	//shift RX data to RX buffer
				CCAP0_tmp += UART3_BitTime;		//数据位时间
				CCAP0L = (u8)CCAP0_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
				CCAP0H = (u8)(CCAP0_tmp >> 8);	//后写CCAP0H
			}
		}
		else
		{
			CCAP0_tmp = ((u16)CCAP0H << 8) + CCAP0L;	//读捕捉寄存器
			CCAP0_tmp += (UART3_BitTime / 2 + UART3_BitTime);	//起始位 + 半个数据位
			CCAP0L = (u8)CCAP0_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
			CCAP0H = (u8)(CCAP0_tmp >> 8);	//后写CCAP0H
			CCAPM0 = (PCA_Mode_SoftTimer | ENABLE);	//16位软件定时中断模式
			Rx3_Ring = 1;					//标志已收到起始位
			Rx3_BitCnt = 9;					//初始化接收的数据位数(8个数据位+1个停止位)
		}
	}

	if(CCF1)	//PCA模块1中断, 16位软件定时中断模式
	{
		CCF1 = 0;						//清PCA模块1中断标志
		CCAP1_tmp += UART3_BitTime;
		CCAP1L = (u8)CCAP1_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
		CCAP1H = (u8)(CCAP1_tmp >> 8);	//后写CCAP0H

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
}



