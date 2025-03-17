
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

读写RTC, IC为PCF8563.

用STC的MCU的IO方式控制74HC595驱动8位数码管。

用户可以修改宏来选择时钟频率.
使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

用户可以在显示函数里修改成共阴或共阳.推荐尽量使用共阴数码管.

8位数码管显示时间(小时-分钟-秒).

行列扫描按键键码为17~32.

按键只支持单键按下, 不支持多键同时按下, 那样将会有不可预知的结果.

键按下超过1秒后,将以10键/秒的速度提供重键输出. 用户只需要检测KeyCode是否非0来判断键是否按下.

调整时间键:
键码17: 小时+.
键码18: 小时-.
键码19: 分钟+.
键码20: 分钟-.


******************************************/


#define MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"



/***********************************************************/

#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11


/****************************** 用户定义宏 ***********************************/


#define	Timer0_Reload	(65536UL -(MAIN_Fosc / 1000))		//Timer 0 中断频率, 1000次/秒

/*****************************************************************************/






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

u8 IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;	//行列键盘变量
u8	KeyHoldCnt;	//键按下计时
u8	KeyCode;	//给用户使用的键码, 1~16有效
u8	cnt50ms;

u8	hour,minute,second;	//RTC变量
u16	msecond;


/*************	本地函数声明	**************/
void	CalculateAdcKey(u16 adc);
void	IO_KeyScan(void);	//50ms call
void	WriteNbyte(u8 addr, u8 *p, u8 number);
void	ReadNbyte( u8 addr, u8 *p, u8 number);
void	DisplayRTC(void);
void	ReadRTC(void);
void	WriteRTC(void);


/****************  外部函数声明和外部变量声明 *****************/


/**********************************************/
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
	
	display_index = 0;
	
	AUXR = 0x80;	//Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;	//Timer0 interrupt enable
	TR0 = 1;	//Tiner0 run
	EA = 1;		//打开总中断
	
	for(i=0; i<8; i++)	LED8[i] = 0x10;	//上电消隐

	
	ReadRTC();
	F0 = 0;
	if(second >= 60)	F0 = 1;	//错误
	if(minute >= 60)	F0 = 1;	//错误
	if(hour   >= 24)	F0 = 1;	//错误
	if(F0)	//有错误, 默认12:00:00
	{
		second = 0;
		minute = 0;
		hour  = 12;
		WriteRTC();
	}

	DisplayRTC();
	LED8[2] = DIS_;
	LED8[5] = DIS_;

	KeyHoldCnt = 0;	//键按下计时
	KeyCode = 0;	//给用户使用的键码, 1~16有效

	IO_KeyState = 0;
	IO_KeyState1 = 0;
	IO_KeyHoldCnt = 0;
	cnt50ms = 0;
	
	while(1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			if(++msecond >= 1000)	//1秒到
			{
				msecond = 0;
				ReadRTC();
				DisplayRTC();
			}

			if(++cnt50ms >= 50)		//50ms扫描一次行列键盘
			{
				cnt50ms = 0;
				IO_KeyScan();
			}
			
			if(KeyCode != 0)		//有键按下
			{
				if(KeyCode == 17)	//hour +1
				{
					if(++hour >= 24)	hour = 0;
					WriteRTC();
					DisplayRTC();
				}
				if(KeyCode == 18)	//hour -1
				{
					if(--hour >= 24)	hour = 23;
					WriteRTC();
					DisplayRTC();
				}
				if(KeyCode == 19)	//minute +1
				{
					second = 0;
					if(++minute >= 60)	minute = 0;
					WriteRTC();
					DisplayRTC();
				}
				if(KeyCode == 20)	//minute -1
				{
					second = 0;
					if(--minute >= 60)	minute = 59;
					WriteRTC();
					DisplayRTC();
				}

				KeyCode = 0;
			}

		}
	}
} 
/**********************************************/

/********************** 显示时钟函数 ************************/
void	DisplayRTC(void)
{
	if(hour >= 10)	LED8[0] = hour / 10;
	else			LED8[0] = DIS_BLACK;
	LED8[1] = hour % 10;
	LED8[2] = DIS_;
	LED8[3] = minute / 10;
	LED8[4] = minute % 10;
	LED8[6] = second / 10;
	LED8[7] = second % 10;
}

/********************** 读RTC函数 ************************/
void	ReadRTC(void)
{
	u8	tmp[3];

	ReadNbyte(2, tmp, 3);
	second = ((tmp[0] >> 4) & 0x07) * 10 + (tmp[0] & 0x0f);
	minute = ((tmp[1] >> 4) & 0x07) * 10 + (tmp[1] & 0x0f);
	hour   = ((tmp[2] >> 4) & 0x03) * 10 + (tmp[2] & 0x0f);
}

