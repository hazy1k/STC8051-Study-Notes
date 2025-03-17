
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


用户可以在宏定义中改变MCU主时钟频率. 范围 8MHZ ~ 33MHZ.

红外接收程序。模拟市场上用量最大的NEC的编码。

用户可以在宏定义中指定用户码.

使用PCA2高速输出产生38KHZ载波, 1/3占空比, 每个38KHZ周期发射管发射9us,关闭16.3us.

使用开发板上的16个IO扫描按键, MCU不睡眠, 连续扫描按键.

当键按下, 第一帧为数据, 后面的帧为重复争,不带数据, 具体定义请自行参考NEC的编码资料.

键释放后, 停止发送.

******************************************/

#define 	MAIN_Fosc		22118400UL	//定义主时钟

#include	"STC15Fxxxx.H"


/*************	红外发送相关变量	**************/
#define	User_code	0xFF00		//定义红外用户码

sbit	P_IR_TX   = P3^7;	//定义红外发送端口

u16		PCA_Timer2;	//PCA2软件定时器变量
bit		B_Space;	//发送空闲(延时)标志
u16		tx_cnt;		//发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
u8		TxTime;		//发送时间


/*************	IO口定义	**************/
sbit	P_HC595_SER   = P4^0;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P5^4;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P4^3;	//pin 11	SRCLK	Shift data clock

/*************	IO键盘变量声明	**************/

u8	IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;	//行列键盘变量
u8	KeyHoldCnt;	//键按下计时
u8	KeyCode;	//给用户使用的键码, 1~16有效


/*************	本地函数声明	**************/
void	delay_ms(u8 ms);
void	DisableHC595(void);
void	IO_KeyScan(void);
void	PCA_config(void);
void	IR_TxPulse(u16 pulse);
void	IR_TxSpace(u16 pulse);
void	IR_TxByte(u8 dat);




/********************** 主函数 ************************/
void main(void)
{
	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口
	
	PCA_config();
	
	EA = 1;						//打开总中断
	
	DisableHC595();		//禁止掉学习板上的HC595显示，省电

	while(1)
	{
		delay_ms(30);		//30ms
		IO_KeyScan();

		if(KeyCode != 0)		//检测到键码
		{
			TxTime = 0;
								//一帧数据最小长度 = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
								//一帧数据最大长度 = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
			IR_TxPulse(342);	//对应9ms，同步头		9ms
			IR_TxSpace(171);	//对应4.5ms，同步头间隔	4.5ms
			IR_TxPulse(21);		//开始发送数据			0.5625ms

			IR_TxByte(User_code%256);	//发用户码高字节
			IR_TxByte(User_code/256);	//发用户码低字节
			IR_TxByte(KeyCode);			//发数据
			IR_TxByte(~KeyCode);		//发数据反码
			
			if(TxTime < 56)		//一帧按最大77ms发送, 不够的话,补偿时间		108ms
			{
				TxTime = 56 - TxTime;
				TxTime = TxTime + TxTime / 8;
				delay_ms(TxTime);
			}
			delay_ms(31);

			while(IO_KeyState != 0)	//键未释放
			{
				IR_TxPulse(342);	//对应9ms，   同步头		9ms
				IR_TxSpace(86);		//对应2.25ms，同步头间隔	2.25ms
				IR_TxPulse(21);		//开始发送数据			 	0.5625ms
				delay_ms(96);
				IO_KeyScan();
			}
			KeyCode = 0;
		}
	}
} 
/**********************************************/

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
     u16 i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;   //13T per loop
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



/************* 发送脉冲函数 **************/
void	IR_TxPulse(u16 pulse)
{
	tx_cnt = pulse;
	B_Space = 0;	//发脉冲
	CCAPM2 = 0x48 | 0x04 | 0x01;	//工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	CR = 1;		//启动
	while(CR);
}

/************* 发送空闲函数 **************/
void	IR_TxSpace(u16 pulse)
{
	tx_cnt = pulse;
	B_Space = 1;	//空闲
	CCAPM2 = 0x48 | 0x01;	//工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	CR = 1;		//启动
	while(CR);
}


/************* 发送一个字节函数 **************/
void	IR_TxByte(u8 dat)
{
	u8 i;
	for(i=0; i<8; i++)
	{
		if(dat & 1)		IR_TxSpace(63),	TxTime += 2;	//数据1对应 1.6875 + 0.5625 ms 
		else			IR_TxSpace(21),	TxTime++;		//数据0对应 0.5625 + 0.5625 ms
		IR_TxPulse(21);			//脉冲都是0.5625ms
		dat >>= 1;				//下一个位
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
	AUXR1   = (AUXR1 & ~0x30) | 0x10;	//切换IO口, 0x00: P1.2 P1.1 P1.0 P3.7,  0x10: P3.4 P3.5 P3.6 P3.7, 0x20: P2.4 P2.5 P2.6 P2.7

	CCON = 0x00;	//清除所有中断标志
	CCAPM2  = 0x48+ 1;	//工作模式 + 允许中断 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	CCAPM2 |= 0x04;	//允许高速取反输出, 一般用在16位软件定时器
	PCA_Timer2 = 100;	//随便给一个小的初值
	CCAP2L = (u8)PCA_Timer2;			//将影射寄存器写入捕获寄存器，先写CCAPxL
	CCAP2H = (u8)(PCA_Timer2 >> 8);	//后写CCAPxH

	PPCA = 1;	//高优先级中断
	CMOD  = (CMOD  & ~0xe0) | 0x08;	//选择时钟源, 0x00: 12T, 0x02: 2T, 0x04: Timer0溢出, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
	CH = 0;
	CL = 0;
	CR = 0;
	tx_cnt = 2;
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
	if(!B_Space)	//发送载波
	{								//发送脉冲，交替装载TH0值，溢出时自动重装
		if(P_IR_TX)
		{
			PCA_Timer2 += D_38K_OFF;	//装载高电平时间	17.3us
			if(--tx_cnt == 0)	CR = 0;	//pulse has sent,	stop
		}
		else	PCA_Timer2 += D_38K_ON;	//装载低电平时间	9us
	}
	else	//发送暂停时间
	{
		PCA_Timer2 += D_38K_DUTY;	//装载周期时间	26.3us
		if(--tx_cnt == 0)	CR = 0;	//空闲时间
	}
	CCAP2L = (u8)PCA_Timer2;			//将影射寄存器写入捕获寄存器，先写CCAP0L
	CCAP2H = (u8)(PCA_Timer2 >> 8);	//后写CCAP0H
}

