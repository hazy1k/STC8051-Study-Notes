
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

驱动LCD128*64黑白点阵屏.

用户可以修改宏来选择时钟频率.

显示效果为: 第一行显示模拟的RTC时间, 7、8行显示8个16*16点的汉字. 本程序使用的LCD是有两个片选的.

注意: LCD接口的16脚是空脚, 本程序要把这个脚接到P2.2做另一页的片选.

******************************************/

#define MAIN_Fosc		22118400UL	//定义主时钟

#include	"STC15Fxxxx.H"



/*************	IO口定义	**************/
sbit	P_HC595_SER   = P4^0;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P5^4;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P4^3;	//pin 11	SRCLK	Shift data clock

u8	hour,minute,second;


void  delay_ms(u8 ms);
void DisableHC595(void);
void SetStartAddress(u8 x,u8 y);
void FillPage(u8 y,u8 color);			//Clear Page LCD RAM
void FillAll(u8 color);			//Clear CSn LCD RAM
void Initialize_LCD(void);				//initialize LCD
void WriteAscii6x8(u8 x,u8 y,u8 ascii,u8 color);
void WriteHZ16(u8 x,u8 y,u16 hz,u8 color);
#define P0n_push_pull(bitn)			P0M1 &= ~(bitn),	P0M0 |=  (bitn)	//01


/****************  外部函数声明和外部变量声明 *****************/


/********************** 显示时钟函数 ************************/
void	DisplayRTC(void)
{
	if(hour >= 10)	WriteAscii6x8(0,0,hour / 10 + '0',0);
	else			WriteAscii6x8(0,0,':',0);
	WriteAscii6x8(6, 0,hour % 10 +'0',0);
	WriteAscii6x8(12,0,':',0);
	WriteAscii6x8(18,0,minute / 10+'0',0);
	WriteAscii6x8(24,0,minute % 10+'0',0);
	WriteAscii6x8(30,0,':',0);
	WriteAscii6x8(36,0,second / 10 +'0',0);
	WriteAscii6x8(42,0,second % 10 +'0',0);

}

/********************** RTC演示函数 ************************/
void	RTC(void)
{
	if(++second >= 60)
	{
		second = 0;
		if(++minute >= 60)
		{
			minute = 0;
			if(++hour >= 24)	hour = 0;
		}
	}
}



/********************** 主函数 ************************/
void main(void)
{
	u8	i;

	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	Initialize_LCD();

	DisableHC595();		//禁止掉学习板上的HC595显示，省电
	
	hour   = 12;	//初始化时间值
	minute = 0;
	second = 0;
	DisplayRTC();

	for(i=0; i<8; i++)	WriteHZ16(i*16,4,i,0);

	while(1)
	{
		delay_ms(250);		//延时1秒
		delay_ms(250);
		delay_ms(250);
		delay_ms(250);
		RTC();
		DisplayRTC();
	}
} 
/**********************************************/

//========================================================================
// 函数: void  delay_ms(u8 ms)
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

void DisableHC595(void)
{		
	u8	i;
	P_HC595_SER   = 1;
	for(i=0; i<20; i++)
	{
		P_HC595_SRCLK = 1;
		P_HC595_SRCLK = 0;
	}
	P_HC595_RCLK = 1;
	P_HC595_RCLK = 0;							//锁存输出数据
	P_HC595_RCLK = 1;
	P_HC595_RCLK = 0;							//锁存输出数据
}





#include	"ASCII6x8.h"
#include	"HZK16.h"


/*************	功能说明	**************

LCD12864的基本驱动程序，兼容IC：HD61202 KS0108B SED1520.

******************************************/



/********************* LCD 128*64 Module, Controler HD61202U *************************************
	R_W		L: write data to LCD;  H: read data from LCD
	RS		L: Instruction Code Input or state output;   H: data input or output 
	RESET	H: normal work;   	L: reset LCD
	CS1		H: disable Pice 1,  L:enable Pice 1
	CS2		H: disable Pice 2,  L:enable Pice 2
	CS3		H: disable Pice 3,  L:enable Pice 3
	E		L: disable;    H: read State or data from LCD; 
			H-->L: write command or data to LCD

	D0~D7	Data bus

	RS  R/W   
	0    0    :write command to LCD
	0    1    :read  status from LCD
	1    0    :write display data to LCD
	1    1    :read  display data from LCD

;********************************************************************************/

#define C_Busy			0x80
#define C_On_Off		0x20
#define C_Reset			0x10
#define LCD_On			0x3F
#define LCD_Off			0x3E
#define LCD_StartLine0	0xC0	//Set display start line(0~3FH), real start line=(0C0H+Line Number)
#define LCD_Page0		0xB8	//Set Page (X Address)(0~7), real page = 0B8H+Page Number
#define LCD_SetColumn0	0x40	//Set Y address (0~3FH), real column = 40H + column number

							//	1----VSS
							//	2----VDD	+5V
							//	3----Vo		LCD Opration Voltage
