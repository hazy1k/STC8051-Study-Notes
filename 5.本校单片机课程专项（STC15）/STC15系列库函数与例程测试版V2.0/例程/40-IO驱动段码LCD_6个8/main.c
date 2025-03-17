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

#include 	<intrins.h>
#include	"config.h"
#include	"timer.h"
#include	"LCD_IO16.h"


/*************	功能说明	**************

用STC115系列测试IO直接驱动段码LCD(6个8字LCD, 1/4 Dutys, 1/3 bias)。

上电后显示一个时间(时分秒).

P3.2对地接一个开关,用来进入睡眠或唤醒.


******************************************/

/*************	本地常量声明	**************/





/*************	本地变量声明	**************/
u8	cnt_500ms;
u8	second,minute,hour;
bit	B_Second;	//秒信号

/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/
extern	bit	B_2ms;



/************************ 定时器配置 ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536 - (MAIN_Fosc / 500);	//初值, 节拍为500HZ
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
}

/******************** 显示时间 **************************/
void	LoadRTC(void)
{
	LCD_load(1,hour/10);
	LCD_load(2,hour%10);
	LCD_load(3,minute/10);
	LCD_load(4,minute%10);
	LCD_load(5,second/10);
	LCD_load(6,second%10);
}

//========================================================================
// 函数: void  delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void  delay_ms(u8 ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}

/******************** 主函数**************************/
void main(void)
{

	Init_LCD_Buffer();
	Timer_config();
	EA = 1;

	LCD_SET_2M;	//显示时分间隔:
	LCD_SET_4M;	//显示分秒间隔:
	LoadRTC();	//显示时间
				
	while (1)
	{
		PCON |= 0x01;	//为了省电, 进入空闲模式(电流大约2.5~3.0mA @5V)，由Timer0 2ms唤醒退出
		_nop_();
		_nop_();
		_nop_();

		if(B_2ms)	//2ms节拍到
		{
			B_2ms = 0;

			if(++cnt_500ms >= 250)	//500ms到
			{
				cnt_500ms = 0;
			//	LCD_FLASH_2M;	//闪烁时分间隔:
			//	LCD_FLASH_4M;	//闪烁分秒间隔:
			
				B_Second = ~B_Second;
				if(B_Second)
				{
					if(++second >= 60)	//1分钟到
					{
						second = 0;
						if(++minute >= 60)	//1小时到
						{
							minute = 0;
							if(++hour >= 24)	hour = 0;	//24小时到
						}
					}
					LoadRTC();	//显示时间
				}
			}
		
			if(!P32)	//键按下，准备睡眠
			{
				LCD_CLR_2M;	//显示时分间隔:
				LCD_CLR_4M;	//显示分秒间隔:
				LCD_load(1,DIS_BLACK);
				LCD_load(2,DIS_BLACK);
				LCD_load(3,0);
				LCD_load(4,0x0F);
				LCD_load(5,0x0F);
				LCD_load(6,DIS_BLACK);
				
				while(!P32)	delay_ms(10);	//等待释放按键
				delay_ms(50);
				while(!P32)	delay_ms(10);	//再次等待释放按键
				
				TR0 = 0;	//关闭定时器
				IE0 = 0;	//外中断0标志位
				EX0 = 1;	//INT0 Enable
				IT0 = 1;	//INT0 下降沿中断		

				P1n_push_pull(0xff);	//com和seg全部输出0
				P2n_push_pull(0xff);	//com和seg全部输出0
				P1 = 0;
				P2 = 0;

				PCON |= 0x02;	//Sleep
				_nop_();
				_nop_();
				_nop_();

				LCD_SET_2M;	//显示时分间隔:
				LCD_SET_4M;	//显示分秒间隔:
				LoadRTC();	//显示时间
				TR0 = 1;	//打开定时器
				while(!P32)	delay_ms(10);	//等待释放按键
				delay_ms(50);
				while(!P32)	delay_ms(10);	//再次等待释放按键
			}
		}
	}
}


/********************* INT0中断函数 *************************/
void INT0_int (void) interrupt INT0_VECTOR
{
	EX0 = 0;			//INT0 Disable
	IE0  = 0;			//外中断0标志位
}




