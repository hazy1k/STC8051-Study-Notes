
/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-755-82905966 -------------------------------------------*/
/* --- Tel: 86-755-82948412 -------------------------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/


#include	<reg52.h>


/*************	功能说明	**************

输出3路9~16位PWM信号。

PWM频率 = MAIN_Fosc / PWM_DUTY, 假设 MAIN_Fosc = 24MHZ, PWM_DUTY = 6000, 则输出PWM频率为4000HZ.

******************************************/

/***************************用户宏定义*******************************************************/
#define 	MAIN_Fosc		24000000UL	//定义主时钟

#define		PWM_DUTY		6000		//定义PWM的周期，数值为PCA所选择的时钟脉冲个数。
#define		PWM_HIGH_MIN	80			//限制PWM输出的最小占空比, 避免中断里重装参数时间不够。
#define		PWM_HIGH_MAX	(PWM_DUTY - PWM_HIGH_MIN)		//限制PWM输出的最大占空比。

/********************************************************************************************/

#define	PCA0			0
#define	PCA1			1
#define	PCA2			2
#define	PCA_Counter		3
#define	PCA_P12_P11_P10_P37	(0<<4)
#define	PCA_P34_P35_P36_P37	(1<<4)
#define	PCA_P24_P25_P26_P27	(2<<4)
#define	PCA_Mode_PWM				0x42
#define	PCA_Mode_Capture			0
#define	PCA_Mode_SoftTimer			0x48
#define	PCA_Mode_HighPulseOutput	0x4c
#define	PCA_Clock_1T	(4<<1)
#define	PCA_Clock_2T	(1<<1)
#define	PCA_Clock_4T	(5<<1)
#define	PCA_Clock_6T	(6<<1)
#define	PCA_Clock_8T	(7<<1)
#define	PCA_Clock_12T	(0<<1)
#define	PCA_Clock_Timer0_OF	(2<<1)
#define	PCA_Clock_ECI	(3<<1)
#define	PCA_Rise_Active	(1<<5)
#define	PCA_Fall_Active	(1<<4)
#define	PCA_PWM_8bit	(0<<6)
#define	PCA_PWM_7bit	(1<<6)
#define	PCA_PWM_6bit	(2<<6)

#define		ENABLE		1
#define		DISABLE		0

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

sfr AUXR1 = 0xA2;
sfr CCON = 0xD8;
sfr CMOD = 0xD9;
sfr CCAPM0 = 0xDA;		//PCA模块0的工作模式寄存器。
sfr CCAPM1 = 0xDB;		//PCA模块1的工作模式寄存器。
sfr CCAPM2 = 0xDC;		//PCA模块2的工作模式寄存器。

sfr CL     = 0xE9;
sfr CCAP0L = 0xEA;		//PCA模块0的捕捉/比较寄存器低8位。
sfr CCAP1L = 0xEB;		//PCA模块1的捕捉/比较寄存器低8位。
sfr CCAP2L = 0xEC;		//PCA模块2的捕捉/比较寄存器低8位。

sfr CH     = 0xF9;
sfr CCAP0H = 0xFA;		//PCA模块0的捕捉/比较寄存器高8位。
sfr CCAP1H = 0xFB;		//PCA模块1的捕捉/比较寄存器高8位。
sfr CCAP2H = 0xFC;		//PCA模块2的捕捉/比较寄存器高8位。

sbit CCF0  = CCON^0;	//PCA 模块0中断标志，由硬件置位，必须由软件清0。
sbit CCF1  = CCON^1;	//PCA 模块1中断标志，由硬件置位，必须由软件清0。
sbit CCF2  = CCON^2;	//PCA 模块2中断标志，由硬件置位，必须由软件清0。
sbit CR    = CCON^6;	//1: 允许PCA计数器计数，0: 禁止计数。
sbit CF    = CCON^7;	//PCA计数器溢出（CH，CL由FFFFH变为0000H）标志。
                        //PCA计数器溢出后由硬件置位，必须由软件清0。
sbit PPCA  = IP^7;		//PCA 中断 优先级设定位

sfr P2M1 = 0x95;	//P2M1.n,P2M0.n 	=00--->Standard,	01--->push-pull
sfr P2M0 = 0x96;	//					=10--->pure input,	11--->open drain

//================================================================

sbit	P25 = P2^5;
sbit	P26 = P2^6;
sbit	P27 = P2^7;

u16		CCAP0_tmp,PWM0_high,PWM0_low;
u16		CCAP1_tmp,PWM1_high,PWM1_low;
u16		CCAP2_tmp,PWM2_high,PWM2_low;

u16	pwm0,pwm1,pwm2;

void	PWMn_Update(u8 PCA_id, u16 pwm);
void	PCA_Init(void);
void	delay_ms(u8 ms);

/******************** 主函数 **************************/
void main(void)
{
	
	PCA_Init();	//PCA初始化
	EA = 1;
	P2M1 &= ~(0xe0);	//P2.7 P2.6 P2.5 设置为推挽输出
	P2M0 |=  (0xe0);
	
	while (1)
	{
		delay_ms(2);

		if(++pwm0 >= PWM_HIGH_MAX)	pwm0 = PWM_HIGH_MIN;
		PWMn_Update(PCA0,pwm0);

		if(++pwm1 >= PWM_HIGH_MAX)	pwm1 = PWM_HIGH_MIN;
		PWMn_Update(PCA1,pwm1);

		if(++pwm2 >= PWM_HIGH_MAX)	pwm2 = PWM_HIGH_MIN;
		PWMn_Update(PCA2,pwm2);
	}
}


