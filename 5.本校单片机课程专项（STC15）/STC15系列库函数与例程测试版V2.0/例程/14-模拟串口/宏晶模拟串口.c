

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



/*************	本程序功能说明	**************

				测试说明

	本例程是使用STC系列MCU做的模拟串口。用户根据自己的时钟和波特率自行设置后编译下载。
	
	使用串口助手向MCU发送数据，MCU收到后原样返回给PC。
	
	本例程使用资源: Timer0中断.

*/

#include "config.h"


/***************************************************************************/

typedef bit BOOL;
typedef unsigned char 	uchar;
typedef unsigned int 	uint;

#define Timer0_Reload		(65536 - MAIN_Fosc / BaudRate / 3)
#define D_RxBitLenth	9		//9: 8 + 1 stop
#define D_TxBitLenth	9		//9: 1 stop bit

sbit RXB = P3^0;                //define UART TX/RX port
sbit TXB = P3^1;

uchar  data TBUF,RBUF;
uchar  data TDAT,RDAT;
uchar  data TCNT,RCNT;	//发送和接收检测 计数器(3倍速率检测)
uchar  data TBIT,RBIT;	//发送和接收的数据计数器
uchar  data t, r;
uchar  data buf[16];

bit  TING,RING;	//正在发送或接收一个字节
bit  REND;	 	//接收完的标志位

#define	RxBitLenth	9	//8个数据位+1个停止位
#define	TxBitLenth	9	//8个数据位+1个停止位

//-----------------------------------------
//UART模块的初始变量	initial UART module variable
void UART_INIT()
{
      TING = 0;
      RING = 0;
      REND = 0;
      TCNT = 0;
      RCNT = 0;
}

void main()
{
	InternalRAM_enable();
//	ExternalRAM_enable();

	Timer0_1T();
	Timer0_AsTimer();
	Timer0_16bitAutoReload();
	Timer0_Load(Timer0_Reload);
	Timer0_InterruptEnable();
	Timer0_Run();
	EA = 1;						//打开总中断					open global interrupt switch

	UART_INIT();				//UART模块的初始变量

	while (1)
	{
		if (REND)				//如果接收完,把接收到的值存入接收buff
		{
			REND = 0;
			buf[r++ & 0x0f] = RBUF;
		}

		if(!TING)		//发送空闲
		{
			if (t != r)	//有要发送的数据
			{
				TBUF = buf[t++ & 0x0f];
				TING = 1;
			}
		}
	}
}


//-----------------------------------------
//定时器0中断程序for UART 以波特率3倍的速度采样判断 开始位		Timer interrupt routine for UART
void tm0(void) interrupt 1
{

	if (RING)
	{
		if (--RCNT == 0)				  //接收数据以定时器的1/3来接收
		{
			RCNT = 3;                   //重置接收计数器  接收数据以定时器的1/3来接收	reset send baudrate counter
			if (--RBIT == 0)			  //接收完一帧数据
			{
				RBUF = RDAT;            //存储数据到缓冲区	save the data to RBUF
				RING = 0;               //停止接收			stop receive
				REND = 1;               //接收完成标志设置	set receive completed flag
			}
			else
			{
				RDAT >>= 1;			  //把接收的单b数据 暂存到 RDAT(接收缓冲)
				if (RXB) RDAT |= 0x80;  //shift RX data to RX buffer
			}
		}
	}

	else if (!RXB)		//判断是不是开始位 RXB=0;
	{
		RING = 1;       //如果是则设置开始接收标志位 	set start receive flag
		RCNT = 4;       //初始化接收波特率计数器       	initial receive baudrate counter
		RBIT = RxBitLenth;       //初始化接收的数据位数(8个数据位+1个停止位)    initial receive bit number (8 data bits + 1 stop bit)
	}

	if (TING)			//发送开始标志位   judge whether sending
	{
		if (--TCNT == 0)			//发送数据以定时器的1/3来发送
		{
			TCNT = 3;				//重置发送计数器   reset send baudrate counter
			if (TBIT == 0)			//发送计数器为0 表明单字节发送还没开始
			{
				TXB = 0;			//发送开始位     					send start bit
				TDAT = TBUF;		//把缓冲的数据放到发送的buff		load data from TBUF to TDAT
				TBIT = TxBitLenth;	//发送数据位数 (8数据位+1停止位)	initial send bit number (8 data bits + 1 stop bit)
			}
			else					//发送计数器为非0 正在发送数据
			{
				if (--TBIT == 0)	//发送计数器减为0 表明单字节发送结束
				{
					TXB = 1;		//送停止位数据
					TING = 0;		//发送停止位    			stop send
				}
				else
				{
					TDAT >>= 1;		//把最低位送到 CY(益处标志位) shift data to CY
					TXB = CY;		//发送单b数据				write CY to TX port
				}
			}
		}
	}
}