sbit RS			=  	P2^5;	//	4----RS		L-->Command,H-->Data
sbit R_W		=  	P2^6;	//	5----R_W	L-->Write,	H-->Read
sbit LCD_EN		=   P2^7;	//	6----E		H-->L: write command or data to LCD,H: read State or data from LCD;
sfr  LCD_DATA	=	0x80;	//	7~14--D0~D7	Data bus
sbit CS1		=  	P2^4;	//	15---CS1	L-->Chip select1
sbit CS2		=  	P2^2;	//	16---CS2	L-->Chip select2
sbit LCD_RESET	=	P2^3;	//	17---RESET	L-->Enable
							//	18---VEE	L-->Chip select3
							//	19---LED-	LED A	Back Light LED Drive
							//	20---LED+	LED K	Back Light LED Drive
sbit LCD_BUSY	=	LCD_DATA^7;

#define	CMD_WriteCMD()		RS=0, R_W=0
#define	CMD_ReadStatus()	RS=0, R_W=1
#define	CMD_WriteData()		RS=1, R_W=0
#define	CMD_ReadData()		RS=1, R_W=1

#define	SELECT		1
#define	UNSELECT	0




//******************************************

void	LCD_delay(void)
{
	u8	i;
//	i = (u8)(MAIN_Fosc / 2000000UL);
	i = (u8)(MAIN_Fosc / 1000000UL);
	while(--i)	;
}

//******************************************
u8	CheckBusy(void)	//不需要检测忙
{
/*
	u8	i;
	LCD_DATA = 0xFF;
	CMD_ReadStatus();
	LCD_EN = 1;
	LCD_delay();
	for(i=0; i<100; i++)		//检测忙, 一般最大 大约10us,  这个使用超时退出
	{
		if(LCD_BUSY == 0)	break;		//  11T / loop, 0.5us at 22.1184MHZ
	}
	LCD_EN = 0;
	if(i >= 100)	return 1;	//超时
*/
	return	0;					//正确
}


//******************************************
void WriteCMD(u8 cmd)
{
	if(CheckBusy() > 0)		return;	//检测忙错误

	CMD_WriteCMD();
	LCD_DATA = cmd;
	LCD_delay();
	LCD_EN = 1; 
	LCD_delay();
	LCD_EN = 0;
	LCD_delay();
	LCD_DATA = 0xFF;
}

//******************************************
void WriteData(u8 dat)			//write display data to LCD
{
	if(CheckBusy() > 0)		return;	//检测忙错误

	CMD_WriteData();
	LCD_DATA = dat;
	LCD_delay();
	LCD_EN = 1;
	LCD_delay();
	LCD_EN = 0;
	LCD_DATA = 0xFF;
}

//******************************************
void SetStartAddress(u8 x,u8 y)
{
	WriteCMD(x + LCD_SetColumn0);
	WriteCMD(y + LCD_Page0);
}

//******************************************
void FillPage(u8 y,u8 color)			//Clear Page LCD RAM
{
	u8 j;
	SetStartAddress(0,y);
	for(j=0; j<64; j++)	WriteData(color);
}

//******************************************
void FillAll(u8 color)			//Clear CSn LCD RAM
{
	u8 i;
	for(i=0; i<8; i++)	FillPage(i,color);
}


//******************************************
void Initialize_LCD(void)				//initialize LCD
{
	LCD_RESET = 0;
	delay_ms(10);
	LCD_RESET = 1;
	delay_ms(150);

//	CS1 = SELECT;
//	CS2 = SELECT;
	WriteCMD(LCD_Off);			//0x3E	  turn off LCD
	WriteCMD(LCD_StartLine0);	//#0C0H    set the start line is the first line
	WriteCMD(LCD_On);			//#3FH    turn on display
	FillAll(0);
	CS1 = UNSELECT;
	CS2 = UNSELECT;
}


//******************************************
void WriteAscii6x8(u8 x,u8 y,u8 ascii,u8 color)
{
	u8 code *p;
	u8 i;
	
	p = ascii * 6 + ASCII6x8;
	if(x < 64)		CS1 = SELECT;
	else			CS2 = SELECT,	x -= 64;
	SetStartAddress(x,y); 
	for(i=0; i<6; i++)		{WriteData((*p) ^ color);	p++;}
	CS1 = UNSELECT;
	CS2 = UNSELECT;
}

//******************************************
void WriteHZ16(u8 x,u8 y,u16 hz,u8 color)
{
	u8 code *p;
	u8 i;

	p = hz * 32 + HZK16;

	if(x < 64)		CS1 = SELECT;
	else			CS2 = SELECT,	x -= 64;
	SetStartAddress(x,y); 
	for(i=0; i<16; i++)		{WriteData((*p) ^ color);	p++;}
	
	SetStartAddress(x,y+1); 
	for(i=0; i<16; i++)		{WriteData((*p) ^ color);	p++;}
	CS1 = UNSELECT;
	CS2 = UNSELECT;
}