//========================================================================
// 函数: void  delay_ms(u8 ms)
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


//========================================================================
// 函数: void PWMn_SetHighReg(unsigned int high)
// 描述: 更新占空比数据。
// 参数: high: 	占空比数据，即PWM输出高电平的PCA时钟脉冲个数。
// 返回: 无
// 版本: VER1.0
// 日期: 2013-5-15
// 备注: 
//========================================================================
void PWMn_Update(u8 PCA_id, u16 pwm)
{
	if(pwm > PWM_HIGH_MAX)	pwm = PWM_HIGH_MAX;	//如果写入大于最大占空比数据，强制为最大占空比。
	if(pwm < PWM_HIGH_MIN)	pwm = PWM_HIGH_MIN;	//如果写入小于最小占空比数据，强制为最小占空比。

	if(PCA_id == PCA0)
	{
		CR = 0;						//停止PCA一会， 一般不会影响PWM。
		PWM0_high = pwm;			//数据在正确范围，则装入占空比寄存器。
		PWM0_low = PWM_DUTY - pwm;	//计算并保存PWM输出低电平的PCA时钟脉冲个数。
		CR = 1;						//启动PCA。
	}
	else if(PCA_id == PCA1)
	{
		CR = 0;						//停止PCA。
		PWM1_high = pwm;			//数据在正确范围，则装入占空比寄存器。
		PWM1_low = PWM_DUTY - pwm;	//计算并保存PWM输出低电平的PCA时钟脉冲个数。
		CR = 1;						//启动PCA。
	}
	else if(PCA_id == PCA2)
	{
		CR = 0;						//停止PCA。
		PWM2_high = pwm;			//数据在正确范围，则装入占空比寄存器。
		PWM2_low = PWM_DUTY - pwm;	//计算并保存PWM输出低电平的PCA时钟脉冲个数。
		CR = 1;						//启动PCA。
	}
}

//========================================================================
// 函数: void	PCA_Init(void)
// 描述: PCA初始化程序.
// 参数: none
// 返回: none.
// 版本: V1.0, 2013-11-22
//========================================================================
void	PCA_Init(void)
{
	CR = 0;
	AUXR1 = (AUXR1 & ~(3<<4)) | PCA_P24_P25_P26_P27;	//切换IO口
	CCAPM0 = (PCA_Mode_HighPulseOutput | ENABLE);	//16位软件定时、高速脉冲输出、中断模式
	CCAPM1 = (PCA_Mode_HighPulseOutput | ENABLE);
	CCAPM2 = (PCA_Mode_HighPulseOutput | ENABLE);

	CH = 0;
	CL = 0;
	CMOD  = (CMOD  & ~(7<<1)) | PCA_Clock_1T;			//选择时钟源
	PPCA  = 1;	// 高优先级中断

	pwm0 = (PWM_DUTY / 4 * 1);	//给PWM一个初值
	pwm1 = (PWM_DUTY / 4 * 2);
	pwm2 = (PWM_DUTY / 4 * 3);

	PWMn_Update(PCA0,pwm0);
	PWMn_Update(PCA1,pwm1);
	PWMn_Update(PCA2,pwm2);

	CR    = 1;	// 运行PCA定时器
}
//======================================================================

//========================================================================
// 函数: void	PCA_Handler (void) interrupt 7
// 描述: PCA中断处理程序.
// 参数: None
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	PCA_Handler (void) interrupt 7
{
	if(CCF0)		//PCA模块0中断
	{
		CCF0 = 0;		//清PCA模块0中断标志
		if(P25)	CCAP0_tmp += PWM0_high;	//输出为高电平，则给影射寄存器装载高电平时间长度
		else	CCAP0_tmp += PWM0_low;	//输出为低电平，则给影射寄存器装载低电平时间长度
		CCAP0L = (u8)CCAP0_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
		CCAP0H = (u8)(CCAP0_tmp >> 8);	//后写CCAP0H
	}

	if(CCF1)	//PCA模块1中断
	{
		CCF1 = 0;		//清PCA模块1中断标志
		if(P26)	CCAP1_tmp += PWM1_high;	//输出为高电平，则给影射寄存器装载高电平时间长度
		else	CCAP1_tmp += PWM1_low;	//输出为低电平，则给影射寄存器装载低电平时间长度
		CCAP1L = (u8)CCAP1_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
		CCAP1H = (u8)(CCAP1_tmp >> 8);	//后写CCAP0H
	}

	if(CCF2)	//PCA模块2中断
	{
		CCF2 = 0;		//清PCA模块1中断标志
		if(P27)	CCAP2_tmp += PWM2_high;	//输出为高电平，则给影射寄存器装载高电平时间长度
		else	CCAP2_tmp += PWM2_low;	//输出为低电平，则给影射寄存器装载低电平时间长度
		CCAP2L = (u8)CCAP2_tmp;			//将影射寄存器写入捕获寄存器，先写CCAP0L
		CCAP2H = (u8)(CCAP2_tmp >> 8);	//后写CCAP0H
	}
}



