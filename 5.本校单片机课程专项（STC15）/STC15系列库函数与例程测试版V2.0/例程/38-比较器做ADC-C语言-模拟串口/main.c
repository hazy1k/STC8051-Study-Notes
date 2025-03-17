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

#include	"config.h"
#include	"compare.h"
#include	"timer.h"
#include	"GPIO.h"

/*************	功能说明	**************

通用程序。


工程文件:
inilize.c
interrupt.c
main.c

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/

u8	cnt0,cnt2;

sbit	P_TXD = P3^1;	//定义模拟串口发送端,可以是任意IO

void	TxSend(u8 dat);
void 	PrintString(unsigned char code *puts);

/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/
extern	bit	B_Timer0;
extern	u16		adc_value;		//ADC值, 用户层使用
extern	bit		adc_ok;			//ADC结束标志, 为1则adc_value的值可用. 此标志给用户层查询,并且清0



/************************ 比较器配置 ****************************/
void	CMP_config(void)
{
	CMP_InitDefine CMP_InitStructure;					//结构定义
	CMP_InitStructure.CMP_EN = ENABLE;					//允许比较器		ENABLE,DISABLE
	CMP_InitStructure.CMP_RiseInterruptEn = DISABLE;	//允许上升沿中断	ENABLE,DISABLE
	CMP_InitStructure.CMP_FallInterruptEn = DISABLE;	//允许下降沿中断	ENABLE,DISABLE
	CMP_InitStructure.CMP_P_Select     = CMP_P_P55;		//比较器输入正极性选择, CMP_P_P55: 选择内部P5.5做正输入, CMP_P_ADCIS: 由ADCIS[2:0]所选择的ADC输入端做正输入.
	CMP_InitStructure.CMP_N_Select     = CMP_N_P54;		//比较器输入负极性选择, CMP_N_BGv: 选择内部BandGap电压BGv做负输入, CMP_N_P54: 选择外部P5.4做输入.
	CMP_InitStructure.CMP_OutptP12_En  = DISABLE;		//允许比较结果输出到P1.2,   ENABLE,DISABLE
	CMP_InitStructure.CMP_InvCMPO      = DISABLE;		//比较器输出取反, 	ENABLE,DISABLE
	CMP_InitStructure.CMP_100nsFilter  = ENABLE;		//内部0.1uF滤波,  	ENABLE,DISABLE
	CMP_InitStructure.CMP_OutDelayDuty = 20;			//比较结果变化延时周期数, 0~63
//	CMP_InitStructure.CMP_Polity	   = PolityHigh;	//中断优先级,     PolityLow,PolityHigh
	CMP_Inilize(&CMP_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2
}


/************************ IO口配置 ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5;	//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_HighZ;				//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);			//初始化

	GPIO_InitStructure.Pin  = GPIO_Pin_2;				//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);			//初始化
}

/************************ 定时器配置 ****************************/

void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536 - MAIN_Fosc / 100000UL;	//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
}


/******************** task A **************************/
void main(void)
{
	u8	i;
	u8	tmp[5];

P20 = 1;
	GPIO_config();
	Timer_config();
	CMP_config();

	EA = 1;

	PrintString("\r\n使用比较器做ADC例子\r\n");

	while (1)
	{
		if(adc_ok)
		{
			adc_ok = 0;				//清除ADC已结束标志
			adc_value += 5;
			adc_value /= 10;
			PrintString("ADC = ");	//转十进制
			tmp[0] = adc_value / 10000 + '0';
			tmp[1] = adc_value % 10000 / 1000 + '0';
			tmp[2] = adc_value % 1000 / 100 + '0';
			tmp[3] = adc_value % 100 / 10 + '0';
			tmp[4] = adc_value % 10 + '0';
			for(i=0; i<4; i++)		//消无效0
			{
				if(tmp[i] != '0')	break;
				tmp[i] = ' ';
			}
			for(i=0; i<5; i++)	TxSend(tmp[i]);	//发串口
			PrintString("\r\n");
		}
//TxSend(0);
	/*	if(B_Timer0)
		{
			B_Timer0 = 0;
			
			
		}*/
	}
}


//========================================================================
// 函数: void	BitTime(void)
// 描述: 位时间函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void	BitTime(void)
{
	u16 i;
	i = ((MAIN_Fosc / 100) * 80) / 130000L - 1;		//根据主时钟来计算位时间	104
	while(--i);
}

//========================================================================
// 函数: void	TxSend(uchar dat)
// 描述: 模拟串口发送一个字节。9600，N，8，1
// 参数: dat: 要发送的数据字节.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void	TxSend(u8 dat)
{
	u8	i;
//	EA = 0;
	P_TXD = 0;
	BitTime();
	for(i=0; i<8; i++)
	{
		if(dat & 1)		P_TXD = 1;
		else			P_TXD = 0;
		dat >>= 1;
		BitTime();
	}
	P_TXD = 1;
//	EA = 1;
	BitTime();
	BitTime();
}

//========================================================================
// 函数: void PrintString(unsigned char code *puts)
// 描述: 模拟串口发送一串字符串。9600，N，8，1
// 参数: *puts: 要发送的字符指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void PrintString(unsigned char code *puts)
{
    for (; *puts != 0;	puts++)  TxSend(*puts);
}



