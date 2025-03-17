
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

/*************	功能说明	**************

本文件为STC15xxx系列的定时器初始化和中断程序,用户可以在这个文件中修改自己需要的中断程序.


******************************************/



#include	"timer.h"

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
   P10 = ~P10;
}

/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
   P11 = ~P11;

}

/********************* Timer2中断函数************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{
	P12 = ~P12;
}

/********************* Timer2中断函数************************/
void timer3_int (void) interrupt TIMER3_VECTOR
{
	P13 = ~P13;
}

/********************* Timer2中断函数************************/
void timer4_int (void) interrupt TIMER4_VECTOR
{
	P14 = ~P14;
}


//========================================================================
// 函数: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer4)	return 1;	//空操作

	if(TIM == Timer0)
	{
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TR0 = 0;		//停止计数
		ET0 = 0;	//禁止中断
		PT0 = 0;	//低优先级中断
		TMOD &= 0xf0;	//定时模式, 16位自动重装
		AUXR &= ~0x80;	//12T模式, 
		INT_CLKO &= ~0x01;	//不输出时钟
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//允许中断
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//高优先级中断
		TMOD |= TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x80;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x04;	//对外计数或分频
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//输出时钟
		
		TH0 = (u8)(TIMx->TIM_Value >> 8);
		TL0 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer1)
	{
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TR1 = 0;	//停止计数
		ET1 = 0;	//禁止中断
		PT1 = 0;	//低优先级中断
		TMOD &=  0x0f;	//定时模式, 16位自动重装
		AUXR &= ~0x40;	//12T模式, 
		INT_CLKO &= ~0x02;	//不输出时钟
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//允许中断
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//高优先级中断
		TMOD |= (TIMx->TIM_Mode << 4);	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x40;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x40;	//对外计数或分频
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//输出时钟
		
		TH1 = (u8)(TIMx->TIM_Value >> 8);
		TL1 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer2)		//Timer2,固定为16位自动重装, 中断无优先级
	{
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		AUXR &= ~0x1c;		//停止计数, 定时模式, 12T模式
		IE2  &= ~(1<<2);	//禁止中断
		INT_CLKO &= ~0x04;	//不输出时钟
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<2);	//允许中断
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  (1<<2);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	AUXR |=  (1<<3);	//对外计数或分频
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;	//输出时钟

		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer3)		//Timer3,固定为16位自动重装, 中断无优先级
	{
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		T4T3M &= 0xf0;		//停止计数, 定时模式, 12T模式, 不输出时钟
		IE2  &= ~(1<<5);	//禁止中断
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<5);	//允许中断
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		T4T3M |=  (1<<1);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	T4T3M |=  (3<<1);	//对外计数或分频
		if(TIMx->TIM_ClkOut == ENABLE)	T4T3M |=  1;	//输出时钟

		TH3 = (u8)(TIMx->TIM_Value >> 8);
		TL3 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	T4T3M |=  (1<<3);	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer4)		//Timer4,固定为16位自动重装, 中断无优先级
	{
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		T4T3M &= 0x0f;		//停止计数, 定时模式, 12T模式, 不输出时钟
		IE2  &= ~(1<<6);	//禁止中断
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<6);	//允许中断
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		T4T3M |=  (1<<5);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	T4T3M |=  (3<<5);	//对外计数或分频
		if(TIMx->TIM_ClkOut == ENABLE)	T4T3M |=  (1<<4);	//输出时钟

		TH4 = (u8)(TIMx->TIM_Value >> 8);
		TL4 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	T4T3M |=  (1<<7);	//开始运行
		return	0;		//成功
	}

	return 2;	//错误
}
