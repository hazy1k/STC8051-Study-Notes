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

#ifndef	__COMPARE_H
#define	__COMPARE_H

#include	"config.h"

//CMPCR1
#define	CMPEN	0x80	//1: 允许比较器, 0: 禁止,关闭比较器电源
#define	CMPIF	0x40	//比较器中断标志, 包括上升沿或下降沿中断, 软件清0
#define	PIE		0x20	//1: 比较结果由0变1, 产生上升沿中断
#define	NIE		0x10	//1: 比较结果由1变0, 产生下降沿中断
#define	PIS		0x08	//输入正极性选择, 0: 选择内部P5.5做正输入,           1: 由ADCIS[2:0]所选择的ADC输入端做正输入.
#define	NIS		0x04	//输入负极性选择, 0: 选择内部BandGap电压BGv做负输入, 1: 选择外部P5.4做输入.
#define	CMPOE	0x02	//1: 允许比较结果输出到P1.2, 0: 禁止.
#define	CMPRES	0x01	//比较结果, 1: CMP+电平高于CMP-,  0: CMP+电平低于CMP-,  只读

//CMPCR2
#define	INVCMPO	0x80	//1: 比较器输出取反,  0: 不取反
#define	DISFLT	0x40	//1: 关闭0.1uF滤波,   0: 允许
#define	LCDTY	0x00	//0~63, 比较结果变化延时周期数

#define	CMP_P_P55		0
#define	CMP_P_ADCIS		1
#define	CMP_N_P54		1
#define	CMP_N_BGv		0

typedef struct
{ 
	u8	CMP_EN;				//比较器允许或禁止, 			ENABLE,DISABLE
	u8	CMP_Interrupt;		//比较器中断允许或禁止, 		ENABLE,DISABLE
	u8	CMP_RiseInterruptEn;	//比较器上升沿中断允许或禁止, 	ENABLE,DISABLE       
	u8	CMP_FallInterruptEn;	//比较器下降沿中断允许或禁止, 	ENABLE,DISABLE
	u8	CMP_P_Select;		//比较器输入正极性选择, CMP_P_P55: 选择内部P5.5做正输入, CMP_P_ADCIS: 由ADCIS[2:0]所选择的ADC输入端做正输入.
	u8	CMP_N_Select;		//比较器输入负极性选择, CMP_N_BGv: 选择内部BandGap电压BGv做负输入, CMP_N_P54: 选择外部P5.4做输入.
	u8	CMP_OutptP12_En;	//允许比较结果输出到P1.2,   ENABLE,DISABLE
	u8	CMP_Polity;			//中断优先级,     PolityLow,PolityHigh
	u8	CMP_InvCMPO;		//比较器输出取反, ENABLE,DISABLE
	u8	CMP_100nsFilter;	//内部0.1uF滤波,  ENABLE,DISABLE
	u8	CMP_OutDelayDuty;	//0~63, 比较结果变化延时周期数

} CMP_InitDefine; 

void	CMP_Inilize(CMP_InitDefine *CMPx);

#endif
