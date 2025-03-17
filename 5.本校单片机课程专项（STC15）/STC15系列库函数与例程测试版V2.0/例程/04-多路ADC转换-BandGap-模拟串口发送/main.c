
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

#define P1n_pure_input(bitn)		P1M1 |=  (bitn),	P1M0 &= ~(bitn)

/*************	功能说明	**************

本程序演示多路ADC查询采样，通过模拟串口发送给上位机，波特率9600,8,n,1。
用户可以修改为1~8路的ADC转换。

说明:
    ADC的第9通道是用来测试内部BandGap参考电压的,由于内部BandGap参考电
压很稳定,不会随芯片的工作电压的改变而变化,所以可以通过测量内部BandGap
参考电压,然后通过ADC的值便可反推出VCC的电压,从而用户可以实现自己的低
压检测功能.
    ADC的第9通道的测量方法:首先将P1ASF初始化为0,即关闭所有P1口的模拟功能
然后通过正常的ADC转换的方法读取第0通道的值,即可通过ADC的第9通道读取当前
内部BandGap参考电压值.
    用户实现自己的低压检测功能的实现方法:首先用户需要在VCC很精准的情况下
(比如5.0V),测量出内部BandGap参考电压的ADC转换值(比如为BGV5),并这个值保存
到EEPROM中,然后在低压检测的代码中,在实际VCC变化后,所测量出的内部BandGap
参考电压的ADC转换值(比如为BGVx),通过计算公式: 实际VCC = 5.0V * BGV5 / BGVx,
即可计算出实际的VCC电压值,需要注意的是,第一步的BGV5的基准测量一定要精确.

标定内部基准Vref, 提供一个稳定的工作电压Vcc, 读内部基准Nref, 计算内部基准 Vref = Nref * Vcc / 1024.

测量某个电压, 读ADC值Nx, 则电压 Ux = Vref * Nx / Nref. 一般Vref = 1220mV.


******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/
u16	Nref;


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

	P1n_pure_input(0x07);	//把ADC口设置为高阻输入
}



/**********************************************/
void main(void)
{
	u8	i;
	u16	j;
//	u8	k;

	ADC_config();

	while (1)
	{
		for(i=0; i<4; i++)
		{
			delay_ms(250);		//为了让发送的速度慢一点，延时250ms

			if(i <3)	//ADC0~ADC2
			{
				j = Get_ADC10bitResult(i);	//参数0~7,查询方式做一次ADC, 返回值就是结果, == 1024 为错误
				TxSend('A');
				TxSend('D');
				TxSend(i+'0');
			}
			else		//内基准
			{
				P1ASF = 0;
				j = Get_ADC10bitResult(0);	//读内部基准电压, P1ASF=0, 读0通道
				P1ASF = ADC_P10 | ADC_P11 | ADC_P12;
				Nref = j;
				TxSend('A');
				TxSend('D');
				TxSend('r');
				TxSend('e');
				TxSend('f');
			}
			TxSend('=');
			TxSend(j/1000 + '0');
			TxSend(j%1000/100 + '0');
			TxSend(j%100/10 + '0');
			TxSend(j%10 + '0');
			TxSend(' ');
			TxSend(' ');

			TxSend('V');
			TxSend('=');
			j = (u32)1250 * (u32)j / Nref;			//Ux = Vref * Nx / Nref. Vref = 1250mV
			TxSend(j/1000 + '0');
			TxSend('.');
			TxSend(j%1000/100 + '0');
			TxSend(j%100/10 + '0');
			TxSend(j%10 + '0');
			TxSend(' ');
			TxSend(' ');
		}
		PrintString("\r\n");
		//=====================================================================
	}
}



