
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
#include	"timer.h"
#include	"GPIO.h"

/*************	功能说明	**************

本程序演示3个定时器的使用。

定时器0配置为16位自动重装, 20000HZ中断率(50us定时)
         允许高速时钟输出时从P3.5输出10KHZ时钟信号。
         允许中断时从P1.0取反输出10KHZ方波信号。
定时器1配置为16位自动重装, 2000HZ中断率(500us定时)
         允许高速时钟输出时从P3.4输出1000HZ时钟信号。
         允许中断时从P1.1取反输出1000HZ方波信号。
定时器2固定为16位自动重装, 1000HZ中断率(1ms定时)
         允许高速时钟输出时从P3.0输出500HZ时钟信号。
         允许中断时从P1.2取反输出500HZ方波信号。
定时器3固定为16位自动重装, 200HZ中断率(5ms定时)
         允许高速时钟输出时从P0.4输出100HZ时钟信号。
         允许中断时从P1.3取反输出100HZ方波信号。

定时器4固定为16位自动重装, 100HZ中断率(10ms定时)
         允许高速时钟输出时从P0.6输出50HZ时钟信号。
         允许中断时从P1.4取反输出50HZ方波信号。

用户可以修改输出频率，但注意时间不要太高（一般100KHZ以下）。


******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



/************************ IO口配置 ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5;	//指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//初始化
}

/************************ 定时器配置 ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 20000);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 2000);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//初始化Timer1	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE. (注意: Timer2固定为16位自动重装, 中断固定为低优先级)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (1000*12));		//初值
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE. (注意: Timer2固定为16位自动重装, 中断固定为低优先级)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (200*12));		//初值
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer3,&TIM_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2,Timer3,Timer4

	TIM_InitStructure.TIM_Interrupt = DISABLE;				//中断是否允许,   ENABLE或DISABLE. (注意: Timer2固定为16位自动重装, 中断固定为低优先级)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (100*12));		//初值
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer4,&TIM_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2,Timer3,Timer4
}


/******************** 主函数**************************/
void main(void)
{

	P0n_standard(0xff);		//4K系列
	P1n_standard(0xff);
	P2n_standard(0xff);
	P3n_standard(0xff);
	P4n_standard(0xff);
	P5n_standard(0xff);
	P6n_standard(0xff);
	P7n_standard(0xff);

	GPIO_config();
	Timer_config();
	EA = 1;

	while (1)
	{

	}
}



