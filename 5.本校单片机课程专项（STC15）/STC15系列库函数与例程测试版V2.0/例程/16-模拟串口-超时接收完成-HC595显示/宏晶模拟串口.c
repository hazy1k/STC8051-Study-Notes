

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

	本例程是使用STC系列MCU做的模拟串口。
	
	用户根据自己的时钟和波特率自行在config.h里设置后编译下载。
	
	使用串口助手向MCU发送数据，MCU收到后原样返回给PC。
	
	串口接收到一个数据块, 并且串口空闲超过35个位时间(超时)后，标志接收完成.
	
	本例程使用资源: Timer0中断.

*/

#include "config.h"


/*************	本地常量声明	**************/
#define Timer0_Reload	(65536 - MAIN_Fosc / BaudRate / 3)
#define RxLength		32		//接收缓冲长度

u8 code t_display[]={						//标准字库
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

unsigned char code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//位码


/*************	本地变量声明	**************/

sbit P_RXB = P3^0;			//定义模拟串口接收IO
sbit P_TXB = P3^1;			//定义模拟串口发送IO

bit  TxBusyFlag;		//正在发送一个字节, 用户设置1开始发送, 发送完毕系统清自动0
bit	B_Rx_OK;	 		//接收完的标志位, 收到数据块系统设置1, 用户处理数据后必须清0
u8	Rx_Cnt;				//接收到的字节数, 用户处理数据后必须清0
u8  idata RxBuf[RxLength];	//接收缓冲
u8	TxSBUF;				//发送数据寄存器, 用户写入要发送的数据, 把TxBusyFlag设置为1,系统就会发送出去, 发送完成后系统把TxBusyFlag清0.

//===== 下面这些为系统使用的常量或变量, 用户不可见，请勿修改 =============

#define	RxBitLenth	9		//8个数据位+1个停止位
#define	TxBitLenth	9		//8个数据位+1个停止位
u8  TxShiftReg,RxShiftReg;	//发送 接收 移位
u8  TxSample,RxSample;		//发送和接收检测 采样计数器(3倍速率检测)
u8  TxBitCnt,RxBitCnt;		//发送和接收的数据 位计数器
u8	RxTimeOut;		//接收超时计数
bit	RxStartFlag;			//正在接收一个字节(已收到起始位)

//=====================================================================
sbit	P_HC595_SER   = P3^2;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^4;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^3;	//pin 11	SRCLK	Shift data clock

u8 	LED8[8];		//显示缓冲
u8	display_index;	//显示位索引
u8	cnt_2ms;

/*************	本地函数声明	**************/
void	TxByte(u8 dat);
void	TxString(u8 *p);


/*************  外部函数和变量声明 *****************/



//========================================================================
// 函数: void UART_INIT(void)
// 描述: UART模块的初始变量.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2012-10-22
//========================================================================
void UART_INIT(void)
{
	TxBusyFlag = 0;
	RxStartFlag = 0;
	TxSample = 3;
	RxSample = 4;
	RxTimeOut = 0;
	B_Rx_OK = 0;
	Rx_Cnt = 0;
}


//========================================================================
// 函数: void main(void)
// 描述: 主程序.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2012-10-22
//========================================================================
void main(void)
{
	u8	i;
	u16	j;

	InternalRAM_enable();
//	ExternalRAM_enable();

	Timer0_1T();
	Timer0_AsTimer();
	Timer0_16bitAutoReload();
	Timer0_Load(Timer0_Reload);
	Timer0_InterruptEnable();
	PT0 = 1;	//定时器0高优先级中断
	Timer0_Run();
	EA = 1;						//打开总中断					open global interrupt switch

	for(i=0; i<8; i++)	LED8[i] = 0x10;	//上电消隐
	LED8[7] = 0;

	UART_INIT();				//UART模块的初始变量
	TxString("STC15F/L系列MCU模拟串口演示程序\r\n");	//发送一串提示字符

	
	while (1)
	{
		if (B_Rx_OK)	//接收完的标志位, 收到数据块系统设置1, 用户处理数据后必须清0
		{
			if(Rx_Cnt >= 3)	//确认有数据
			{
				for(i=0; i<Rx_Cnt; i++)	TxByte(RxBuf[i]);	//把收到的数据原样返回,用于测试
			}
			if(Rx_Cnt == 2)	//确认有数据
			{
				j = ((u16)RxBuf[0] << 8) + RxBuf[1];
				LED8[3] = j / 10000;
				LED8[4] = j % 10000 / 1000;
				LED8[5] = j % 1000 / 100;
				LED8[6] = j % 100 / 10;
				LED8[7] = j % 10;
				for(i=3; i<7; i++)
				{
					if(LED8[i] > 0)	break;
					LED8[i] = 0x10;
				}
			//	for(i=0; i<Rx_Cnt; i++)	TxByte(RxBuf[i]);	//把收到的数据原样返回,用于测试
			}
			Rx_Cnt  = 0;	//清除字节数
			B_Rx_OK = 0;	//清除接收完成标志
		}
	}
}


//========================================================================
// 函数: void	TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: dat: 要发送的数据.
// 返回: 无.
// 版本: V1.0, 2012-10-22
//========================================================================
void	TxByte(u8 dat)
{
	TxSBUF = dat;		//要发送的数据写入发送缓冲
	TxBusyFlag = 1;		//启动发送
	while(TxBusyFlag);	//等待发送完成
}

//========================================================================
// 函数: void	TxString(u8 *p)
// 描述: 发送一个数据块, 遇到0结束.
// 参数: *p: 数据块的指针.
// 返回: 无.
// 版本: V1.0, 2012-10-22
//========================================================================
void	TxString(u8 *p)
{
	for( ; *p > 0; p++)	TxByte(*p);
}


/**********************************************/
void Send_595(unsigned char dat)	//发送一个字节
{		
	unsigned char	i;
	for(i=0; i<8; i++)
	{
		dat <<= 1;
		P_HC595_SER = CY;
		P_HC595_SRCLK = 1;
		P_HC595_SRCLK = 0;
	}
}

/**********************************************/
#define		LED_TYPE	0xff		//定义LED类型, 0x00--共阴, 0xff--共阳

void DisplayScan(void)	//显示扫描函数
{	
	Send_595((~LED_TYPE) ^ T_COM[display_index]);			//输出位码
	Send_595(  LED_TYPE  ^ t_display[LED8[display_index]]);	//输出段码

	P_HC595_RCLK = 1;
	P_HC595_RCLK = 0;							//锁存输出数据
	if(++display_index >= 8)	display_index = 0;	//8位结束回0
}

//========================================================================
// 函数: void tm0(void) interrupt 1
// 描述: 定时器0中断程序, for UART 以波特率3倍的速度采样判断 开始位.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2012-10-22
//========================================================================

void tm0(void) interrupt 1
{
	//====================== 模拟串口接收程序 ========================================
	if (RxStartFlag)			//已接收到起始位
	{
		if (--RxSample == 0)			//接收数据以定时器的1/3来接收
		{
			RxSample = 3;               //重置接收计数器  接收数据以定时器的1/3来接收	reset send baudrate counter
			if (--RxBitCnt == 0)		//接收完一帧数据
			{
				RxStartFlag = 0;        //停止接收			stop receive
				if(P_RXB && !B_Rx_OK)	//确认停止位正确,并且缓冲已空
				{
					RxBuf[Rx_Cnt] = RxShiftReg;     //存储数据到缓冲区	save the data to RBUF
					if(++Rx_Cnt >= RxLength)	Rx_Cnt = 0;	//溢出判断
					RxTimeOut = 105;				//超时计数初值, 35个位的时间(对应5个字节), 参考MODBUS协议
				}
			}
			else
			{
				RxShiftReg >>= 1;			  //把接收的单b数据 暂存到 RxShiftReg(接收缓冲)
				if (P_RXB) RxShiftReg |= 0x80;  //shift RX data to RX buffer
			}
		}
	}

	else if (!P_RXB)		//判断是不是开始位 P_RXB=0;
	{
		RxStartFlag = 1;       //如果是则设置开始接收标志位 	set start receive flag
		RxSample = 4;       //初始化接收波特率计数器       	initial receive baudrate counter
		RxBitCnt = RxBitLenth;       //初始化接收的数据位数(8个数据位+1个停止位)    initial receive bit number (8 data bits + 1 stop bit)
	}
	if(RxTimeOut > 0)	//接收超时处理
	{
		if(--RxTimeOut == 0)	B_Rx_OK = 1;	//标志已收到一帧数据
	}

	//====================== 模拟串口发送程序 ========================================
	if (TxBusyFlag)			//发送开始标志位   judge whether sending
	{
		if (--TxSample == 0)			//发送数据以定时器的1/3来发送
		{
			TxSample = 3;				//重置发送计数器   reset send baudrate counter
			if (TxBitCnt == 0)			//发送计数器为0 表明单字节发送还没开始
			{
				P_TXB = 0;			//发送开始位     					send start bit
				TxShiftReg = TxSBUF;		//把缓冲的数据放到发送的buff		load data from TxSBUF to TxShiftReg
				TxBitCnt = TxBitLenth;	//发送数据位数 (8数据位+1停止位)	initial send bit number (8 data bits + 1 stop bit)
			}
			else					//发送计数器为非0 正在发送数据
			{
				if (--TxBitCnt == 0)	//发送计数器减为0 表明单字节发送结束
				{
					P_TXB = 1;			//送停止位数据
					TxBusyFlag = 0;	//发送停止位    			stop send
				}
				else
				{
					TxShiftReg >>= 1;		//把最低位送到 CY(益处标志位) shift data to CY
					P_TXB = CY;		//发送单b数据				write CY to TX port
				}
			}
		}
	}

	if(--cnt_2ms == 0)
	{
		cnt_2ms = ((u32)BaudRate * 6 + 500)/1000;
		DisplayScan();
	}
}

