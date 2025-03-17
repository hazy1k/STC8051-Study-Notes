
/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-755-82905966 -------------------------------------------*/
/* --- Tel: 86-755-82948412 -------------------------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

测试使用STC15W408AS的ADC做的电容感应触摸键.

假定测试芯片的工作频率为24MHz

******************************************/



#include	<reg52.h>
#include	<intrins.h>

#define MAIN_Fosc		24000000UL	//定义主时钟

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define	Timer0_Reload	(65536UL -(MAIN_Fosc / 600000))		//Timer 0 重装值， 对应300KHZ

sfr P1ASF     = 0x9D;	//只写，模拟输入选择
sfr ADC_CONTR = 0xBC;	//带AD系列
sfr ADC_RES   = 0xBD;	//带AD系列
sfr ADC_RESL  = 0xBE;	//带AD系列
sfr	AUXR      = 0x8E;
sfr	AUXR2     = 0x8F;

sfr P1M1 = 0x91;	//P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull		实际上1T的都一样
sfr P1M0 = 0x92;	//					=10--->pure input,	11--->open drain
sfr P0M1 = 0x93;	//P0M1.n,P0M0.n 	=00--->Standard,	01--->push-pull
sfr P0M0 = 0x94;	//					=10--->pure input,	11--->open drain
sfr P2M1 = 0x95;	//P2M1.n,P2M0.n 	=00--->Standard,	01--->push-pull
sfr P2M0 = 0x96;	//					=10--->pure input,	11--->open drain
sfr P3M1  = 0xB1;	//P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
sfr P3M0  = 0xB2;	//					=10--->pure input,	11--->open drain
sfr P4M1  = 0xB3;	//P4M1.n,P4M0.n 	=00--->Standard,	01--->push-pull
sfr P4M0  = 0xB4;	//					=10--->pure input,	11--->open drain
sfr P5M1   = 0xC9;	//	P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
sfr P5M0   = 0xCA;	//					=10--->pure input,	11--->open drain
sfr P6M1   = 0xCB;	//	P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
sfr P6M0   = 0xCC;	//					=10--->pure input,	11--->open drain
sfr	P7M1   = 0xE1;
sfr	P7M0   = 0xE2;


/*************	本地常量声明	**************/

#define	TOUCH_CHANNEL	8	//ADC通道数

#define ADC_90T		(3<<5)	//ADC时间 90T
#define ADC_180T	(2<<5)	//ADC时间 180T
#define ADC_360T	(1<<5)	//ADC时间 360T
#define ADC_540T	0		//ADC时间 540T
#define ADC_FLAG	(1<<4)	//软件清0
#define ADC_START	(1<<3)	//自动清0

/*************	本地变量声明	**************/
sbit	P_LED7 = P2^7;
sbit	P_LED6 = P2^6;
sbit	P_LED5 = P2^5;
sbit	P_LED4 = P2^4;
sbit	P_LED3 = P2^3;
sbit	P_LED2 = P2^2;
sbit	P_LED1 = P2^1;
sbit	P_LED0 = P2^0;

u16	idata adc[TOUCH_CHANNEL];			//当前ADC值
u16	idata adc_prev[TOUCH_CHANNEL];		//上一个ADC值
u16	idata TouchZero[TOUCH_CHANNEL];		//0点ADC值
u8	idata TouchZeroCnt[TOUCH_CHANNEL];	//0点自动跟踪计数

u8	cnt_250ms;

/*************	本地函数声明	**************/
void	delay_ms(u8 ms);
void	ADC_init(void);
u16		Get_ADC10bitResult(u8 channel);
void	AutoZero(void);
u8		check_adc(u8 index);
void	ShowLED(void);



/******************** 主函数 **************************/
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

	delay_ms(50);

	ET0 = 0;		//初始化Timer0输出一个300KHZ时钟
	TR0 = 0;
	AUXR |=  0x80;	//Timer0 set as 1T mode
	AUXR2 |= 0x01;	//允许输出时钟
	TMOD = 0;		//Timer0 set as Timer, 16 bits Auto Reload.
	TH0 = (u8)(Timer0_Reload >> 8);
	TL0 = (u8)Timer0_Reload;
	TR0 = 1;

	ADC_init();		//ADC初始化
	delay_ms(50);	//延时50ms

	for(i=0; i<TOUCH_CHANNEL; i++)		//初始化0点和上一个值和0点自动跟踪计数
	{
		adc_prev[i]  = 1023;
		TouchZero[i] = 1023;
		TouchZeroCnt[i] = 0;
	}
	cnt_250ms = 0;

	while (1)
	{
		delay_ms(50);		//每隔50ms处理一次按键
		ShowLED();
		if(++cnt_250ms >= 5)
		{
			cnt_250ms = 0;
			AutoZero();		//每隔250ms处理一次0点自动跟踪
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
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;
     }while(--ms);
}

