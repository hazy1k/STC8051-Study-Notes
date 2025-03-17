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

#include "compare.h"


//========================================================================
// 函数:void	CMP_Inilize(CMP_InitDefine *CMPx)
// 描述: 定时器初始化程序.
// 参数: CMPx: 结构参数,请参考compare.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
void	CMP_Inilize(CMP_InitDefine *CMPx)
{
	CMPCR1 = 0;
	CMPCR2 = CMPx->CMP_OutDelayDuty & 0x3f;						//比较结果变化延时周期数, 0~63
	if(CMPx->CMP_EN == ENABLE)				CMPCR1 |= CMPEN;	//允许比较器		ENABLE,DISABLE
	if(CMPx->CMP_RiseInterruptEn == ENABLE)	CMPCR1 |= PIE;		//允许上升沿中断	ENABLE,DISABLE
	if(CMPx->CMP_FallInterruptEn == ENABLE)	CMPCR1 |= NIE;		//允许下降沿中断	ENABLE,DISABLE
	if(CMPx->CMP_P_Select  == CMP_P_ADCIS)	CMPCR1 |= PIS;		//比较器输入正极性选择, CMP_P_P55: 选择内部P5.5做正输入, CMP_P_ADCIS: 由ADCIS[2:0]所选择的ADC输入端做正输入.
	if(CMPx->CMP_N_Select  == CMP_N_P54)	CMPCR1 |= NIS;		//比较器输入负极性选择, CMP_N_BGv: 选择内部BandGap电压BGv做负输入, CMP_N_P54: 选择外部P5.4做输入.
	if(CMPx->CMP_OutptP12_En == ENABLE)		CMPCR1 |= CMPOE;	//允许比较结果输出到P1.2,   ENABLE,DISABLE
	if(CMPx->CMP_InvCMPO     == ENABLE)		CMPCR2 |= INVCMPO;	//比较器输出取反, 	ENABLE,DISABLE
	if(CMPx->CMP_100nsFilter == DISABLE)	CMPCR2 |= DISFLT;	//内部0.1uF滤波,  	ENABLE,DISABLE
//	u8	CMP_Polity;			//中断优先级,     PolityLow,PolityHigh
}



/********************* UART1中断函数************************/
void CMP_int (void) interrupt CMP_VECTOR
{
	P13 = ~P13;
	if((CMPCR1 & CMPRES) > 0)	P14 = ~P14;
	else						P15 = ~P15;
	CMPCR1 &= ~CMPIF;	//清除中断标志
}




