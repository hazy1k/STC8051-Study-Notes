
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

6路PWM程序。

工程文件:
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
	
	PWMx_InitStructure.PWMx_IO_Select       = PWM2_P37;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM2_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P3n_standard(1<<7);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM3_P21;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM3_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P2n_standard(1<<1);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM4_P22;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM4_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P2n_standard(1<<2);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM5_P23;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM5_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P2n_standard(1<<3);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM6_P07;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM6_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P0n_standard(1<<7);										//IO初始化, 上电时为高阻

	PWMx_InitStructure.PWMx_IO_Select       = PWM7_P06;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = ENABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = ENABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM7_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P0n_standard(1<<6);										//IO初始化, 上电时为高阻

	TH2 = (u8)((65536UL - MAIN_Fosc / 500000UL) / 256);	//波特率使用Timer2
	TL2 = (u8)((65536UL - MAIN_Fosc / 500000UL) % 256);
	AUXR = 0x14;		//Timer2 set As Timer, Timer2 set as 1T mode;
	PWM_SourceClk_Duty(PwmClk_T2, 256);	//时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_T2,   PWM周期: 1~32767
	
	PWMCR |= ENPWM;		// 使能PWM波形发生器，PWM计数器开始计数
	PWMCR &= ~ECBI;		//禁止PWM计数器归零中断
//	PWMCR |=  ECBI;		//允许PWM计数器归零中断

//	PWMFDCR = ENFD | FLTFLIO | FDIO;	//PWM失效中断控制,  ENFD | FLTFLIO | EFDI | FDCMP | FDIO
}


/**********************************************/
void main(void)
{
	
	PWM_config();

//	EA = 1;

	PWMx_SetPwmWide(PWM2_ID, 16, 32);	//PWM_id: PWM通道, PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID.
	PWMx_SetPwmWide(PWM3_ID, 16, 48);
	PWMx_SetPwmWide(PWM4_ID, 16, 64);
	PWMx_SetPwmWide(PWM5_ID, 16, 96);
	PWMx_SetPwmWide(PWM6_ID, 16, 128);
	PWMx_SetPwmWide(PWM7_ID, 16, 144);

	while (1)
	{

	}
}



