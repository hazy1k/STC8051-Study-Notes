
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

#define MAIN_Fosc		22118400L	//定义主时钟
#define	BaudRate1		9600ul		//定义波特率
#define	ADC_SCALE		50000		//ADC满量程, 根据需要设置

#include	"STC15Fxxxx.H"


/*************	本地常量声明	**************/

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

#define	TIM_16BitAutoReload			0
#define	TIM_16Bit					1
#define	TIM_8BitAutoReload			2
#define	TIM_16BitAutoReloadNoMask	3

#define	Pin0		0x01	//IO引脚 Px.0
#define	Pin1		0x02	//IO引脚 Px.1
#define	Pin2		0x04	//IO引脚 Px.2
#define	Pin3		0x08	//IO引脚 Px.3
#define	Pin4		0x10	//IO引脚 Px.4
#define	Pin5		0x20	//IO引脚 Px.5
#define	Pin6		0x40	//IO引脚 Px.6
#define	Pin7		0x80	//IO引脚 Px.7
#define	PinAll		0xFF	//IO所有引脚

/*************	本地变量声明	**************/
//sbit	P_ADC	= P1^2;	//P1.2 比较器转IO输出端
sbit	P_ADC	= P1^4;	//P1.2 比较器转IO输出端
u16		adc;			//ADC中间值, 用户层不可见
u16		adc_duty;		//ADC计数周期, 用户层不可见
u16		adc_value;		//ADC值, 用户层使用
bit		adc_ok;			//ADC结束标志, 为1则adc_value的值可用. 此标志给用户层查询,并且清0


/*************	本地函数声明	**************/
void	TxString(u8 *puts);






void main(void)
{
	u8	i;
	u8	tmp[5];


//IO口初始化
//	P1n_push_pull(Pin2);		//P1.2设置为push-pull output
	P1n_push_pull(Pin4);		//P1.2设置为push-pull output
	P5n_pure_input(Pin4+Pin5);	//P5.4 P5.5设置为高阻输入


//比较器初始化
	CMPCR1 = 0;
	CMPCR2 = 20;		//比较结果变化延时周期数, 0~63
	CMPCR1 |= CMPEN;	//允许比较器		ENABLE,DISABLE
//	CMPCR1 |= PIE;		//允许上升沿中断	ENABLE,DISABLE
//	CMPCR1 |= NIE;		//允许下降沿中断	ENABLE,DISABLE
//	CMPCR1 |= PIS;		//输入正极性选择, 0: 选择内部P5.5做正输入,           1: 由ADCIS[2:0]所选择的ADC输入端做正输入.
	CMPCR1 |= NIS;		//输入负极性选择, 0: 选择内部BandGap电压BGv做负输入, 1: 选择外部P5.4做输入
//	CMPCR1 |= CMPOE;	//允许比较结果输出到P1.2,   ENABLE,DISABLE
//	CMPCR2 |= INVCMPO;	//比较器输出取反, 	ENABLE,DISABLE
	CMPCR2 |= DISFLT;	//内部0.1uF滤波,  	ENABLE,DISABLE


//定时器0 初始化
	TMOD &= ~0x0f;
	Timer0_16bitAutoReload();	//设置为16位自动重装模式
	Timer0_1T();	//设置为1T模式
	ET0 = 1;		//允许中断
	PT0 = 1;		//高优先级中断
	TH0 = (u8)((65536 - MAIN_Fosc / 100000ul)>>8);		//重装值	100KHZ, 10us,	65536 - (MAIN_Fosc)/100000
	TL0 = (u8)( 65536 - MAIN_Fosc / 100000ul);
	TR0 = 1;		//开始运行


//串口1初始化
	S1_USE_P30P31();	//UART1 使用P30 P31口	默认
//	S1_USE_P36P37();	//UART1 使用P36 P37口
//	S1_USE_P16P17();	//UART1 使用P16 P17口

	S1_8bit();			//8位数据,波特率可变
	S1_RX_Enable();		//允许接收
	S1_TXD_RXD_OPEN();	//将TXD与RXD连接中继断开	默认
	S1_BRT_UseTimer2();	//使用Timer2做波特率发生器
	ES = 0;				//禁止中断, 使用查询发送

	Timer2_1T();	//Timer2 1T模式, 固定为16位自动重装
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //设置波特率重装值
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
	Timer2_Run();	//允许定时器2计数

	EA = 1;			//允许全局中断

	TxString("\r\n使用比较器做ADC例子\r\n");	//SUART1发送一个字符串
	

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


/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	if((CMPCR1 & CMPRES) == 0)	P_ADC = 0;	//比较器输出高电平
	else				//P_ADC输出低电平, 给负输入端做反馈. 
	{
		P_ADC = 1;			//P_ADC输出高电平, 给负输入端做反馈. 
		adc ++;	//ADC计数+1
	}

	if(--adc_duty == 0)			//ADC周期-1, 到0则ADC结束
	{
		adc_duty = ADC_SCALE;	//周期计数赋初值
		adc_value = adc;		//保存ADC值
		adc = 0;				//清除ADC值
		adc_ok = 1;				//标志ADC已结束
	}
}





