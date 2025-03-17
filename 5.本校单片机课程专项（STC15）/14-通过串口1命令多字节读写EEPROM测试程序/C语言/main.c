

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



/*************	本程序功能说明	**************

				测试说明

通过串口对STC内部自带的EEPROM(FLASH)进行读写测试。

对FLASH做扇区擦除、写入、读出的操作，命令指定地址。

默认波特率:  115200,8,N,1. 
默认主时钟:  22118400HZ.

串口命令设置: (命令字母不区分大小写)
	W 0x8000 1234567890  --> 对0x8000地址写入字符1234567890.
	R 0x8000 10          --> 对0x8000地址读出10个字节数据. 

注意：为了通用，程序不识别地址是否有效，用户自己根据具体的型号来决定。

******************************************/


#define 	MAIN_Fosc			22118400L	//定义主时钟
#include	"STC15Fxxxx.H"

#define		Baudrate1			115200L
#define		Tmp_Length			70		//读写EEPROM缓冲长度

#define		UART1_BUF_LENGTH	(Tmp_Length+9)	//串口缓冲长度

u8	RX1_TimeOut;
u8	TX1_Cnt;	//发送计数
u8	RX1_Cnt;	//接收计数
bit	B_TX1_Busy;	//发送忙标志

u8 	xdata 	RX1_Buffer[UART1_BUF_LENGTH];	//接收缓冲
u8	xdata	tmp[Tmp_Length];		//EEPROM操作缓冲


void	UART1_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void	PrintString1(u8 *puts);
void	UART1_TxByte(u8 dat);
void	delay_ms(u8 ms);
u8		CheckData(u8 dat);
u16		GetAddress(void);
u8		GetDataLength(void);
void	EEPROM_SectorErase(u16 EE_address);
void 	EEPROM_read_n(u16 EE_address,u8 *DataAddress,u8 length);
u8		EEPROM_write_n(u16 EE_address,u8 *DataAddress,u8 length);


/********************* 主函数 *************************/
void main(void)
{
	u8	i,j;
	u16 addr;
	u8	status;

	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	UART1_config(1);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	EA = 1;	//允许总中断

	PrintString1("STC15F2K60S2系列单片机EEPROM测试程序, 串口命令设置如下示例:\r\n");	//SUART1发送一个字符串
	PrintString1("W 0x8000 1234567890  --> 对0x8000地址写入字符1234567890.\r\n");	//SUART1发送一个字符串
	PrintString1("R 0x8000 10          --> 对0x8000地址读出10个字节数据.\r\n");	//SUART1发送一个字符串

	while(1)
	{
		delay_ms(1);
		if(RX1_TimeOut > 0)		//超时计数
		{
			if(--RX1_TimeOut == 0)
			{
		//		for(i=0; i<RX1_Cnt; i++)	UART1_TxByte(RX1_Buffer[i]);	//把收到的数据原样返回,用于测试

				status = 0xff;	//状态给一个非0值
				if((RX1_Cnt >= 10) && (RX1_Buffer[1] == ' ') && (RX1_Buffer[8] == ' '))	//最短命令为10个字节
				{
					for(i=0; i<8; i++)
					{
						if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))	RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';	//小写转大写
					}
					addr = GetAddress();
					if(addr < 63488)	//限制在0~123扇区
					{
						if(RX1_Buffer[0] == 'W')	//写入N个字节
						{
							j = RX1_Cnt - 9;
							if(j > Tmp_Length)	j = Tmp_Length;	//越界检测
							EEPROM_SectorErase(addr);			//擦除扇区
							i = EEPROM_write_n(addr,&RX1_Buffer[9],j);		//写N个字节
							if(i == 0)
							{
								PrintString1("已写入");
								if(j >= 100)	{UART1_TxByte(j/100+'0');	j = j % 100;}
								if(j >= 10)		{UART1_TxByte(j/10+'0');	j = j % 10;}
								UART1_TxByte(j%10+'0');
								PrintString1("字节数据!\r\n");
							}
							else	PrintString1("写入错误!\r\n");
							status = 0;	//命令正确
						}

						else if(RX1_Buffer[0] == 'R')	//PC请求返回N字节EEPROM数据
						{
							j = GetDataLength();
							if(j > Tmp_Length)	j = Tmp_Length;	//越界检测
							if(j > 0)
							{
								PrintString1("读出");
								UART1_TxByte(j/10+'0');
								UART1_TxByte(j%10+'0');
								PrintString1("个字节数据如下：\r\n");
								EEPROM_read_n(addr,tmp,j);
								for(i=0; i<j; i++)	UART1_TxByte(tmp[i]);
								UART1_TxByte(0x0d);
								UART1_TxByte(0x0a);
								status = 0;	//命令正确
							}
						}
					}
				}
				if(status != 0)	PrintString1("命令错误!\r\n");
				RX1_Cnt  = 0;	//清除字节数
			}
		}
	}
}
//========================================================================


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
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 14000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}

