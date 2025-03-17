
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

//编写: STC梁工

//8位数据访问方式	LCD1602		标准程序	梁工编写	2002-12-21

#include	"config.h"
#include	"delay.h"

#define LineLenth	16		//16x2
//#define LineLenth 20		//20x2
//#define LineLenth 40		//40x2

//this programme is used for LCD module 16x2 20x2 or 40x2.

/*************	Pin define	*****************************************************/

sfr		LCD_BUS = 0x80;	//P0--0x80, P1--0x90, P2--0xA0, P3--0xB0

sbit	LCD_B7  = LCD_BUS^7;	//D7 -- Pin 14		LED- -- Pin 16 
sbit	LCD_B6  = LCD_BUS^6;	//D6 -- Pin 13		LED+ -- Pin 15
sbit	LCD_B5  = LCD_BUS^5;	//D5 -- Pin 12		Vo   -- Pin 3
sbit	LCD_B4  = LCD_BUS^4;	//D4 -- Pin 11		VDD  -- Pin 2
sbit	LCD_B3  = LCD_BUS^3;	//D3 -- Pin 10		VSS  -- Pin 1
sbit	LCD_B2  = LCD_BUS^2;	//D2 -- Pin  9
sbit	LCD_B1  = LCD_BUS^1;	//D1 -- Pin  8
sbit	LCD_B0  = LCD_BUS^0;	//D0 -- Pin  7

sbit	LCD_ENA	= P2^7;	//Pin 6
sbit	LCD_RW	= P2^6;	//Pin 5	//LCD_RS   R/W   DB7--DB0        FOUNCTION
sbit	LCD_RS	= P2^5;	//Pin 4	//	0		0	  INPUT      write the command to LCD model
								//	0		1     OUTPUT     read BF and AC pointer from LCD model
								//	1		0     INPUT      write the data to LCD  model
								//	1		1     OUTPUT     read the data from LCD model

/******************************************************************************
                 HD44780U    LCD_MODUL DRIVE PROGRAMME
*******************************************************************************

total 2 lines, 16x2= 32
first line address:  0~15
second line address: 64~79

total 2 lines, 20x2= 40
first line address:  0~19
second line address: 64~83

total 2 lines, 40x2= 80
first line address:  0~39
second line address: 64~103
*/

#define C_CLEAR			0x01		//clear LCD
#define C_HOME 			0x02		//cursor go home
#define C_CUR_L			0x04		//cursor shift left after input
#define C_RIGHT			0x05		//picture shift right after input
#define C_CUR_R			0x06		//cursor shift right after input
#define C_LEFT 			0x07		//picture shift left after input
#define C_OFF  			0x08		//turn off LCD
#define C_ON   			0x0C		//turn on  LCD
#define C_FLASH			0x0D		//turn on  LCD, flash 
#define C_CURSOR		0x0E		//turn on  LCD and cursor
#define C_FLASH_ALL		0x0F		//turn on  LCD and cursor, flash
#define C_CURSOR_LEFT	0x10		//single cursor shift left
#define C_CURSOR_RIGHT	0x10		//single cursor shift right
#define C_PICTURE_LEFT	0x10		//single picture shift left
#define C_PICTURE_RIGHT	0x10		//single picture shift right
#define C_BIT8			0x30		//set the data is 8 bits
#define C_BIT4			0x20		//set the data is 8 bits
#define C_L1DOT7		0x30		//8 bits,one line 5*7  dots
#define C_L1DOT10		0x34		//8 bits,one line 5*10 dots
#define C_L2DOT7		0x38		//8 bits,tow lines 5*7 dots
#define C_4bitL2DOT7	0x28		//4 bits,tow lines 5*7 dots
#define C_CGADDRESS0	0x40		//CGRAM address0 (addr=40H+x)
#define C_DDADDRESS0	0x80		//DDRAM address0 (addr=80H+x)


#define	LCD_DelayNop()	NOP(20)

#define		LCD_BusData(dat)	LCD_BUS = dat


void	CheckBusy(void)
{
	u16	i;
	for(i=0; i<5000; i++)	{if(!LCD_B7)	break;}		//check the LCD busy or not. With time out
//	while(LCD_B7);			//check the LCD busy or not. Without time out
}

/**********************************************/
void IniSendCMD(u8 cmd)		//write the command to LCD
{
	LCD_RW = 0;
	LCD_BusData(cmd);
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_BusData(0xff);
}

/**********************************************/
void Write_CMD(u8 cmd)		//write the command to LCD
{
	LCD_RS  = 0;
	LCD_RW = 1;
	LCD_BusData(0xff);
	LCD_DelayNop();
	LCD_ENA = 1;
	CheckBusy();			//check the LCD busy or not.
	LCD_ENA = 0;
	LCD_RW = 0;
	
	LCD_BusData(cmd);
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_BusData(0xff);
}

/**********************************************/
void Write_DIS_Data(u8 dat)		//write the display data to LCD
{
	LCD_RS = 0;
	LCD_RW = 1;

	LCD_BusData(0xff);
	LCD_DelayNop();
	LCD_ENA = 1;
	CheckBusy();			//check the LCD busy or not.
	LCD_ENA = 0;
	LCD_RW = 0;
	LCD_RS  = 1;

	LCD_BusData(dat);
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_BusData(0xff);
}


/*********	初始化函数	**************************/
void Initialize_LCD(void)		//intilize LCD, input none, output none
{
	LCD_ENA = 0;
	LCD_RS  = 0;
	LCD_RW = 0;

	delay_ms(100);
	IniSendCMD(C_BIT8);		//set the data is 8 bits

	delay_ms(10);
	IniSendCMD(C_BIT8);		//set the data is 8 bits

	delay_ms(10);
	IniSendCMD(C_L2DOT7);		//tow lines 5*7 dots

	delay_ms(6);
	Write_CMD(C_CLEAR);		//clear LCD RAM
	Write_CMD(C_CUR_R);		//Curror Shift Right
//	Write_CMD(C_FLASH_ALL);	//turn on  LCD and cursor, flash
	Write_CMD(C_ON);		//turn on  LCD
//	Write_AC(1,1);			//set the AC pointer
}


/**********************************************/
void Write_AC(u8 hang,u8 lie)	//write the AC Pointer address to LCD
{
	if(hang == 1)	Write_CMD((lie-1)  | 0x80);
	if(hang == 2)	Write_CMD((lie+63) | 0x80);
}

/**********************************************/
void ClearLine(u8 hang)
{
	u8 i;
	if((hang == 0) || (hang > 2))		return; 
	Write_AC(hang,1);	//set the AC pointer
	for(i=0; i<LineLenth; i++)	Write_DIS_Data(' ');
}


/****************** 写一个字符，指定行、列和字符 ****************************/
void	WriteChar(u8 hang,u8 lie,u8 dat)
{
	if((hang == 0) || (hang > 2))		return; 
	if((lie == 0) || (lie > LineLenth))	return;
	Write_AC(hang,lie);	//set the AC pointer
	Write_DIS_Data(dat);
}

/****************** 写一个字符串，指定行、列和字符串首地址 ****************************/
void PutString(u8 hang,u8 lie,u8 *puts)
{
	for ( ;  *puts != 0;  puts++)		//遇到停止符0结束
	{
		WriteChar(hang, lie, *puts);
		if(++lie > 20)	break;
	}
}


//******************** LCD40 Module END ***************************

