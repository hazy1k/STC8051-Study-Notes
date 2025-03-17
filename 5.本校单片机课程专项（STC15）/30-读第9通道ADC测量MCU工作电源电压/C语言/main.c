
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

读ADC内部基准计算MCU的工作电压, 不需要增加任何硬件零件.

用STC的MCU的IO方式控制74HC595驱动8位数码管。

用户可以修改宏来选择时钟频率.

用户可以在"用户定义宏"中选择共阴或共阳. 推荐尽量使用共阴数码管.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

右边4位数码管显示MCU的工作电压值值.


******************************************/

#define MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"


/***********************************************************/

#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11

#define P1n_pure_input(bitn)		P1M1 |=  (bitn),	P1M0 &= ~(bitn)

/****************************** 用户定义宏 ***********************************/

#define		LED_TYPE	0x00		//定义LED类型, 0x00--共阴, 0xff--共阳

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

u8	msecond;

u16	Get_ADC10bitResult(u8 channel);	//channel = 0~7




/**********************************************/
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
	P1ASF = 0;	//对内部基准做ADC
	ADC_CONTR = 0xE0;			//90T, ADC power on
	
	AUXR = 0x80;	//Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;	//Timer0 interrupt enable
	TR0 = 1;	//Tiner0 run
	EA = 1;		//打开总中断
	
	for(i=0; i<8; i++)	LED8[i] = 0x10;	//上电消隐

	
	while(1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			if(++msecond >= 200)	//200ms到
			{
				msecond = 0;

				for(j=0,i=0; i<16; i++)		j += Get_ADC10bitResult(0);	//读内部基准ADC, P1ASF=0, 读0通道
				j = (u32)128000UL*16 / j;	// ADC = 1024 * Uref / Ux, 则 Ux = 1024 * Uref / ADC = 1024 * 1.25 / ADC = 1280 / ADC

				LED8[5] = j / 100 + DIS_DOT;	//显示MCU电压值, 计算时放大了100倍, 电压有两位小数.
				LED8[6] = (j % 100) / 10;
				LED8[7] = j % 10;
			}
		}
	}
} 
/**********************************************/

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


