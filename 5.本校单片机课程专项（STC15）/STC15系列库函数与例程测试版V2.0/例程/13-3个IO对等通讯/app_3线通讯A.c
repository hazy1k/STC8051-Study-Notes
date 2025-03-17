
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
#include	"USART1.h"
#include	"delay.h"

/*************	功能说明	**************

两个MCU使用3线通讯程序。不分主从,不使用中断。但是为了描述方便,发送方称为主机,接收方称为从机.

PC1 <==串口===> MCU1 <===3线对等传输====> MCU2 <===串口===> PC2.

对等传输，不分主从机，收到串口数据后，从3线传给对方，对方传来的数据从串口返回。

有两个LED分别指示接收或发送.

IO口可以随意定义. 如果通讯不成功,自动超时退出.

************************* 通讯原理说明 *********************************

3线双向零等待IO通讯机制


许多设备需要通过IO通讯交互数据,怎样才做到速度最快,通讯可靠,所用资源又少呢?
下面介绍一个我编写的通讯协议:
它没有1线,2线那样节省IO资源,但是它的通讯速度绝对最快,无需延时,且不用中断,还可以
对等传输.
实际情况下C51编写，实现了50us传输一个字节，折合160Kbps左右，晶振22M。

特点如下:
使用3根普通IO通讯
不使用中断
双方都可以主动发起数据通讯,也都可以被动接收数据,即可以对等传输.
有发送和接收的检查等待机制,发送方知道对方什么时候收了数据,接收方知道发送方什么时
候发了数据.
无需进行数据延时,最大可能地加快了通讯速度.
CPU闲的时候通讯速率可以最快.忙的时候又可以无限等待.保证绝对同步,不会出错.
双方CPU工作速度可以任意,即使是51和P4通讯,也能保证正确无误.

    ask w1    w2    w3    w4    w5    w6    w7    w8            
MCLK ┐┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌───            
     └┘    └──┘    └──┘    └──┘    └──┘ 
                          
SDT ___┌─┐┌─┐┌─┐┌─┐┌─┐┌─┐┌─┐┌─┐________
       └─┘└─┘└─┘└─┘└─┘└─┘└─┘└─┘  

ACK   ┐  ┌──┐    ┌──┐    ┌──┐    ┌──┐    ┌─                 
      └─┘    └──┘    └──┘    └──┘    └──┘
    res  r1    r2    r3    r4    r5    r6    r7    r8

1.主机发起通讯申请到从机
主机检测MCLK是否为0,为0的话是对方在申请通讯,退出函数.
主机检测到MCLK=1,表示通讯空闲.
主机将MCLK=0,表示申请通讯. 之后检测ACK是否为0(是否响应通讯)

2.从机响应通讯,从机在闲时检测MCLK是否为0,发现为0则主机向它发起了通讯申请.
从机将ACK=0,表示接受申请,之后检测MCLK是否变为1,如果是1则主机已经发出了第一个bit
的数据.  

3.主机发现ACK=0,知道从机已经开始准备接受数据.
4.主机发送第一个bit到SDA.
5.主机将MCLK=1,表示已经发送第一个bit到SDA.

6.从机发现MCLK=1,知道主机已经发出第一个数据
7.从机收第一个bit的数据.
8.从机将ACK=1,表示已经收了第一个bit.之后等待MCLK是否变0,如果变0,则主机已经发出
了第二个bit


9.主机发现ACK=1,知道从机已经开始准备接受第二个数据.
10.主机发送第二个bit到SDA.
11.主机将MCLK=0,表示已经发送第二个bit到SDA.

12.从机发现MCLK=0,知道主机已经发出第二个数据
13.从机收第二个bit的数据.
14.从机将ACK=0,表示已经收了第二个bit.之后等待MCLK是否变1,如果变1,则主机已经发出
了第三个bit

15.再重复3-14,3遍,传完8bit数据.

16.主机发现ACK=0,知道从机已经收完8位数据
17.主机将MCLK=1,准备下一轮通讯

18.从机发现MCLK=1,知道主机已经收起MCLK
19.从机将ACK=1,准备下一轮通讯


******************************************/


/*************	本地常量声明	**************/


/*************	本地变量声明	**************/
#define	SPI_LENTH	128
u8	xdata	SPI_buff[SPI_LENTH];

sbit	P_MCLK = P2^7;	//主机时钟，总是由发送方(主机)提供。
sbit	P_ACK  = P2^6;	//从机时钟，总是由接收方(从机)提供。
sbit	P_SDA  = P2^5;	//双向数据，主机发送，从机接收。

sbit	P_LED_RX = P2^0;	//接收指示灯
sbit	P_LED_TX = P2^1;	//发送指示灯


