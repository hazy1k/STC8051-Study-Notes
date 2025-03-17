
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


#define 	MAIN_Fosc			22118400L	//定义主时钟
#include	"STC15Fxxxx.H"

#include	"app_test_xdata.h"

/*************	功能说明	**************

测试外挂的32K xdata程序。
测试方式: 
1: 写入0x55, 并读出判断是否全部是0x55.
2: 写入0xaa, 并读出判断是否全部是0xaa.
3: 写入32768字节的汉字字库(类似于随机数据), 并读出比较.

通过串口发送单个字符x或X, 开始测试, 并返回相关的测试结果.

串口设置115200bps, 8, N, 1,  主时钟为22.1184MHZ. 切换到P1.6 P1.7.

******************************************/

/*************	本地常量声明	**************/
#define		Baudrate1		115200L

#define		XDATA_LENTH		32768	//xdata长度
#define		HZK_LENTH		32768	//字库长度



/*************	本地变量声明	**************/
#define		UART1_BUF_LENGTH	64	//串口缓冲长度

u8	RX1_TimeOut;
u8	TX1_Cnt;	//发送计数
u8	RX1_Cnt;	//接收计数
bit	B_TX1_Busy;	//发送忙标志

u8 	xdata 	RX1_Buffer[UART1_BUF_LENGTH];	//接收缓冲


void	delay_ms(u8 ms);
u8		TestXRAM(void);
void	TxErrorAddress(void);
void	Xdata_Test(void);

void	delay_ms(u8 ms);
void	RX1_Check(void);
void	UART1_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void	PrintString1(u8 *puts);
void	UART1_TxByte(u8 dat);





/***************** 主函数 *****************************/
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

	delay_ms(10);
	UART1_config(1);
	EA = 1;		//全局中断允许

	PrintString1("STC15F2K60S2 xdata test programme!\r\n");	//SUART1发送一个字符串
	PrintString1("\r\n通过串口发送单个字符x或X, 开始测试.\r\n");

	BUS_SPEED_1T();	//1T  2T  4T  8T	3V@22MHZ用1T会访问错误
	ExternalRAM_enable();	//允许外部XDATA
//	InternalRAM_enable();	//允许内部XDATA

	while (1)
	{
		delay_ms(1);
		if(RX1_TimeOut > 0)		//超时计数
		{
			if(--RX1_TimeOut == 0)	//串口通讯结束
			{
				if(RX1_Cnt > 0)		//收到数据字节数
				{
					if(RX1_Cnt == 1)	Xdata_Test();	//单字节命令
				}
				RX1_Cnt = 0;
			}
		}
	}
}

//========================================================================
// 函数: void  delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void  delay_ms(u8 ms)
{
     u16 i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}


/*************  测试xdata函数 *****************/
u8	TestXRAM(void)
{
	u16	ptc;
	u8	xdata *ptx;
	u16	i,j;

	for(ptx=0; ptx<XDATA_LENTH; ptx++)	*ptx = 0x55;	//测试是否有位短路
	for(ptx=0; ptx<XDATA_LENTH; ptx++)	if(*ptx != 0x55)	return 1;	//测试0x55错误

	for(ptx=0; ptx<XDATA_LENTH; ptx++)	*ptx = 0xaa;	//测试是否有位短路
	for(ptx=0; ptx<XDATA_LENTH; ptx++)	if(*ptx != 0xaa)	return 2;	//测试0xaa错误

	i = 0;
	for(ptx=0; ptx<XDATA_LENTH; ptx++)
	{
		*ptx = (u8)(i >> 8);
		ptx++;
		*ptx = (u8)i;
		i++;
	}
	i = 0;
	for(ptx=0; ptx<XDATA_LENTH; ptx++)
	{
		j = *ptx;
		ptx++;
		j = (j << 8) + *ptx;
		if(i != j)	return 3;	//写连续数字错误
		i++;
	}

	ptx = 0;
	for(ptc=0; ptc<HZK_LENTH; ptc++)	{*ptx = hz[ptc];	ptx++;}
	ptx = 0;
	for(ptc=0; ptc<HZK_LENTH; ptc++)
	{
		if(*ptx != hz[ptc])	return 4;	//写字库错误
		ptx++;
	}

	return 0;

}


/*************  发送错误地址函数 *****************/
void	TxErrorAddress(void)
{
	u16	i;
	i = 0x00fd;
	PrintString1("错误地址 = ");
	if(i >= 10000)	UART1_TxByte(i/10000+'0'),	i %= 10000;
	UART1_TxByte(i/1000+'0'),	i %= 1000;
	UART1_TxByte(i/100+'0'),	i %= 100;
	UART1_TxByte(i/10+'0');
	UART1_TxByte(i%10+'0');
	UART1_TxByte(0x0d);
	UART1_TxByte(0x0a);
}

/*************  xdata测试返回信息函数 *****************/
void	Xdata_Test(void)
{
	u8	i;
	if((RX1_Buffer[0] == 'x') || (RX1_Buffer[0] == 'X'))
	{
		PrintString1("正在测试 xdata, 请稍候......\r\n");
		i = TestXRAM();
			 if(i == 0)	PrintString1("测试 xdata 结果正确!\r\n");
		else if(i == 1)	PrintString1("测试 xdata 写入0x55错误!  "),	TxErrorAddress();
		else if(i == 2)	PrintString1("测试 xdata 写入0xaa错误!  "),	TxErrorAddress();
		else if(i == 3)	PrintString1("测试 xdata 连续写入错误!  "),	TxErrorAddress();
		else if(i == 4)	PrintString1("测试 xdata 写入字库错误!  "),	TxErrorAddress();
	}
}


//========================================================================
// 函数: void	UART1_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================

void	UART1_TxByte(u8 dat)
{
	SBUF = dat;
	B_TX1_Busy = 1;
	while(B_TX1_Busy);
}

//========================================================================
// 函数: void PrintString1(u8 *puts)
// 描述: 串口1发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString1(u8 *puts)	//发送一个字符串
{
    for (; *puts != 0;	puts++)   	UART1_TxByte(*puts);  	//遇到停止符0结束
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
	/*********** 波特率使用定时器2 *****************/
	if(brt == 2)
	{
		AUXR |= 0x01;		//S1 BRT Use Timer2;
		SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
	}

	/*********** 波特率使用定时器1 *****************/
	else
	{
		TR1 = 0;
		AUXR &= ~0x01;		//S1 BRT Use Timer1;
		AUXR |=  (1<<6);	//Timer1 set as 1T mode
		TMOD &= ~(1<<6);	//Timer1 set As Timer
		TMOD &= ~0x30;		//Timer1_16bitAutoReload;
		TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
		TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
		ET1 = 0;	//禁止中断
		INT_CLKO &= ~0x02;	//不输出时钟
		TR1  = 1;
	}
	/*************************************************/

	SCON = (SCON & 0x3f) | 0x40;	//UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//	PS  = 1;	//高优先级中断
	ES  = 1;	//允许中断
	REN = 1;	//允许接收
	P_SW1 &= 0x3f;
	P_SW1 |= 0x80;		//UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (必须使用内部时钟)
//	PCON2 |=  (1<<4);	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE

	B_TX1_Busy = 0;
	TX1_Cnt = 0;
	RX1_Cnt = 0;
}


//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		if(RX1_Cnt >= UART1_BUF_LENGTH)	RX1_Cnt = 0;
		RX1_Buffer[RX1_Cnt] = SBUF;
		RX1_Cnt++;
		RX1_TimeOut = 5;
	}

	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;
	}
}

