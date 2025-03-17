
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
#include	"USART.h"
#include	"PCA.h"

/*************	功能说明	**************

PCA0  为8位PWM. 输出变化的PWM信号。类似"呼吸灯"的驱动.
PCA1  为捕捉. 可以连接到P2.7用来测试捕捉,捕捉的时间从串口1输出. 也可以从外部输入一个信号来捕捉.
PCA2  为16位软件定时, 定时时间为30000个PCA时钟, 并且从P2.7高速这个信号,输出周期为60000个PCA时钟.
      P2.7可以连接到P2.6用来测试捕捉,捕捉的时间从串口1输出.

串口1和串口2把收到的数据原样返回.

定时器0产生1ms的基准,作为程序运行的节拍.

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/

u8	cnt0;
u16	Cap_time;	//上一次捕捉时间
u8	pwm0;		//pwm
bit	B_PWM0_Dir;	//方向, 0为+, 1为-.

/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



extern	bit	B_Timer0_1ms;

void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - ((1000 * Main_Fosc_KHZ) / 12000);	//初值, 1000us
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
}

void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BaudRate  = 57600ul;			//波特率,     110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART2_SW_P10_P11;	//切换端口,   UART2_SW_P10_P11,UART2_SW_P46_P47
	USART_Configuration(USART2, &COMx_InitStructure);		//初始化串口2 USART1,USART2

	PrintString1("STC15F2K60S2 PCA Test Prgramme!\r\n");	//SUART1发送一个字符串
	PrintString2("STC15F2K60S2 PCA Test Prgramme!\r\n");	//SUART2发送一个字符串
}


void	PCA_config(void)
{
	PCA_InitTypeDef		PCA_InitStructure;

	PCA_InitStructure.PCA_Clock    = PCA_Clock_12T;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_IoUse    = PCA_P24_P25_P26_P27;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//ENABLE, DISABLE
	PCA_InitStructure.PCA_Polity   = PolityHigh;		//优先级设置	PolityHigh,PolityLow
	PCA_InitStructure.PCA_RUN      = DISABLE;			//ENABLE, DISABLE
	PCA_Init(PCA_Counter,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_PWM;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = PCA_PWM_8bit;		//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//PCA_Rise_Active, PCA_Fall_Active, ENABLE, DISABLE
	PCA_InitStructure.PCA_Value    = 128 << 8;			//对于PWM,高8位为PWM占空比
	PCA_Init(PCA0,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_Capture;	//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = 0;					//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Interrupt_Mode = PCA_Fall_Active | ENABLE;	//(PCA_Rise_Active, PCA_Fall_Active) or (ENABLE, DISABLE)
	PCA_InitStructure.PCA_Value    = 0;					//对于捕捉, 这个值没意义
	PCA_Init(PCA1,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_HighPulseOutput;	//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = 0;					//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Interrupt_Mode = ENABLE;		//PCA_Rise_Active, PCA_Fall_Active, ENABLE, DISABLE
	PCA_InitStructure.PCA_Value    = 30000;				//对于软件定时, 为匹配比较值
	PCA_Init(PCA2,&PCA_InitStructure);

	CR = 1;
}


/******************** task A **************************/
void main(void)
{
	u8	i;
	u16	j;
	
	Timer_config();
	UART_config();
	PCA_config();
	EA = 1;
	
	Cap_time = 0;
	pwm0 = 128;
	B_PWM0_Dir = 0;

	while (1)
	{
		if(B_Timer0_1ms)
		{
			B_Timer0_1ms = 0;

			if(B_Capture1)
			{
				B_Capture1 = 0;
				j = CCAP1_tmp - Cap_time;	//计算时间差
				Cap_time = CCAP1_tmp;
				TX1_write2buff(j/10000 + '0');
				TX1_write2buff(j%10000/1000 + '0');
				TX1_write2buff(j%1000/100 + '0');
				TX1_write2buff(j%100/10 + '0');
				TX1_write2buff(j%10 + '0');
				PrintString1("\r\n");
			}

			cnt0++;
			if((cnt0 & 15) == 15)	//16ms
			{
				if(B_PWM0_Dir)
				{
						if(--pwm0 <= 8)		B_PWM0_Dir = 0;
				}
				else	if(++pwm0 >= 248)	B_PWM0_Dir = 1;
				UpdatePwm(PCA0,pwm0);
			}

			if(COM1.RX_TimeOut > 0)		//超时计数
			{
				if(--COM1.RX_TimeOut == 0)
				{
					if(COM1.RX_Cnt > 0)
					{
						for(i=0; i<COM1.RX_Cnt; i++)	TX1_write2buff(RX1_Buffer[i]);	//收到的数据原样返回
					}
					COM1.RX_Cnt = 0;
				}
			}
			if(COM2.RX_TimeOut > 0)		//超时计数
			{
				if(--COM2.RX_TimeOut == 0)
				{
					if(COM2.RX_Cnt > 0)
					{
						for(i=0; i<COM2.RX_Cnt; i++)	TX2_write2buff(RX2_Buffer[i]);	//收到的数据原样返回
					}
					COM2.RX_Cnt = 0;
				}
			}
		}
	}
}