/*************	本地函数声明	**************/
u8	SpiTx(u8 *p,u8 lenth);
u8	SpiRx(void);



/*************  外部函数和变量声明 *****************/



/*************************************************************/
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

	PrintString1("3线对等通讯测试程序!\r\n");	//SUART1发送一个字符串
}


/**********************************************/
void main(void)
{
	u8	i,j;

	UART_config();
	EA = 1;

	while (1)
	{
		if(COM1.RX_TimeOut > 0)		//串口1超时计数
		{
			delay_ms(1);	//1ms时标
			if(--COM1.RX_TimeOut == 0)
			{
				if(COM1.RX_Cnt > 0)
				{
				//	for(i=0; i<COM1.RX_Cnt; i++)	TX1_write2buff(RX1_Buffer[i]);	//收到的数据原样返回
					P_LED_TX = 0;	//发送指示
					if(SpiTx(RX1_Buffer,COM1.RX_Cnt) > 0)		//串口1接收到数据, 转模拟SPI发送
						PrintString1("对方没有回应!\r\n");	//发送失败
					P_SDA  = 1;	P_MCLK = 1;	P_ACK = 1;
					P_LED_TX = 1;
				}
				COM1.RX_Cnt = 0;
			}
		}

		if(!P_MCLK) 	//检测MCLK是否为0,为0的话是对方在申请通讯.
		{
			P_LED_RX = 0;	//接收指示
			j = SpiRx();	//j为接收数据的长度
			if(j > 0)		{for(i=0; i<j; i++) 	TX1_write2buff(SPI_buff[i]);}	//模拟SPI收到数据, 转串口1发出
			P_SDA  = 1;	P_MCLK = 1;	P_ACK = 1;
			P_LED_RX = 1;
		}
	}
}


/****************** 3线通讯发送函数 ***************************/
//返回: 0--成功, 1: 对方请求通讯,  2: 对方没有回应, 3: 通讯过程对方没有回应
//调用函数之后, 要执行 P_SDA  = 1;	P_MCLK = 1;	P_ACK = 1;
u8	SpiTx(u8 *p,u8 lenth)
{
	u8 i,dat;
	u16	TimeOut;	//超时计数, 用于通讯结束或异常结束

	if(!P_MCLK)	return 1; 	//检测MCLK是否为0,为0的话是对方在申请通讯,退出函数.
	P_MCLK = 0;				//MCLK空闲, 申请通讯
	
	TimeOut = 1000;
	while(P_ACK)			//等待从机拉低应答线
	{
		if(--TimeOut == 0)	return 2;	//从机超时无反应，返回错误
	}

	do			//开始发送
	{
		dat = *p;			//取一个要发送的字节
		p++;				//指针指向下一个地址
		i = 8;				//8位一个字节
		do
		{	P_MCLK = 0;		//MCLK=0, 告诉从机数据已准备好
			dat <<= 1;		//移一个bit到CY
			P_SDA = CY;		//放一个bit到SDA
			TimeOut = 1000;
			while(P_ACK)	//等待从机响应, 从机一旦回应，马上退出等待.
			{
				if(--TimeOut == 0)	return 3;	//超时返回错误
			}
			P_MCLK  = 1;	//将MCLK=1,表示已经发送第一个bit到SDA
			TimeOut = 1000;
			while(!P_ACK)	//检测从机响应, ACK=1,表示从机已经收了一个bit.
			{
				if(--TimeOut == 0)	return 3;	//超时返回错误
			}
		}while(--i);		//发完8位
	}while(--lenth);		//发完所有字节
	return 0;
}

/****************** 3线通讯接收函数 ***************************/
//调用函数之后, 要执行 P_SDA  = 1;	P_MCLK = 1;	P_ACK = 1;
//返回0,空闲. 返回>0, 则收到数据. 协议要带校验.
u8	SpiRx(void)
{
	u8	i,dat;
	u16	TimeOut;	//超时计数, 用于通讯结束或异常结束
	u8	lenth;
	lenth = 0;
	while(1)
	{
		i = 8;
		do
		{	TimeOut = 300;
			while(P_MCLK)		//等待主机通讯请求
			{
				if(--TimeOut == 0)	return(lenth);	//超时返回
			}
			P_ACK = 0;			//接受通讯请求
			dat <<= 1;			//准备收下一个bit
			TimeOut = 300;
			while(!P_MCLK)		//检测主机发数据
			{
				if(--TimeOut == 0)	return(lenth);	//超时返回
			}
			if(P_SDA)	dat++;	//收一个bit
			P_ACK = 1;			//从机已接收一个bit
		}while(--i);

		SPI_buff[lenth] = dat;
		if(++lenth >= SPI_LENTH)	lenth = 0;
	}
}

