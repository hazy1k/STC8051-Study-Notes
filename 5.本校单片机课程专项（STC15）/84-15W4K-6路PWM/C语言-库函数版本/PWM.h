
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


#ifndef	__PWM_H
#define	__PWM_H

#include	"config.h"


typedef struct
{ 
	u8	id;				//串口号

	u8	TX_read;		//发送读指针
	u8	TX_write;		//发送写指针
	u8	B_TX_busy;		//忙标志

	u8 	RX_Cnt;			//接收字节计数
	u8	RX_TimeOut;		//接收超时
	u8	B_RX_OK;		//接收块完成
} PWMx_Define; 

typedef struct
{
	u16	FirstEdge;				// 第一个翻转计数, 1~32767
	u16	SecondEdge;				// 第二个翻转计数, 1~32767
	u8	Start_IO_Level;			// 设置PWM输出端口的初始电平, 0或1
	u8	PWMx_IO_Select;			// PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	u8	PWMx_Interrupt;			// 中断允许,   ENABLE,DISABLE
	u8	FirstEdge_Interrupt;	// 第一个翻转中断允许, ENABLE,DISABLE
	u8	SecondEdge_Interrupt;	// 第二个翻转中断允许, ENABLE,DISABLE
} PWMx_InitDefine; 


void	PWM_SourceClk_Duty(u8 PWM_SourceClk, u16 init_duty);
void	PWMx_Configuration(u8 PWM_id, PWMx_InitDefine *PWMx);
void	PWMx_SetPwmWide(u8 PWM_id, u16 FirstEdge, u16 SecondEdge);


#endif
