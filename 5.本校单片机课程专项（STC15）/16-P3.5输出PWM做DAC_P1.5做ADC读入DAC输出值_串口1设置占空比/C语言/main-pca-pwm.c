
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


/*************	功能说明	**************

使用PCA0从P3.5输出8位的PWM, 输出的PWM经过RC滤波成直流电压送P1.5做ADC并用数码管显示出来.

串口1配置为115200bps, 8,n, 1, 切换到P3.0 P3.1, 下载后就可以直接测试. 主时钟为22.1184MHZ, 通过串口1设置占空比.

串口命令使用ASCII码的数字，比如： 10，就是设置占空比为10/256， 100： 就是设置占空比为100/256。

可以设置的值为0~256, 0为连续低电平, 256为连续高电平.

左边4位数码管显示PWM的占空比值，右边4位数码管显示ADC值。

******************************************/

#define 	MAIN_Fosc			22118400L	//定义主时钟

#include	"STC15Fxxxx.H"


/****************************** 用户定义宏 ***********************************/
#define		LED_TYPE	0x00	//定义LED类型, 0x00--共阴, 0xff--共阳

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

u8	cnt200ms;


#define		Baudrate1			115200L

#define		UART1_BUF_LENGTH	128		//串口缓冲长度

u8	RX1_TimeOut;
u8	TX1_Cnt;	//发送计数
u8	RX1_Cnt;	//接收计数
bit	B_TX1_Busy;	//发送忙标志

u8 	xdata 	RX1_Buffer[UART1_BUF_LENGTH];	//接收缓冲

bit		B_Capture0,B_Capture1,B_Capture2;
u8		PCA0_mode,PCA1_mode,PCA2_mode;
u16		CCAP0_tmp,PCA_Timer0;
u16		CCAP1_tmp,PCA_Timer1;
u16		CCAP2_tmp,PCA_Timer2;


void	UART1_config(u8 brt);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void	PrintString1(u8 *puts);
void	UART1_TxByte(u8 dat);
void	UpdatePwm(u16 pwm_value);


/*************	本地变量声明	**************/

u16	adc;

u16		Get_ADC10bitResult(u8 channel);	//channel = 0~7





/******************** 住函数 **************************/
void main(void)
{
	u8	i;
	u16	j;
	
	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	display_index = 0;

	//  Timer0初始化
	AUXR = 0x80;	//Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;	//Timer0 interrupt enable
	TR0 = 1;	//Tiner0 run

	//  PCA0初始化
	AUXR1 &= ~0x30;
	AUXR1 |= 0x10;	//切换IO口, 0x00: P1.2 P1.1 P1.0 P3.7,  0x10: P3.4 P3.5 P3.6 P3.7, 0x20: P2.4 P2.5 P2.6 P2.7
	CCAPM0   = 0x42;	//工作模式 PWM
	PCA_PWM0 = (PCA_PWM0 & ~0xc0) | 0x00;	//PWM宽度, 0x00: 8bit, 0x40: 7bit,  0x80: 6bit
	CMOD  = (CMOD  & ~0xe0) | 0x08;	//选择时钟源, 0x00: 12T, 0x02: 2T, 0x04: Timer0溢出, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
	CR = 1;
	UpdatePwm(128);

	//  ADC初始化
	P1ASF = (1 << 5);	//P1.5做ADC
	ADC_CONTR = 0xE0;	//90T, ADC power on

	UART1_config(1);	// 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	EA = 1;		//打开总中断
	
	for(i=0; i<8; i++)	LED8[i] = DIS_;	//上电全部显示-
	
	LED8[0] = 1;	//显示PWM默认值
	LED8[1] = 2;
	LED8[2] = 8;
	LED8[3] = DIS_BLACK;	//这位不显示

	PrintString1("PWM和ADC测试程序, 输入占空比为 0~256!\r\n");	//SUART1发送一个字符串
	
	while (1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			if(++cnt200ms >= 200)	//200ms读一次ADC
			{
				cnt200ms = 0;
				j = Get_ADC10bitResult(5);	//参数0~7,查询方式做一次ADC, 返回值就是结果, == 1024 为错误
				if(j >= 1000)	LED8[4] = j / 1000;		//显示按键ADC值
				else			LED8[4] = DIS_BLACK;
				LED8[5] = (j % 1000) / 100;
				LED8[6] = (j % 100) / 10;
				LED8[7] = j % 10;
			}

			if(RX1_TimeOut > 0)		//超时计数
			{
				if(--RX1_TimeOut == 0)
				{
					if((RX1_Cnt > 0) && (RX1_Cnt <= 3))	//限制为3位数字
					{
						F0 = 0;	//错误标志
						j = 0;
						for(i=0; i<RX1_Cnt; i++)
						{
							if((RX1_Buffer[i] >= '0') && (RX1_Buffer[i] <= '9'))	//限定为数字
							{
								j = j * 10 + RX1_Buffer[i] - '0';
							}
							else
							{
								F0 = 1;	//接收到非数字字符, 错误
								PrintString1("错误! 接收到非数字字符! 占空比为0~256!\r\n");
								break;
							}
						}
						if(!F0)
						{
							if(j > 256)	PrintString1("错误! 输入占空比过大, 请不要大于256!\r\n");
							else
							{
								UpdatePwm(j);
								if(j >= 100)	LED8[0] = j / 100,	j %= 100;	//显示PWM默认值
								else			LED8[0] = DIS_BLACK;
								LED8[1] = j % 100 / 10;
								LED8[2] = j % 10;
								PrintString1("已更新占空比!\r\n");
							}
						}
					}
					else  PrintString1("错误! 输入字符过多! 输入占空比为0~256!\r\n");
					RX1_Cnt = 0;
				}
			}
		}
	}
}


/**********************************************/


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
    for (; *puts != 0;	puts++)   UART1_TxByte(*puts);	//遇到停止符0结束
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
	Send_595(~LED_TYPE ^ T_COM[display_index]);				//输出位码
	Send_595( LED_TYPE ^ t_display[LED8[display_index]]);	//输出段码

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


//========================================================================
// 函数: u16	Get_ADC10bitResult(u8 channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 10位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
u16	Get_ADC10bitResult(u8 channel)	//channel = 0~7
{
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | 0x08 | channel; 	//start the ADC
	NOP(4);

	while((ADC_CONTR & 0x10) == 0)	;	//wait for ADC finish
	ADC_CONTR &= ~0x10;		//清除ADC结束标志
	return	(((u16)ADC_RES << 2) | (ADC_RESL & 3));
}


//========================================================================
// 函数: UpdatePwm(u8 PCA_id, u16 pwm_value)
// 描述: 更新PWM值. 
// 参数: PCA_id: PCA序号. 取值 PCA0,PCA1,PCA2,PCA_Counter
//		 pwm_value: pwm值, 这个值是输出高电平的时间.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	UpdatePwm(u16 pwm_value)
{
	if(pwm_value == 0)			PWM0_OUT_0();	//输出连续低电平
	else						CCAP0H = (u8)(256 - pwm_value), PWM0_NORMAL();
}


