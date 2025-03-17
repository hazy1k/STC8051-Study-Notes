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
#include	"delay.h"

/*************	功能说明	**************

本程序演示用4个COM线驱动8位数码管，4位共阴、4位共阳搭配，使用4+8=12个IO驱动8个数码管。

******************************************/


/*************	宏定义	**************/
#define P1n_push_pull(bitn)			P1M1 &= ~(bitn),	P1M0 |=  (bitn)
#define P1n_pure_input(bitn)		P1M1 |=  (bitn),	P1M0 &= ~(bitn)
#define P2n_push_pull(bitn)			P2M1 &= ~(bitn),	P2M0 |=  (bitn)
#define P2n_pure_input(bitn)		P2M1 |=  (bitn),	P2M0 &= ~(bitn)


/*************	本地常量声明	**************/
u8 code t_display[]={						//标准字库
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11


/*************	IO口定义	**************/

sbit	P_COM0 = P1^0;
sbit	P_COM1 = P1^1;
sbit	P_COM2 = P1^2;
sbit	P_COM3 = P1^3;


/*************	本地变量声明	**************/

u8	DisplayCnt;
u8	LED8[8];
u8	hour,minute,second;
u16	msecond;

/*************	本地函数声明	**************/
void	LED_Driver(void);



/*************  外部函数和变量声明 *****************/



/********************** 阴阳数码管双驱动函数 ************************/
void	LED_Driver(void)
{
	u8	i,segment;

	i = 0;
	segment = t_display[LED8[DisplayCnt]];	//取段码
	P1n_pure_input(0x0f);	//COM扫描输出 禁止
	P2n_pure_input(0xff);	//SEG扫描输出 禁止

	if(DisplayCnt & 4)	//共阴 4~7
	{
		P2 = segment;		//输出共阴段码
		if(DisplayCnt == 4)			P_COM0 = 0,	i = 0x01;
		else if(DisplayCnt == 5)	P_COM1 = 0,	i = 0x02;
		else if(DisplayCnt == 6)	P_COM2 = 0,	i = 0x04;
		else						P_COM3 = 0,	i = 0x08;
	}
	else	//共阳 0~3
	{
		P2 = ~segment;	//输出共阳段码
		if(DisplayCnt == 0)			P_COM0 = 1,	i = 0x01;
		else if(DisplayCnt == 1)	P_COM1 = 1,	i = 0x02;
		else if(DisplayCnt == 2)	P_COM2 = 1,	i = 0x04;
		else						P_COM3 = 1,	i = 0x08;
	}
	P2n_push_pull(segment);		//SEG有输出的IO, 设置成PUSH-PULL OUTPUT
	P1n_push_pull(i);			//COM有输出的IO, 设置成PUSH-PULL OUTPUT
	if(++DisplayCnt >= 8)	DisplayCnt = 0;
}

/********************** 显示时钟函数 ************************/
void	DisplayRTC(void)
{
	if(hour >= 10)	LED8[0] = hour / 10;
	else			LED8[0] = DIS_BLACK;
	LED8[1] = hour % 10;
	LED8[2] = DIS_;
	LED8[3] = minute / 10;
	LED8[4] = minute % 10;
	LED8[5] = DIS_;
	LED8[6] = second / 10;
	LED8[7] = second % 10;
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


/******************** 主函数 **************************/
void main(void)
{
	hour   = 12;	//初始化时间值
	minute = 0;
	second = 0;
	
	DisplayRTC();
	DisplayCnt = 0;
	
	while(1)
	{
		delay_ms(2);	//扫描一位时间
		LED_Driver();
		
		if(++msecond >= 500)	//1秒到
		{
			msecond = 0;
			RTC();
			DisplayRTC();
		}
	}
}



