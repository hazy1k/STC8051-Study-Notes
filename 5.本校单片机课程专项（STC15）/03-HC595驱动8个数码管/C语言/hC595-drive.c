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

用STC的MCU的IO方式控制74HC595驱动8位数码管。

显示效果为: 数码时钟.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

******************************************/

#define 	MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"

/****************************** 用户定义宏 ***********************************/

#define	Timer0_Reload	(65536UL -(MAIN_Fosc / 1000))		//Timer 0 中断频率, 1000次/秒

/*****************************************************************************/


#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11




/*************	本地常量声明	**************/
u8 code t_display[]={						//标准字库
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//位码


/*************	IO口定义	**************/
sbit	P_HC595_SER   = P4^0;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P5^4;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P4^3;	//pin 11	SRCLK	Shift data clock

/*************	本地变量声明	**************/

u8 	LED8[8];		//显示缓冲
u8	display_index;	//显示位索引
bit	B_1ms;			//1ms标志

u8	hour,minute,second;
u16	msecond;

/*************	本地函数声明	**************/


/****************  外部函数声明和外部变量声明 *****************/


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


/********************** 主函数 ************************/
void main(void)
{
	u8	i,k;
	
	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	AUXR = 0x80;	//Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;	//Timer0 interrupt enable
	TR0 = 1;	//Tiner0 run
	EA = 1;		//打开总中断
	
	display_index = 0;
	hour   = 11;	//初始化时间值
	minute = 59;
	second = 58;
	RTC();
	DisplayRTC();
	
//	for(i=0; i<8; i++)	LED8[i] = DIS_BLACK;	//上电消隐
	for(i=0; i<8; i++)	LED8[i] = i;	//显示01234567
	k = 0;

	while(1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			if(++msecond >= 1000)	//1秒到
			{
				msecond = 0;
				RTC();
				DisplayRTC();
			}

		}
	}
} 
/**********************************************/


/**************** 向HC595发送一个字节函数 ******************/
void Send_595(u8 dat)
{		
	u8	i;
	for(i=0; i<8; i++)
	{
		dat <<= 1;
		P_HC595_SER   = CY;
		P_HC595_SRCLK = 1;
		P_HC595_SRCLK = 0;
	}
}

/********************** 显示扫描函数 ************************/
void DisplayScan(void)
{	
	Send_595(~T_COM[display_index]);				//输出位码
	Send_595(t_display[LED8[display_index]]);	//输出段码

	P_HC595_RCLK = 1;
	P_HC595_RCLK = 0;							//锁存输出数据
	if(++display_index >= 8)	display_index = 0;	//8位结束回0
}


/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt TIMER0_VECTOR
{
	DisplayScan();	//1ms扫描显示一位
	B_1ms = 1;		//1ms标志

}


