
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


#define 	MAIN_Fosc		24000000L	//定义主时钟

#include	"STC15Fxxxx.H"

#include	"T_SineTable.h"

#define		PWM_DeadZone	12	/* 死区时钟数, 6 ~ 24之间  */


/*************	功能说明	**************

演示使用2路PWM产生互补或同相的SPWM.

主时钟选择24MHZ, PWM时钟选择1T, PWM周期2400, 死区12个时钟(0.5us).正弦波表用200点. 

输出正弦波频率 = 24000000 / 2400 / 200 = 50 HZ.

本程序仅仅是一个SPWM的演示程序, 用户可以通过上面的计算方法修改PWM周期和正弦波的点数和幅度.

本程序输出频率固定, 如果需要变频, 请用户自己设计变频方案.

本程序从P2.1(PWM3)输出正相脉冲, 从P2.2(PWM4)输出反相脉冲(互补).

如果需要P2.2输出同相的, 请在初始化配置中"PWMCFG"项选择设置1(设置PWM输出端口的初始电平, 0或1).


******************************************/

u8	PWM_Index;	//SPWM查表索引


//========================================================================
// 函数: void	PWM_config(void)
// 描述: PWM配置函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-8-15
// 备注: 
//========================================================================
void	PWM_config(void)
{
	u8	xdata	*px;

	EAXSFR();			// 访问XFR

	px = PWM3T1H;		// 指针指向PWM3
	*px = 0;			// 第一个翻转计数高字节
	px++;
	*px = 65;			// 第一个翻转计数低字节
	px++;
	*px = 1220 / 256;	// 第二个翻转计数高字节
	px++;
	*px = 1220 % 256;	// 第二个翻转计数低字节
	px++;
	*px = 0;				// PWM3输出选择P2.1, 无中断
	PWMCR  |=  0x02;	// 相应PWM通道的端口为PWM输出口，受PWM波形发生器控制
	PWMCFG &= ~0x02;	// 设置PWM输出端口的初始电平为0
//	PWMCFG |=  0x02;	// 设置PWM输出端口的初始电平为1
	P21 = 0;
	P2n_push_pull(1<<1);	//IO初始化, 上电时为高阻

	px = PWM4T1H;		// 指针指向PWM4
	*px = 0;			// 第一个翻转计数高字节
	px++;
	*px = 65-PWM_DeadZone;			// 第一个翻转计数低字节
	px++;
	*px = (1220+PWM_DeadZone) / 256;	// 第二个翻转计数高字节
	px++;
	*px = (1220+PWM_DeadZone) % 256;	// 第二个翻转计数低字节
	px++;
	*px = 0;			// PWM4输出选择P2.2, 无中断
	PWMCR  |=  0x04;	// 相应PWM通道的端口为PWM输出口，受PWM波形发生器控制
//	PWMCFG &= ~0x04;	// 设置PWM输出端口的初始电平为0
	PWMCFG |=  0x04;	// 设置PWM输出端口的初始电平为1
	P22 = 1;
	P2n_push_pull(1<<2);	//IO初始化, 上电时为高阻

	px  = PWMCH;		// PWM计数器的高字节
	*px = 2400 / 256;
	px++;
	*px = 2400 % 256;	// PWM计数器的低字节
	px++;				// PWMCKS, PWM时钟选择
	*px = PwmClk_1T;	// 时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_Timer2

	EAXRAM();			// 恢复访问XRAM
	
	PWMCR |= ENPWM;		// 使能PWM波形发生器，PWM计数器开始计数
//	PWMCR &= ~ECBI;		// 禁止PWM计数器归零中断
	PWMCR |=  ECBI;		// 允许PWM计数器归零中断

//	PWMFDCR = ENFD | FLTFLIO | FDIO;	//PWM失效中断控制,  ENFD | FLTFLIO | EFDI | FDCMP | FDIO
}


/**********************************************/
void main(void)
{

	PWM_config();	//初始化PWM

	EA = 1;			//允许全局中断

	while (1)
	{

	}
}


/********************* PWM中断函数************************/

void PWM_int (void) interrupt PWM_VECTOR
{
	u8	xdata	*px;
	u16	j;
	u8	SW2_tmp;

	if(PWMIF & CBIF)	//PWM计数器归零中断标志
	{
		PWMIF &= ~CBIF;	//清除中断标志

		SW2_tmp = P_SW2;	//保存SW2设置
		EAXSFR();		//访问XFR
		px = PWM3T2H;	// 指向PWM3
		j = T_SinTable[PWM_Index];
		*px = (u8)(j >> 8);	//第二个翻转计数高字节
		px++;
		*px = (u8)j;			//第二个翻转计数低字节

		j += PWM_DeadZone;		//死区
		px = PWM4T2H;			// 指向PWM4
		*px = (u8)(j >> 8);	//第二个翻转计数高字节
		px++;
		*px = (u8)j;			//第二个翻转计数低字节
		P_SW2 = SW2_tmp;	//恢复SW2设置

		if(++PWM_Index >= 200)	PWM_Index = 0;

	}
/*
	if(PWMIF & C2IF)	//PWM2中断标志
	{
		PWMIF &= ~C2IF;	//清除中断标志
	}

	if(PWMIF & C3IF)	//PWM3中断标志
	{
		PWMIF &= ~C3IF;	//清除中断标志
	}

	if(PWMIF & C4IF)	//PWM4中断标志
	{
		PWMIF &= ~C4IF;	//清除中断标志
	}

	if(PWMIF & C5IF)	//PWM5中断标志
	{
		PWMIF &= ~C5IF;	//清除中断标志
	}

	if(PWMIF & C6IF)	//PWM6中断标志
	{
		PWMIF &= ~C6IF;	//清除中断标志
	}

	if(PWMIF & C7IF)	//PWM7中断标志
	{
		PWMIF &= ~C7IF;	//清除中断标志
	}
*/
}



