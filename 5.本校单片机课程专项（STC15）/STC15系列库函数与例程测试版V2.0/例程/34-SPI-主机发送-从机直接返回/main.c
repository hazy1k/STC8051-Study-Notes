
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
#include	"delay.h"
#include	"USART1.h"
#include	"spi.h"

/*************	功能说明	**************

把两个MCU的P1.2 P1.3 P1.4 P1.5分别同名端连接, 两个MCU分别连接两个PC串口A和B，就可以用串口助手测试了.

电脑串口A发给MCU-A, MCU-A把收到的数据通过SPI发给MCU-B, MCU-B从SPI接收到数据后,从串口发给电脑串口B.

PC-A --RS232--> MCU-A ---SPI---> MCU_B ---RS232---> PC_B

PC-A <--RS232-- MCU-A <---SPI--- MCU_B <---RS232--- PC_B

默认波特率:  115200,8,N,1. 可以在"串口初始化函数"里修改.
默认主时钟:  22118400HZ, 可以在配置文件"config.h"中修改.


工程文件:
inilize.c
interrupt.c
main.c

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/


/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



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

	PrintString1("STC15F2K60S2 SPI Test Prgramme!\r\n");	//SUART1发送一个字符串
}

void	SPI_config(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	SPI_InitStructure.SPI_Module    = ENABLE;              //SPI启动    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;			  //片选位     ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;			  //移位方向   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave;	  //主从选择   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;      //时钟相位   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_1Edge;	  //数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Interrupt = ENABLE;			  //中断允许   ENABLE,DISABLE
	SPI_InitStructure.SPI_Speed     = SPI_Speed_64;		  //SPI速度    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64, SPI_Speed_128
	SPI_InitStructure.SPI_IoUse     = SPI_P12_P13_P14_P15; //IO口切换   SPI_P12_P13_P14_P15, SPI_P24_P23_P22_P21, SPI_P54_P40_P41_P43
	SPI_Init(&SPI_InitStructure);
	
	SPI_TxRxMode = SPI_Mode_Slave;
}


/******************** task A **************************/
void main(void)
{
	u8	i;

	UART_config();
	SPI_config();
	EA = 1;

	while (1)
	{
		delay_ms(1);
				
		if(COM1.RX_TimeOut > 0)
		{
			if(--COM1.RX_TimeOut == 0)
			{
				if(COM1.RX_Cnt > 0)
				{
					SPI_TxRead = 0;
					SPI_TxWrite = 0;
					SPI_RxCnt = 0;
					COM1.RX_Cnt++;	//多发一个无效的字节
					for(i=0; i<COM1.RX_Cnt; i++)	SPI_WriteToTxBuf(RX1_Buffer[i]);	//串口收到的数据转发SPI
					SPI_TrigTx();
				}
				COM1.RX_Cnt = 0;	COM1.B_RX_OK = 0;
			}
		}
		if(SPI_RxTimerOut > 0)
		{
			if(--SPI_RxTimerOut == 0)
			{
				if(SPI_RxCnt > 1)
				{
					i = 0;
					if(B_RxSlave0_Master1)	i++;
					else					SPI_RxCnt--;
					for(; i<SPI_RxCnt; i++)	TX1_write2buff(SPI_RxBuffer[i]);
				}
				SPI_RxCnt = 0;	//B_SPI_RxOk = 0;
			}
		}
	}
}



