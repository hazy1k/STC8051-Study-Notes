
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
#include	"adc.h"
#include	"delay.h"
#include	"soft_uart.h"


/*************	功能说明	**************

本程序演示多路ADC查询采样，通过模拟串口发送给上位机，波特率9600,8,n,1。
用户可以修改为1~8路的ADC转换。

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/


void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//结构定义
	ADC_InitStructure.ADC_Px        = ADC_P10 | ADC_P11 | ADC_P12;	//设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_360T;			//ADC速度			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC功率允许/关闭	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC结果调整,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//优先级设置	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//中断允许		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//初始化
	ADC_PowerControl(ENABLE);							//单独的ADC电源操作函数, ENABLE或DISABLE
}



/**********************************************/
void main(void)
{
	u8	i;
	u16	j;

	ADC_config();

	while (1)
	{

		for(i=0; i<3; i++)
		{
			delay_ms(250);

		//	Get_ADC10bitResult(i);		//参数0~7,查询方式做一次ADC, 丢弃一次
			j = Get_ADC10bitResult(i);	//参数0~7,查询方式做一次ADC, 返回值就是结果, == 1024 为错误
			TxSend('A');
			TxSend('D');
			TxSend(i+'0');
			TxSend('=');
			TxSend(j/1000 + '0');
			TxSend(j%1000/100 + '0');
			TxSend(j%100/10 + '0');
			TxSend(j%10 + '0');
			TxSend(' ');
			TxSend(' ');
		}
		PrintString("\r\n");
	}
}



