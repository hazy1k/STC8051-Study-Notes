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


/*************	功能说明	**************

演示INT2唤醒源对MCU唤醒。
从串口输出结果(ASCII)，9600,8,N,1.

******************************************/


/*************	用户系统配置	**************/

#define MAIN_Fosc		22118400L	//定义主时钟, 模拟串口和和延时会自动适应。5~35MHZ



/*************	以下宏定义用户请勿修改	**************/
#include	<reg52.H>
#include 	<intrins.h>

#define	uchar	unsigned char
#define uint	unsigned int

/******************************************/

sfr INT_CLKO = 0x8F;

//sbit INT2 = P3^4;	//Only for STC15F/L10xW
sbit INT2 = P3^6;


sbit	P_TXD1 = P3^1;
void	Tx1Send(unsigned char dat);
void	PrintString(unsigned char code *puts);
void  	delay_ms(unsigned char ms);

uchar	WakeUpCnt;


void main(void)
{
	P3 = 0xff;
	delay_ms(2);	//delay 2ms


	PrintString("*** STC15F系列INT2唤醒源测试程序 ***\r\n");
	
	while(1)
	{
		while(!INT2);	//等待外中断为高电平
		delay_ms(10);	//delay 10ms

		while(!INT2);	//等待外中断为高电平
		delay_ms(10);	//delay 10ms

		INT_CLKO |= (1 << 4);	//INT2 Enable	
		EA  = 1;		//Enable all interrupt

		PCON |= 0x02;	//Sleep
		_nop_();
		_nop_();
		_nop_();
		delay_ms(1);	//delay 1ms
		
		PrintString("外中断INT2唤醒  ");
		WakeUpCnt++;
		Tx1Send(WakeUpCnt/100+'0');
		Tx1Send(WakeUpCnt%100/10+'0');
		Tx1Send(WakeUpCnt%10+'0');
		PrintString("次唤醒\r\n");
	}

}


/********************* INT2中断函数 *************************/
void INT2_int (void) interrupt 10
{
	INT_CLKO &= ~(1 << 4);	//INT2 Disable	
}



//========================================================================
// 函数: void  delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数.
// 返回: none.
// 版本: VER1.0
// 日期: 2010-12-15
// 备注: 
//========================================================================
void  delay_ms(unsigned char ms)
{
     unsigned int i;
	 do
	 {
		i = MAIN_Fosc / 13000L;	//1T
		while(--i)	;   //13T per loop
     }while(--ms);
}

/********************** 模拟串口相关函数************************/

void	BitTime(void)	//位时间函数 9600bps
{
	unsigned int i;
	i = ((MAIN_Fosc / 100) * 104) / 130000L - 1;	//根据主时钟来计算位时间 1T
	while(--i)	;   //13T per loop
}

//模拟串口发送
void	Tx1Send(unsigned char dat)		//9600，N，8，1		发送一个字节
{
	unsigned char	i;
	P_TXD1 = 0;
	BitTime();
	i = 8;
	do
	{
		dat >>= 1;
		P_TXD1 = CY;
		BitTime();
	}while(--i);
	P_TXD1 = 1;
	BitTime();	//stop bit
	BitTime();	//stop bit
}

void PrintString(unsigned char code *puts)		//发送一串字符串
{
    for (; *puts != 0;	puts++)  Tx1Send(*puts); 	//遇到停止符0结束
}

