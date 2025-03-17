
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

红外接收程序。适用于市场上用量最大的NEC编码。

应用层查询 B_IR_Press标志为,则已接收到一个键码放在IR_code中, 处理完键码后， 用户程序清除B_IR_Press标志.

数码管左起4位显示用户码, 最右边两位显示数据, 均为十六进制.

******************************************/

#define MAIN_Fosc	22118400L	//定义主时钟
#define	SysTick 	10000		// 次/秒, 系统滴答频率, 在4000~16000之间

#include	"STC15Fxxxx.H"


/***********************************************************/

#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11


/****************************** 自动定义宏 ***********************************/

#define	Timer0_Reload	(65536UL - ((MAIN_Fosc + SysTick/2) / SysTick))		//Timer 0 中断频率, 在config.h中指定系统滴答频率, 在4000~16000之间.

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

u8	cnt_1ms;		//1ms基本计时


/*************	红外接收程序变量声明	**************/
sbit	P_IR_RX	= P3^6;			//定义红外接收输入IO口

u8	IR_SampleCnt;		//采样计数
u8	IR_BitCnt;			//编码位数
u8	IR_UserH;			//用户码(地址)高字节
u8	IR_UserL;			//用户码(地址)低字节
u8	IR_data;			//数据原码
u8	IR_DataShit;		//数据移位

bit	P_IR_RX_temp;		//Last sample
bit	B_IR_Sync;			//已收到同步标志
bit	B_IR_Press;			//安键动作发生
u8	IR_code;			//红外键码
u16	UserCode;			//用户码

/*********************************/



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
	
	display_index = 0;

	AUXR = 0x80;	//Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;	//Timer0 interrupt enable
	TR0 = 1;	//Tiner0 run

	cnt_1ms = SysTick / 1000;
	EA = 1;		//打开总中断
	
	
	for(i=0; i<8; i++)	LED8[i] = DIS_;
	LED8[4] = DIS_BLACK;
	LED8[5] = DIS_BLACK;

	while(1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			
			if(B_IR_Press)		//检测到收到红外键码
			{
				B_IR_Press = 0;
				
				LED8[0] = (u8)((UserCode >> 12) & 0x0f);	//用户码高字节的高半字节
				LED8[1] = (u8)((UserCode >> 8)  & 0x0f);	//用户码高字节的低半字节
				LED8[2] = (u8)((UserCode >> 4)  & 0x0f);	//用户码低字节的高半字节
				LED8[3] = (u8)(UserCode & 0x0f);			//用户码低字节的低半字节
				LED8[6] = IR_code >> 4;
				LED8[7] = IR_code & 0x0f;
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



//*******************************************************************
//*********************** IR Remote Module **************************
//*********************** By  (Coody) 2002-8-24 *********************
//*********************** IR Remote Module **************************
//this programme is used for Receive IR Remote (NEC Code).

//data format: Synchro, AddressH, AddressL, data, /data, (total 32 bit).

//send a frame(85ms), pause 23ms, send synchro of continue frame, pause 94ms

//data rate: 108ms/Frame


//Synchro: low=9ms, high=4.5 / 2.25ms, low=0.5626ms
//Bit0: high=0.5626ms, low=0.5626ms
//Bit1: high=1.6879ms, low=0.5626ms
//frame rate = 108ms ( pause 23 ms or 96 ms)

/******************** 红外采样时间宏定义, 用户不要随意修改	*******************/

#define IR_SAMPLE_TIME		(1000000UL/SysTick)			//查询时间间隔, us, 红外接收要求在60us~250us之间
#if ((IR_SAMPLE_TIME <= 250) && (IR_SAMPLE_TIME >= 60))
	#define	D_IR_sample			IR_SAMPLE_TIME		//定义采样时间，在60us~250us之间
#endif

#define D_IR_SYNC_MAX		(15000/D_IR_sample)	//SYNC max time
#define D_IR_SYNC_MIN		(9700 /D_IR_sample)	//SYNC min time
#define D_IR_SYNC_DIVIDE	(12375/D_IR_sample)	//decide data 0 or 1
#define D_IR_DATA_MAX		(3000 /D_IR_sample)	//data max time
#define D_IR_DATA_MIN		(600  /D_IR_sample)	//data min time
#define D_IR_DATA_DIVIDE	(1687 /D_IR_sample)	//decide data 0 or 1
#define D_IR_BIT_NUMBER		32					//bit number

//*******************************************************************************************
//**************************** IR RECEIVE MODULE ********************************************

void IR_RX_NEC(void)
{
	u8	SampleTime;

	IR_SampleCnt++;							//Sample + 1

	F0 = P_IR_RX_temp;						//Save Last sample status
	P_IR_RX_temp = P_IR_RX;					//Read current status
	if(F0 && !P_IR_RX_temp)					//Pre-sample is high，and current sample is low, so is fall edge
	{
		SampleTime = IR_SampleCnt;			//get the sample time
		IR_SampleCnt = 0;					//Clear the sample counter

			 if(SampleTime > D_IR_SYNC_MAX)		B_IR_Sync = 0;	//large the Maxim SYNC time, then error
		else if(SampleTime >= D_IR_SYNC_MIN)					//SYNC
		{
			if(SampleTime >= D_IR_SYNC_DIVIDE)
			{
				B_IR_Sync = 1;					//has received SYNC
				IR_BitCnt = D_IR_BIT_NUMBER;	//Load bit number
			}
		}
		else if(B_IR_Sync)						//has received SYNC
		{
			if(SampleTime > D_IR_DATA_MAX)		B_IR_Sync=0;	//data samlpe time too large
			else
			{
				IR_DataShit >>= 1;					//data shift right 1 bit
				if(SampleTime >= D_IR_DATA_DIVIDE)	IR_DataShit |= 0x80;	//devide data 0 or 1
				if(--IR_BitCnt == 0)				//bit number is over?
				{
					B_IR_Sync = 0;					//Clear SYNC
					if(~IR_DataShit == IR_data)		//判断数据正反码
					{
						UserCode = ((u16)IR_UserH << 8) + IR_UserL;
						IR_code      = IR_data;
						B_IR_Press   = 1;			//数据有效
					}
				}
				else if((IR_BitCnt & 7)== 0)		//one byte receive
				{
					IR_UserL = IR_UserH;			//Save the User code high byte
					IR_UserH = IR_data;				//Save the User code low byte
					IR_data  = IR_DataShit;			//Save the IR data byte
				}
			}
		}
	}
}


/********************** Timer0中断函数 ************************/
void timer0 (void) interrupt TIMER0_VECTOR
{

	IR_RX_NEC();
	if(--cnt_1ms == 0)
	{
		cnt_1ms = SysTick / 1000;
		B_1ms = 1;		//1ms标志
		DisplayScan();	//1ms扫描显示一位
	}

}


