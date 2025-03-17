
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


/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU Programme Demo ----------------------------*/
/* --- Fax: 86-755-82944243 ----------------------------------------*/
/* --- Tel: 86-755-82948412 ----------------------------------------*/
/* --- Web: www.STCMCU.com -----------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

/*      本程序经过测试完全正常, 不提供电话技术支持, 如不能理解, 请自行补充相关基础.  */


/*************	本程序功能说明	**************

对EEPROM做扇区擦除、写入64字节、读出64字节的操作。

EEPROM通用测试程序。可以在配置文件"config.h"中选择MCU类型。

波特率:  9600,8,N,1. 可以在配置文件"config.h"中修改.

命令设置:
	E 0, E 12, E 120	--> 对EEPROM进行扇区擦除操作，数字为扇区号，十进制.
	W 0, W 12, W 120	--> 对EEPROM进行扇区写入操作，数字为扇区号，十进制. 从扇区的开始地址写入。
	R 0, R 12, R 120	--> 对EEPROM进行扇区读出操作，数字为扇区号，十进制. 从扇区的开始地址读出。
	
	E 0x1234, E 0x1af5, E 0x10AB	--> 对EEPROM进行扇区擦除操作，数字为开始地址，十六进制.
	W 0x1234, W 0x1af5, W 0x10AB	--> 对EEPROM进行扇区写入操作，数字为开始地址，十六进制. 
	R 0x1234, R 0x1af5, R 0x10AB	--> 对EEPROM进行扇区读出操作，数字为开始地址，十六进制. 

注意：为了通用，程序不识别扇区是否有效，用户自己根据具体的型号来决定。

工程文件:
STC_EEPROM.c
test_EEPROM.c

日期: 2013-5-10

******************************************/

#include	"config.h"
#include	"USART.h"
#include	"delay.h"
#include 	"eeprom.h"



/*************	本地常量声明	**************/
u8	code	T_Strings[]={"去年今日此门中，人面桃花相映红。人面不知何处去，桃花依旧笑春风。"};


/*************	本地变量声明	**************/
u8	xdata	tmp[70];


/*************	本地函数声明	**************/


/*************  外部函数和变量声明 *****************/




void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 57600ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	USART_Configuration(USART2, &COMx_InitStructure);		//初始化串口2 USART1,USART2

	PrintString1("STC15F2K60S2系列单片机EEPROM测试程序!\r\n");	//SUART1发送一个字符串
	PrintString2("STC15F2K60S2系列单片机EEPROM测试程序!\r\n");	//SUART2发送一个字符串
}


/**********************************************/

u8	CheckData(u8 dat)
{
	if((dat >= '0') && (dat <= '9'))		return (dat-'0');
	if((dat >= 'A') && (dat <= 'F'))		return (dat-'A'+10);
	if((dat >= 'a') && (dat <= 'f'))		return (dat-'a'+10);
	return 0xff;
}

u16	GetAddress(void)
{
	u16	address;
	u8	i;
	
	address = 0;
	if(COM1.RX_Cnt <  3)	return 65535;	//error
	if(COM1.RX_Cnt <= 5)	//5个字节以内是扇区操作，十进制, 支持命令: 	E 0, E 12, E 120
						//												W 0, W 12, W 120
						//												R 0, R 12, R 120
	{
		for(i=2; i<COM1.RX_Cnt; i++)
		{
			if(CheckData(RX1_Buffer[i]) > 9)	return 65535;	//error
			address = address * 10 + CheckData(RX1_Buffer[i]);
		}
		if(address < 124)	//限制在0~123扇区
		{
			address <<= 9;
			return (address);
		}
	}
	else if(COM1.RX_Cnt == 8)	//8个字节直接地址操作，十六进制, 支持命令: E 0x1234, W 0x12b3, R 0x0A00
	{
		if((RX1_Buffer[2] == '0') && ((RX1_Buffer[3] == 'x') || (RX1_Buffer[3] == 'X')))
		{
			for(i=4; i<8; i++)
			{
				if(CheckData(RX1_Buffer[i]) > 0x0F)		return 65535;	//error
				address = (address << 4) + CheckData(RX1_Buffer[i]);
			}
			if(address < 63488)	return (address);	//限制在0~123扇区
		}
	}
	
	 return	65535;	//error
}

/********************* 主函数 *************************/
void main(void)
{
	u8	i;
	u16 addr;

	UART_config();
	EA = 1;

	while(1)
	{
		delay_ms(1);
		if(COM1.RX_TimeOut > 0)		//超时计数
		{
			if(--COM1.RX_TimeOut == 0)
			{
				if(RX1_Buffer[1] == ' ')
				{
					addr = GetAddress();
					if(addr < 63488)	//限制在0~123扇区
					{
						if(RX1_Buffer[0] == 'E')	//PC请求擦除一个扇区
						{
							EEPROM_SectorErase(addr);
							PrintString1("扇区擦除完成");
						}

						else if(RX1_Buffer[0] == 'W')	//PC请求写入EEPROM 64字节数据
						{
							EEPROM_write_n(addr,T_Strings,64);
							PrintString1("写入操作完成");
						}

						else if(RX1_Buffer[0] == 'R')	//PC请求返回64字节EEPROM数据
						{
							PrintString1("读出的数据如下：\r\n");
							EEPROM_read_n(addr,tmp,64);
							for(i=0; i<64; i++)	TX1_write2buff(tmp[i]);	//将数据返回给串口
							TX1_write2buff(0x0d);
							TX1_write2buff(0x0a);
						}
						else	PrintString1("命令错误!\r\n");
					}
					else	PrintString1("命令错误!\r\n");
				}

				COM1.RX_Cnt = 0;
			}
		}
	}
} 
/**********************************************/


