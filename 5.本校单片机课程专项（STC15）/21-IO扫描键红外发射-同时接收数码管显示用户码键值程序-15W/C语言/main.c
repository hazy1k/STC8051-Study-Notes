
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

用户可以在宏定义中指定用户码.

用户底层程序按固定的时间间隔(60~125us)调用 "IR_RX_NEC()"函数.


按下IO行列键（不支持ADC键盘），显示发送、接收到的键值。

******************************************/

#define MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"

sbit	P_IR_TX   = P3^7;	//定义红外发送脚

/****************************** 用户定义宏 ***********************************/

#define	SysTick 14225	// 次/秒, 系统滴答频率, 在4000~16000之间

/***********************************************************/

#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11



/****************************** 自动定义宏 ***********************************/

#define	Timer0_Reload	(65536UL - ((MAIN_Fosc + SysTick/2) / SysTick))		//Timer 0 中断频率, 在config.h中指定系统滴答频率, 例程为14225.

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


u8	IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;	//行列键盘变量
u8	KeyHoldCnt;	//键按下计时
u8	KeyCode;	//给用户使用的键码, 1~16有效
u8	cnt_27ms;

u16		PCA_Timer0;



/*************	红外发送程序变量声明	**************/
u8	IR_TxIndex;			//发送步骤索引, 用户把数据准备好, 用 IR_TxIndex = 1; 来启动,之后程序自动发送完成
u8	IR_TxData[5];		//发送数据, IR_TxData[0] -- 用户码低字节, IR_TxData[1] -- 用户码高字节, 
						//			IR_TxData[2] -- 用户数据字节, IR_TxData[3] -- 数据字节反码,  IR_TxData[4]留空.

u8	IR_Tx_Tick;			//(系统使用,用户不可操作) 发送时隙, 做0.5625ms延时计数
u8	IR_TxPulseTime;		//(系统使用,用户不可操作) 发送38KHZ时间
u8	IR_TxSpaceTime;		//(系统使用,用户不可操作) 发送空格时间
u8	IR_TxTmp;			//(系统使用,用户不可操作) 发送缓冲
u8	IR_TxBitCnt;		//(系统使用,用户不可操作) 发送bit计数
u8	IR_TxFrameTime;		//(系统使用,用户不可操作) 帧时间, 108ms
bit	B_StopCR;			//(系统使用,用户不可操作) 停止PCA
bit	B_Space;			//发送空闲(延时)标志
u16	tx_cnt;				//发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us

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