/********************** 写RTC函数 ************************/
void	WriteRTC(void)
{
	u8	tmp[3];

	tmp[0] = ((second / 10) << 4) + (second % 10);
	tmp[1] = ((minute / 10) << 4) + (minute % 10);
	tmp[2] = ((hour / 10) << 4) + (hour % 10);
	WriteNbyte(2, tmp, 3);
}


/*****************************************************
	行列键扫描程序
	使用XY查找4x4键的方法，只能单键，速度快

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/


u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

void IO_KeyDelay(void)
{
	u8 i;
	i = 60;
	while(--i)	;
}

void	IO_KeyScan(void)	//50ms call
{
	u8	j;

	j = IO_KeyState1;	//保存上一次状态

	P0 = 0xf0;	//X低，读Y
	IO_KeyDelay();
	IO_KeyState1 = P0 & 0xf0;

	P0 = 0x0f;	//Y低，读X
	IO_KeyDelay();
	IO_KeyState1 |= (P0 & 0x0f);
	IO_KeyState1 ^= 0xff;	//取反
	
	if(j == IO_KeyState1)	//连续两次读相等
	{
		j = IO_KeyState;
		IO_KeyState = IO_KeyState1;
		if(IO_KeyState != 0)	//有键按下
		{
			F0 = 0;
			if(j == 0)	F0 = 1;	//第一次按下
			else if(j == IO_KeyState)
			{
				if(++IO_KeyHoldCnt >= 20)	//1秒后重键
				{
					IO_KeyHoldCnt = 18;
					F0 = 1;
				}
			}
			if(F0)
			{
				j = T_KeyTable[IO_KeyState >> 4];
				if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
					KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;	//计算键码，17~32
			}
		}
		else	IO_KeyHoldCnt = 0;
	}
	P0 = 0xff;
}




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



#define SLAW	0xA2
#define SLAR	0xA3

sbit	SDA	= P1^1;	//定义SDA  PIN5
sbit	SCL	= P1^0;	//定义SCL  PIN6

/****************************/
void	I2C_Delay(void)	//for normal MCS51,	delay (2 * dly + 4) T, for STC12Cxxxx delay (4 * dly + 10) T
{
	u8	dly;
	dly = MAIN_Fosc / 2000000UL;		//按2us计算
	while(--dly)	;
}

/****************************/
void I2C_Start(void)               //start the I2C, SDA High-to-low when SCL is high
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SDA = 0;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}		


void I2C_Stop(void)					//STOP the I2C, SDA Low-to-high when SCL is high
{
	SDA = 0;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SDA = 1;
	I2C_Delay();
}

void S_ACK(void)              //Send ACK (LOW)
{
	SDA = 0;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}

void S_NoACK(void)           //Send No ACK (High)
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}
		
void I2C_Check_ACK(void)         //Check ACK, If F0=0, then right, if F0=1, then error
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	F0  = SDA;
	SCL = 0;
	I2C_Delay();
}

/****************************/
void I2C_WriteAbyte(u8 dat)		//write a byte to I2C
{
	u8 i;
	i = 8;
	do
	{
		if(dat & 0x80)	SDA = 1;
		else			SDA	= 0;
		dat <<= 1;
		I2C_Delay();
		SCL = 1;
		I2C_Delay();
		SCL = 0;
		I2C_Delay();
	}
	while(--i);
}

/****************************/
u8 I2C_ReadAbyte(void)			//read A byte from I2C
{
	u8 i,dat;
	i = 8;
	SDA = 1;
	do
	{
		SCL = 1;
		I2C_Delay();
		dat <<= 1;
		if(SDA)		dat++;
		SCL  = 0;
		I2C_Delay();
	}
	while(--i);
	return(dat);
}

/****************************/
void WriteNbyte(u8 addr, u8 *p, u8 number)			/*	WordAddress,First Data Address,Byte lenth	*/
                         									//F0=0,right, F0=1,error
{
	I2C_Start();
	I2C_WriteAbyte(SLAW);
	I2C_Check_ACK();
	if(!F0)
	{
		I2C_WriteAbyte(addr);
		I2C_Check_ACK();
		if(!F0)
		{
			do
			{
				I2C_WriteAbyte(*p);		p++;
				I2C_Check_ACK();
				if(F0)	break;
			}
			while(--number);
		}
	}
	I2C_Stop();
}


/****************************/
void ReadNbyte(u8 addr, u8 *p, u8 number)		/*	WordAddress,First Data Address,Byte lenth	*/
{
	I2C_Start();
	I2C_WriteAbyte(SLAW);
	I2C_Check_ACK();
	if(!F0)
	{
		I2C_WriteAbyte(addr);
		I2C_Check_ACK();
		if(!F0)
		{
			I2C_Start();
			I2C_WriteAbyte(SLAR);
			I2C_Check_ACK();
			if(!F0)
			{
				do
				{
					*p = I2C_ReadAbyte();	p++;
					if(number != 1)		S_ACK();	//send ACK
				}
				while(--number);
				S_NoACK();			//send no ACK
			}
		}
	}
	I2C_Stop();
}


/****************************/


