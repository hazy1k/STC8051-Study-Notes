
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


#include	"config.h"
#include	"PWM.h"


/*************	功能说明	**************

演示使用2路PWM产生互补或同相的SPWM.

主时钟选择24MHZ, PWM时钟选择1T, PWM周期2400, 死区12个时钟(0.5us).正弦波表用200点. 

输出正弦波频率 = 24000000 / 2400 / 200 = 50 HZ.

本程序仅仅是一个SPWM的演示程序, 用户可以通过上面的计算方法修改PWM周期和正弦波的点数和幅度.

本程序输出频率固定, 如果需要变频, 请用户自己设计变频方案.

本程序从P2.1(PWM3)输出正相脉冲, 从P2.2(PWM4)输出反相脉冲(互补).

如果需要P2.2输出同相的, 请在初始化配置中"Start_IO_Level"项选择0(设置PWM输出端口的初始电平, 0或1).


工程文件:
delay.c
pwm.c
main.c

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



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
	PWMx_InitDefine		PWMx_InitStructure;					//结构定义
	
	PWMx_InitStructure.PWMx_IO_Select       = PWM3_P21;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 65;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 1220;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM3_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P21 = 0;
	P2n_push_pull(1<<1);									//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM4_P22;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 1;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 65-PWM_DeadZone;	//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 1220+PWM_DeadZone;//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM4_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P22 = 1;
	P2n_push_pull(1<<2);									//IO初始化, 上电时为高阻

	PWM_SourceClk_Duty(PwmClk_1T, 2400);	//时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_Timer2,   PWM周期: 1~32767
	
	PWMCR |= ENPWM;		// 使能PWM波形发生器，PWM计数器开始计数
//	PWMCR &= ~ECBI;		//禁止PWM计数器归零中断
	PWMCR |=  ECBI;		//允许PWM计数器归零中断

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



