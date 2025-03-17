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



/****************************
本示例在Keil开发环境下请选择Intel的8052芯片型号进行编译

本例程MCU的工作频率为22.1184MHz.

使用MCU自带的比较器进行ADC转换, 并通过串口输出结果. 用定时器0产生10us中断查询比较器的状态.

使用比较器做ADC, 原理图如下.
做ADC的原理是基于电荷平衡的计数式ADC.
电压从Vin输入, 通过100K+104滤波, 进入比较器的P5.5正输入端, 经过比较器的比较, 将结果输出到P1.5再通过100K+104滤波后送比较器P5.4负输入端,跟输入电压平衡.
设置两个变量: 计数周期(量程)adc_duty 和 比较结果高电平的计数值 adc, adc严格比例于输入电压.
ADC的基准就是P1.5的高电平. 如果高电平准确,比较器的放大倍数足够大,则ADC结果会很准确.
当比较结果为高电平,则P1.5输出1, 并且adc+1.
当比较结果为低电平,则P1.5输出0.
每一次比较都判断计数周期是否完成,完成则adc里的值就是ADC结果.
电荷平衡计数式ADC的性能类似数字万用表用的双积分ADC, 当计数周期为20ms的倍数时,具有很强的抗工频干扰能力,很好的线性和精度.
原理可以参考ADD3501(3 1/2位数字万用表)或ADD3701(3 3/4位数字万用表), 也可以参考AD7740 VFC电路.

例: 比较一次的时间间隔为10us, 量程为10000, 则做1次ADC的时间为100ms. 比较器的响应时间越短, 则完成ADC就越快.

由于要求每次比较时间间隔都要相等,所以用C编程最好在定时器中断里进行, 定时器设置为自动重装, 高优先级中断, 其它中断均低优先级.
用汇编的话, 保证比较输出电平处理的时间要相等.


                                         100K
                            /| P5.5       ___
                    P1.2   /+|---------o-|___|- ------- Vin
                     .----<  | P5.4    |
                     |     \-|---.     |
                     |      \|   |     |
                     |           |     |
                     |    ___    |     |
                     '---|___|---o     |
                        100K     |     |
                                ---   ---
                                ---   ---
                            104  |     |  104
                                 |     |
                                ===   ===
                                GND   GND



******************************/


#include	"config.h"
#include	"compare.h"
#include	"timer.h"
#include	"GPIO.h"
#include	"USART1.h"



/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/
void	TxString(u8 *puts);



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

/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 9600ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2
}

/******************** task A **************************/
void main(void)
{
	u8	i;
	u8	tmp[5];

	GPIO_config();
	Timer_config();
	CMP_config();
	UART_config();

	EA = 1;

	PrintString1("\r\n使用比较器做ADC例子\r\n");	//SUART1发送一个字符串
	while(COM1.B_TX_busy > 0)	;	//等待发送完毕
	
	ES = 0;	//禁止中断, 下面使用查询发送

	while (1)
	{
		if(adc_ok)		//等待ADC结束
		{
			adc_ok = 0;				//清除ADC已结束标志
			TxString("ADC = ");	//转十进制
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
			for(i=0; i<5; i++)		//发串口
			{
				TI = 0;
				SBUF = tmp[i];
				while(!TI);
				TI = 0;
			}
			TxString("\r\n");
		}
	}
}


void TxString(u8 *puts)		//发送一个字符串
{
    for (; *puts != 0;	puts++)   	//遇到停止符0结束
	{
		TI = 0;
		SBUF = *puts;
		while(!TI);
		TI = 0;
	}
}






