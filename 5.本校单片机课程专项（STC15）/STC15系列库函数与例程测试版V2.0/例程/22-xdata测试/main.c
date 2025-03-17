
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


#include	"config.h"
#include	"USART1.h"
#include	"delay.h"
#include	"app_test_xdata.h"


/*************	功能说明	**************

测试外挂的32K xdata程序。
测试方式: 
1: 写入0x55, 并读出判断是否全部是0x55.
2: 写入0xaa, 并读出判断是否全部是0xaa.
3: 写入32768字节的汉字字库(类似于随机数据), 并读出比较.

通过串口发送单个字符x或X, 开始测试, 并返回相关的测试结果.

串口设置115200bps, 8, N, 1,  主时钟为22.1184MHZ.

******************************************/

/*************	本地常量声明	**************/
#define	XDATA_LENTH		32768	//xdata长度
#define	HZK_LENTH		4096	//32768	//字库长度



/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/


/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	PrintString1("STC15F2K60S2 xdata test programme!\r\n");	//SUART1发送一个字符串
}



/*************  测试xdata函数 *****************/
u8	TestXRAM(void)
{
	u8	code	*ptc;
	u8	xdata	*ptx;
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
	for(ptc=0; ptc<HZK_LENTH; ptc++)	{*ptx = *ptc;	ptx++;}
	ptx = 0;
	for(ptc=0; ptc<HZK_LENTH; ptc++)
	{
		if(*ptx != *ptc)	return 4;	//写字库错误
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
	if(i >= 10000)	TX1_write2buff(i/10000+'0'),	i %= 10000;
	TX1_write2buff(i/1000+'0'),	i %= 1000;
	TX1_write2buff(i/100+'0'),	i %= 100;
	TX1_write2buff(i/10+'0');
	TX1_write2buff(i%10+'0');
	TX1_write2buff(0x0d);
	TX1_write2buff(0x0a);
}

/*************  xdata测试返回信息函数 *****************/
void	Xdata_Test(void)
{
	u8	i;
	if((RX1_Buffer[0] == 'x') || (RX1_Buffer[0] == 'X'))
	{
		PrintString1("正在测试 xdata, 请稍候......\r\n");
		while(COM1.B_TX_busy > 0)	;
		
		i = TestXRAM();
			 if(i == 0)	PrintString1("测试 xdata 结果正确!\r\n");
		else if(i == 1)	PrintString1("测试 xdata 写入0x55错误!  "),	TxErrorAddress();
		else if(i == 2)	PrintString1("测试 xdata 写入0xaa错误!  "),	TxErrorAddress();
		else if(i == 3)	PrintString1("测试 xdata 连续写入错误!  "),	TxErrorAddress();
		else if(i == 4)	PrintString1("测试 xdata 写入字库错误!  "),	TxErrorAddress();

		while(COM1.B_TX_busy > 0)	;
	}
}

/***************** 主函数 *****************************/
void main(void)
{

	UART_config();	//串口初始化
	EA = 1;			//全局中断允许

	BUS_SPEED_1T();	//1T  2T  4T  8T	3V@22MHZ用1T会访问错误
	ExternalRAM_enable();	//允许外部XDATA
//	InternalRAM_enable();	//允许内部XDATA

	while (1)
	{
		delay_ms(1);
		if(COM1.RX_TimeOut > 0)		//超时计数
		{
			if(--COM1.RX_TimeOut == 0)	//串口通讯结束
			{
				if(COM1.RX_Cnt > 0)		//收到数据字节数
				{
					if(COM1.RX_Cnt == 1)	Xdata_Test();	//单字节命令
				}
				COM1.RX_Cnt = 0;
			}
		}
	}
}