//========================================================================
// 函数: u8	CheckData(u8 dat)
// 描述: 将字符"0~9,A~F或a~f"转成十六进制.
// 参数: dat: 要检测的字符.
// 返回: 0x00~0x0F为正确. 0xFF为错误.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	CheckData(u8 dat)
{
	if((dat >= '0') && (dat <= '9'))		return (dat-'0');
	if((dat >= 'A') && (dat <= 'F'))		return (dat-'A'+10);
	return 0xff;
}

//========================================================================
// 函数: u16	GetAddress(void)
// 描述: 计算各种输入方式的地址.
// 参数: 无.
// 返回: 16位EEPROM地址.
// 版本: V1.0, 2013-6-6
//========================================================================
u16	GetAddress(void)
{
	u16	address;
	u8	i,j;
	
	address = 0;
	if((RX1_Buffer[2] == '0') && (RX1_Buffer[3] == 'X'))
	{
		for(i=4; i<8; i++)
		{
			j = CheckData(RX1_Buffer[i]);
			if(j >= 0x10)	return 65535;	//error
			address = (address << 4) + j;
		}
		return (address);
	}
	return	65535;	//error
}

/**************** 获取要读出数据的字节数 ****************************/
u8	GetDataLength(void)
{
	u8	i;
	u8	length;
	
	length = 0;
	for(i=9; i<RX1_Cnt; i++)
	{
		if(CheckData(RX1_Buffer[i]) >= 10)	break;
		length = length * 10 + CheckData(RX1_Buffer[i]);
	}
	return (length);
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
	for (; *puts != 0;	puts++)	UART1_TxByte(*puts);  	//遇到停止符0结束
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
	RX1_TimeOut = 0;
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


/*
STC15F/L2KxxS2	扇区分配，512字节/扇区，从0x0000开始。

     型号        大小   扇区数  开始地址  结束地址   MOVC读偏移地址
STC15F/L2K08S2   53K   106扇区  0x0000  ~  0xD3FF        0x2000
STC15F/L2K16S2   45K    90扇区  0x0000  ~  0xB3FF        0x4000
STC15F/L2K24S2   37K    74扇区  0x0000  ~  0x93FF        0x6000
STC15F/L2K32S2   29K    58扇区  0x0000  ~  0x73FF        0x8000
STC15F/L2K40S2   21K    42扇区  0x0000  ~  0x53FF        0xA000
STC15F/L2K48S2   13K    26扇区  0x0000  ~  0x33FF        0xC000
STC15F/L2K56S2   5K     10扇区  0x0000  ~  0x13FF        0xE000
STC15F/L2K60S2   1K      2扇区  0x0000  ~  0x03FF        0xF000

STC15F/L2K61S2   无EPROM, 整个122扇区的FLASH都可以擦写 地址 0x0000~0xF3ff.
*/


#define		ISP_ENABLE()	ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY)
#define		ISP_DISABLE()	ISP_CONTR = 0; ISP_CMD = 0; ISP_TRIG = 0; ISP_ADDRH = 0xff; ISP_ADDRL = 0xff



//========================================================================
// 函数: void DisableEEPROM(void)
// 描述: 禁止EEPROM.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void DisableEEPROM(void)		//禁止访问EEPROM
{
	ISP_CONTR = 0;				//禁止ISP/IAP操作
	ISP_CMD   = 0;				//去除ISP/IAP命令
	ISP_TRIG  = 0;				//防止ISP/IAP命令误触发
	ISP_ADDRH = 0xff;			//指向非EEPROM区，防止误操作
	ISP_ADDRL = 0xff;			//指向非EEPROM区，防止误操作
}

//========================================================================
// 函数: void	EEPROM_Trig(void)
// 描述: 触发EEPROM操作.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void	EEPROM_Trig(void)
{
	F0 = EA;	//保存全局中断
	EA = 0;		//禁止中断, 避免触发命令无效
	ISP_TRIG();							//先送5AH，再送A5H到ISP/IAP触发寄存器，每次都需要如此
										//送完A5H后，ISP/IAP命令立即被触发启动
										//CPU等待IAP完成后，才会继续执行程序。
	_nop_();
	_nop_();
	EA = F0;	//恢复全局中断
}

//========================================================================
// 函数: void	EEPROM_SectorErase(u16 EE_address)
// 描述: 擦除一个扇区.
// 参数: EE_address:  要擦除的EEPROM的扇区中的一个字节地址.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void	EEPROM_SectorErase(u16 EE_address)
{
	ISP_ENABLE();						//设置等待时间，允许ISP/IAP操作，送一次就够
	ISP_ERASE();						//宏调用, 送扇区擦除命令，命令不需改变时，不需重新送命令
										//只有扇区擦除，没有字节擦除，512字节/扇区。
										//扇区中任意一个字节地址都是扇区地址。
	ISP_ADDRH = EE_address / 256;		//送扇区地址高字节（地址需要改变时才需重新送地址）
	ISP_ADDRL = EE_address % 256;		//送扇区地址低字节
	EEPROM_Trig();						//触发EEPROM操作
	DisableEEPROM();					//禁止EEPROM操作
}

//========================================================================
// 函数: void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u8 lenth)
// 描述: 读N个字节函数.
// 参数: EE_address:  要读出的EEPROM的首地址.
//       DataAddress: 要读出数据的指针.
//       length:      要读出的长度
// 返回: 0: 写入正确.  1: 写入长度为0错误.  2: 写入数据错误.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u8 length)
{
	ISP_ENABLE();							//设置等待时间，允许ISP/IAP操作，送一次就够
	ISP_READ();								//送字节读命令，命令不需改变时，不需重新送命令
	do
	{
		ISP_ADDRH = EE_address / 256;		//送地址高字节（地址需要改变时才需重新送地址）
		ISP_ADDRL = EE_address % 256;		//送地址低字节
		EEPROM_Trig();						//触发EEPROM操作
		*DataAddress = ISP_DATA;			//读出的数据送往
		EE_address++;
		DataAddress++;
	}while(--length);

	DisableEEPROM();
}


//========================================================================
// 函数: u8 EEPROM_write_n(u16 EE_address,u8 *DataAddress,u8 length)
// 描述: 写N个字节函数.
// 参数: EE_address:  要写入的EEPROM的首地址.
//       DataAddress: 要写入数据的指针.
//       length:      要写入的长度
// 返回: 0: 写入正确.  1: 写入长度为0错误.  2: 写入数据错误.
// 版本: V1.0, 2014-6-30
//========================================================================
u8 	EEPROM_write_n(u16 EE_address,u8 *DataAddress,u8 length)
{
	u8	i;
	u16	j;
	u8	*p;
	
	if(length == 0)	return 1;	//长度为0错误

	ISP_ENABLE();						//设置等待时间，允许ISP/IAP操作，送一次就够
	i = length;
	j = EE_address;
	p = DataAddress;
	ISP_WRITE();							//宏调用, 送字节写命令
	do
	{
		ISP_ADDRH = EE_address / 256;		//送地址高字节（地址需要改变时才需重新送地址）
		ISP_ADDRL = EE_address % 256;		//送地址低字节
		ISP_DATA  = *DataAddress;			//送数据到ISP_DATA，只有数据改变时才需重新送
		EEPROM_Trig();						//触发EEPROM操作
		EE_address++;						//下一个地址
		DataAddress++;						//下一个数据
	}while(--length);						//直到结束

	EE_address = j;
	length = i;
	DataAddress = p;
	i = 0;
	ISP_READ();								//读N个字节并比较
	do
	{
		ISP_ADDRH = EE_address / 256;		//送地址高字节（地址需要改变时才需重新送地址）
		ISP_ADDRL = EE_address % 256;		//送地址低字节
		EEPROM_Trig();						//触发EEPROM操作
		if(*DataAddress != ISP_DATA)		//读出的数据与源数据比较
		{
			i = 2;
			break;
		}
		EE_address++;
		DataAddress++;
	}while(--length);

	DisableEEPROM();
	return i;
}