/*************	本地函数声明	**************/
void	IO_KeyScan(void);
void	PCA_config(void);



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
	EA = 1;		//打开总中断
	
	B_StopCR = 0;
	PCA_config();
	
	
	for(i=0; i<8; i++)	LED8[i] = DIS_;	//上电显示-
	LED8[4] = DIS_BLACK;
	LED8[5] = DIS_BLACK;

	while(1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			if(++cnt_27ms >= 27)
			{
				cnt_27ms = 0;
				IO_KeyScan();

				if(KeyCode > 0)		//检测到收到红外键码
				{
					IR_TxData[0] = 0x00;		//IR_TxData[0] -- 用户码低字节
					IR_TxData[1] = 0xFF;		//IR_TxData[1] -- 用户码高字节
					IR_TxData[2] = KeyCode;		//IR_TxData[2] -- 用户数据字节
					IR_TxData[3] = ~KeyCode;	//IR_TxData[3] -- 数据字节反码
					IR_TxIndex = 1;				//启动,之后程序自动发送完成
					KeyCode = 0;
				}
			}

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
				if(++IO_KeyHoldCnt >= 37)	//1秒后重键
				{
					IO_KeyHoldCnt = 33;		//108ms repeat
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


//*******************************************************************
//*********************** IR Remote Module **************************
//*********************** By 梁深 (Coody) 2002-8-24 *****************
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


/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt TIMER0_VECTOR
{
	IR_RX_NEC();

	if(IR_TxIndex > 0)	//有发送任务, 用户把数据准备好, 用 IR_TxIndex = 1; 来启动,之后程序自动发送完成
	{
		if(++IR_Tx_Tick >= 8)	//8*70.3=562.4us, 接近标准的 256 / 0.455 = 562.6us, 一般遥控器使用455K晶体(0.455MHZ)
		{
			IR_Tx_Tick = 0;
			if(IR_TxPulseTime > 0)	//发送时间未完
			{
				if(--IR_TxPulseTime == 0)	B_StopCR = 1;	//脉冲时间发完, 停止发送脉冲
			}
			else if(IR_TxSpaceTime > 0)	IR_TxSpaceTime--;	//空格时间未完
		
			if((IR_TxPulseTime | IR_TxSpaceTime) == 0)		//一个脉冲发完了
			{
				if(IR_TxIndex == 1)		//刚开始, 发送数据同步头, IR_TxPulseTime = 9ms, IR_TxSpaceTime = 4.5ms
				{
					CL = 0;
					CH = 0;
					PCA_Timer0 = 10;
					CCAP0L = 10;			//将影射寄存器写入捕获寄存器，先写CCAP0L
					CCAP0H = 0;	//后写CCAP0H
					CR = 1;

					IR_TxPulseTime = 16;	//16*0.5625 = 9ms
					IR_TxSpaceTime = 8;		// 8*0.5625 = 4.5ms
					IR_TxIndex = 2;
					IR_TxTmp = IR_TxData[0];	//取第一个数据
					IR_TxBitCnt = 0;
				}
				else if(IR_TxIndex == 2)	//发送数据, 0为IR_TxPulseTime = 0.5625ms, IR_TxSpaceTime = 0.5625ms
				{							//发送数据, 1为IR_TxPulseTime = 0.5625ms, IR_TxSpaceTime = 1.6875ms
					CL = 0;
					CH = 0;
					PCA_Timer0 = 10;
					CCAP0L = 10;			//将影射寄存器写入捕获寄存器，先写CCAP0L
					CCAP0H = 0;	//后写CCAP0H
					CR = 1;
					
					IR_TxPulseTime = 1;
					if(IR_TxTmp & 1)	IR_TxSpaceTime = 3;	//数据1对应 1.6875 + 0.5625 ms 
					else				IR_TxSpaceTime = 1;	//数据0对应 0.5625 + 0.5625 ms
					IR_TxTmp >>= 1;
					if(++IR_TxBitCnt >= 33)		//4个字节发送完成
					{
						IR_TxIndex = 3;			//下一步就结束了
					}
					else if((IR_TxBitCnt & 7) == 0)	IR_TxTmp = IR_TxData[IR_TxBitCnt >> 3];	//发完一个字节, 取一个数
				}
				else if(IR_TxIndex == 3)	IR_TxIndex = 0;	//发送完成了
			}
		}
	}
	
	if(--cnt_1ms == 0)
	{
		cnt_1ms = SysTick / 1000;
		B_1ms = 1;		//1ms标志
		DisplayScan();	//1ms扫描显示一位
	}
}


//========================================================================
// 函数: void	PCA_config(void)
// 描述: PCA配置函数.
// 参数: None
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	PCA_config(void)
{
	//  PCA0初始化
	CCAPM0  = 0x48 + 1;	//工作模式 + 中断允许 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	PCA_Timer0 = 100;	//随便给一个小的初值
	CCAP0L = (u8)PCA_Timer0;			//将影射寄存器写入捕获寄存器，先写CCAP0L
	CCAP0H = (u8)(PCA_Timer0 >> 8);	//后写CCAP0H

	PPCA = 1;	//高优先级中断
	CMOD = (CMOD  & ~0xe0) | 0x08;	//选择时钟源, 0x00: 12T, 0x02: 2T, 0x04: Timer0溢出, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
	CH = 0;
	CL = 0;
	CR = 0;
	P_IR_TX = 1;	//停止发射
}

//========================================================================
// 函数: void	PCA_Handler (void) interrupt PCA_VECTOR
// 描述: PCA中断处理程序.
// 参数: None
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================

#define	D_38K_DUTY	((MAIN_Fosc * 26) / 1000000UL + MAIN_Fosc / 3000000UL)	/* 	38KHZ周期时间	26.3us */
#define	D_38K_OFF	((MAIN_Fosc * 17) / 1000000UL + MAIN_Fosc / 3000000UL)	/* 发射管关闭时间	17.3us */
#define	D_38K_ON	((MAIN_Fosc * 9) / 1000000UL)							/* 发射管导通时间	9us */

void	PCA_Handler (void) interrupt PCA_VECTOR
{
	CCON = 0x40;	//清除所有中断标志,但不关CR
	P_IR_TX = ~P_IR_TX;
	if(P_IR_TX)		PCA_Timer0 += D_38K_OFF;//装载高电平时间	17.3us
	else			PCA_Timer0 += D_38K_ON;	//装载低电平时间	9us
	CCAP0L = (u8)PCA_Timer0;				//将影射寄存器写入捕获寄存器，先写CCAP0L
	CCAP0H = (u8)(PCA_Timer0 >> 8);			//后写CCAP0H
	if(B_StopCR)
	{
		CR = 0;
		B_StopCR = 0;
		P_IR_TX = 1;
	}
}