/*************  ADC初始化函数 *****************/
void	ADC_init(void)
{
	P1ASF = 0xff;		//8路ADC
	ADC_CONTR = 0x80;	//允许ADC
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
	ADC_RES  = 0;
	ADC_RESL = 0;
	ADC_CONTR = 0x80 | ADC_90T | ADC_START | channel; //触发ADC
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while((ADC_CONTR & ADC_FLAG) == 0)	;	//等待ADC转换结束
	ADC_CONTR = 0x80;		//清除标志
	return(((u16)ADC_RES << 2) | ((u16)ADC_RESL & 3));	//返回ADC结果
}



/********************* 自动0点跟踪函数 *************************/
void	AutoZero(void)		//250ms调用一次 这是使用相邻2个采样的差的绝对值之和来检测。
{
	u8	i;
	u16	j,k;

	for(i=0; i<TOUCH_CHANNEL; i++)		//处理8个通道
	{
		j = adc[i];
		k = j - adc_prev[i];			//减前一个读数
		F0 = 0;								//按下
		if(k & 0x8000)	F0 = 1,	k = 0 - k;	//释放	求出两次采样的差值
		if(k >= 20)	//变化比较大
		{
			TouchZeroCnt[i] = 0;			//如果变化比较大，则清0计数器
			if(F0)	TouchZero[i] = j;		//如果是释放，并且变化比较大，则直接替代
		}
		else		//变化比较小，则蠕动，自动0点跟踪
		{
			if(++TouchZeroCnt[i] >= 20)		//连续检测到小变化20次/4 = 5秒.
			{
				TouchZeroCnt[i] = 0;
				TouchZero[i] = adc_prev[i];	//变化缓慢的值作为0点
			}
		}
		adc_prev[i] = j;	//保存这一次的采样值
	}
}



/********************* 获取触摸信息函数 50ms调用1次 *************************/
u8	check_adc(u8 index)		//判断键按下或释放,有回差控制
{
	u16	delta;
	adc[index] = 1023 - Get_ADC10bitResult(index);	//获取ADC值, 转成按下键, ADC值增加
	if(adc[index] < TouchZero[index])	return	0;	//比0点还小的值，则认为是键释放
	delta = adc[index] - TouchZero[index];
	if(delta >= 40)	return 1;	//键按下
	if(delta <= 20)	return 0;	//键释放
	return	2;		//保持原状态
}


/********************* 键处理 50ms调用1次 *************************/
void	ShowLED(void)
{
	u8	i;
	
	i = check_adc(0);
	if(i == 0)		P_LED0 = 1;	//指示灯灭
	if(i == 1)		P_LED0 = 0;	//指示灯亮

	i = check_adc(1);
	if(i == 0)		P_LED1 = 1;	//指示灯灭
	if(i == 1)		P_LED1 = 0;	//指示灯亮

	i = check_adc(2);
	if(i == 0)		P_LED2 = 1;	//指示灯灭
	if(i == 1)		P_LED2 = 0;	//指示灯亮

	i = check_adc(3);
	if(i == 0)		P_LED3 = 1;	//指示灯灭
	if(i == 1)		P_LED3 = 0;	//指示灯亮

	i = check_adc(4);
	if(i == 0)		P_LED4 = 1;	//指示灯灭
	if(i == 1)		P_LED4 = 0;	//指示灯亮

	i = check_adc(5);
	if(i == 0)		P_LED5 = 1;	//指示灯灭
	if(i == 1)		P_LED5 = 0;	//指示灯亮

	i = check_adc(6);
	if(i == 0)		P_LED6 = 1;	//指示灯灭
	if(i == 1)		P_LED6 = 0;	//指示灯亮

	i = check_adc(7);
	if(i == 0)		P_LED7 = 1;	//指示灯灭
	if(i == 1)		P_LED7 = 0;	//指示灯亮

}


