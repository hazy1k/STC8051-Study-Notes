
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

读ADC测量外部电压，使用外部TL431基准计算电压.

用STC的MCU的IO方式控制74HC595驱动8位数码管。

用户可以修改宏来选择时钟频率.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

右边4位数码管显示测量的电压值值.

外部电压从板上测温电阻两端输入, 输入电压0~VDD, 不要超过VDD或低于0V. 

实际项目使用请串一个1K的电阻到ADC输入口, ADC输入口再并一个电容到地.

******************************************/

#define MAIN_Fosc		22118400L	//定义主时钟

#include	"STC15Fxxxx.H"


/***********************************************************/

#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11

#define P1n_pure_input(bitn)		P1M1 |=  (bitn),	P1M0 &= ~(bitn)

/****************************** 用户定义宏 ***********************************/

	#define	Cal_MODE 	0	//每次测量只读1次ADC. 分辨率0.01V
//	#define	Cal_MODE 	1	//每次测量连续读16次ADC 再平均计算. 分辨率0.01V

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

u16	msecond;

u16	Bandgap;

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
	P1ASF = (1<<2) + (1<<3);	//P1.2 P1.3做ADC
	P1M1 |= (0xc0);	P1M0 &= ~(0xc0);	//P1.2 P1.3设置成高阻
	ADC_CONTR = 0xE0;			//90T, ADC power on
	
	Timer0_1T();
	Timer0_AsTimer();
	Timer0_16bitAutoReload();
	Timer0_Load(Timer0_Reload);
	Timer0_InterruptEnable();
	Timer0_Run();
	EA = 1;						//打开总中断
	
	for(i=0; i<8; i++)	LED8[i] = 0x10;	//上电消隐

	
	while(1)
	{
		if(B_1ms)	//1ms到
		{
			B_1ms = 0;
			if(++msecond >= 300)	//300ms到
			{
				msecond = 0;

			#if (Cal_MODE == 0)
			//=================== 只读1次ADC, 10bit ADC. 分辨率0.01V ===============================
				Get_ADC10bitResult(2);				//通道改变, 先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
				Bandgap = Get_ADC10bitResult(2);	//读外部基准TL431对应的ADC
				Get_ADC10bitResult(3);				//通道改变, 先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
				j = Get_ADC10bitResult(3);			//读外部电压ADC
				j = (u16)((u32)j * 250 / Bandgap);	//计算外部电压, TL431电压为2.50V, 测电压分辨率0.01V
			#endif
			//==========================================================================

			//===== 连续读16次ADC 再平均计算. 分辨率0.01V =========
			#if (Cal_MODE == 1)
				Get_ADC10bitResult(2);				//通道改变, 先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
				for(j=0, i=0; i<16; i++)
				{
					j += Get_ADC10bitResult(2);		//读外部基准TL431对应的ADC
				}
				Bandgap = j >> 4;	//16次平均

				Get_ADC10bitResult(3);				//通道改变, 先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
				for(j=0, i=0; i<16; i++)
				{
					j += Get_ADC10bitResult(3);		//读外部电压ADC
				}
				j = j >> 4;	//16次平均
				j = (u16)((u32)j * 250 / Bandgap);	//计算外部电压, TL431电压为2.50V, 测电压分辨率0.01V
			#endif
			//==========================================================================
		
				LED8[5] = j / 100 + DIS_DOT;	//显示外部电压值
				LED8[6] = (j % 100) / 10;
				LED8[7] = j % 10;

			//	j = Bandgap;
			//	LED8[0] = j / 1000;		//显示Bandgap ADC值
			//	LED8[1] = (j % 1000) / 100;
			//	LED8[2] = (j % 100) / 10;
			//	LED8[3] = j % 10;
		
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


