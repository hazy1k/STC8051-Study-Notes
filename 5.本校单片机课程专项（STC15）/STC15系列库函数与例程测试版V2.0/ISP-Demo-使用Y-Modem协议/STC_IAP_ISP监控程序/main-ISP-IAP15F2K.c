

#define MAIN_Fosc		22118400L	//定义主时钟
#define Baudrate0		115200UL		//串口0波特率600~115200

#include	"STC15Fxxxx.H"
#include	"ymodem.h"


#define	WDT_Enable	0			//1: 允许内部看门狗,	0: 禁止

#define D_BRT0_1T_16bit		(65536UL - MAIN_Fosc / 4 / Baudrate0)		//计算波特率	1T  16bit



#define	ISP_ADDRESS		0xE400				//ISP开始地址, 高地址必须是偶数, 注意要预留ISP空间,本例程留3K
#define	UserflashLenth 	(ISP_ADDRESS-3)		//用户FLASH长度, 保留个字节存放用户地址程序的跳转地址



/*************	本地变量声明	**************/

typedef		void (*pFunction)(void);
pFunction	Jump_To_Application;
u16			JumpAddress;

u16		FileLength;
u16		Y_TimeOut,WaitTime;
u8		packets_received, session_begin;	//session -- 会话
u8		file_name[FILE_NAME_LENGTH];
u8		xdata file_size[FILE_SIZE_LENGTH];
u8		StartCode[3];
u16		FlashDestination;
u16		RxCnt;
u8		HandCnt;
u8	xdata	RxBuff[1032];

u32		DownCheckSum,FlashCheckSum;


/*************	本地函数声明	**************/
u8		Hex2Ascii(u8 dat);
u8		UART_Download(void);
u16		Str2Int(u8 *inputstr);
void	ISP_WriteByte(u16 addr, u8 dat);
void	ISP_EraseSector(u16 addr);
u8	ISP_ReadByte(u16 addr);




//========================================================================
// 函数: void	TX1_write2buff(u8 dat)
// 描述: 串口发送一个字节.
// 参数: dat: 要发送的字节.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	TX1_write2buff(u8 dat)
{
	TI = 0;
	SBUF = dat;
	while(!TI);
}

//========================================================================
// 函数: void	PrintString1(u8 *p)
// 描述: 串口发送一个字符串.
// 参数: *p: 要发送的字符串指针.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	PrintString1(u8 *p)
{
	for(; *p > 0; p++)	TX1_write2buff(*p);
}

//========================================================================
// 函数: void	Tx_DEC_U16(u16 j)
// 描述: 把一个16位整型数转成十进制送串口发送.
// 参数: j: 要处理的16位整型数.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	Tx_DEC_U16(u16 j)
{
	u8	i;
	u8	tmp[10];
	for(i=4; i<5; i--)	tmp[i] = j % 10 + '0',	j = j / 10;
	for(i=0; i<4; i++)
	{
		if(tmp[i] != '0')	break;
		tmp[i] = ' ';
	}
	for(i=0; i<5; i++)	TX1_write2buff(tmp[i]);
}

//========================================================================
// 函数: void	Tx_HEX_U32(u32 j)
// 描述: 把一个32位长整型数转成十进制送串口发送.
// 参数: j: 要处理的32位整型数.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	Tx_HEX_U32(u32 j)
{
	u8	i,k;
	u8	tmp[10];
	for(i=8; i>0; i--)
	{
		k = ((u8)j) & 0x0f;
		if(k <= 9)	tmp[i] = k+'0';
		else		tmp[i] = k-10+'A';
		j >>= 4;
	}
	for(i=1; i<9; i++)	TX1_write2buff(tmp[i]);
}


//========================================================================
// 函数: void	ReturnNameAndLength(void)
// 描述: 返回程序的文件名和长度, 和累加校验和.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	ReturnNameAndLength(void)
{
	u16	i;
	PrintString1("================================\r\n File name:     ");
	for(i=0; i<FILE_NAME_LENGTH; i++)	
	{
		if(file_name[i] == 0)	break;
		TX1_write2buff(file_name[i]);
	}
	PrintString1("\r\n File length:   ");
	Tx_DEC_U16(FileLength);
	PrintString1(" Bytes\r\n DownChexkSum:  ");
	Tx_HEX_U32(DownCheckSum);
	PrintString1("\r\n ISP Versiom:   2013-4-29 by Coody");
	PrintString1("\r\n================================\r\n\r\n");
}

//========================================================================
// 函数: void	UART1_RxPackage(void)
// 描述: 串口接收一个数据块.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	UART1_RxPackage(void)
{
	u16	j;	//5ms超时
//	RI = 0;
	RxCnt = 0;
	for(j = 0;	j < 5300; j++)	//最后收到一个字节5ms后,超时退出	5300
	{
		if(RI)
		{
			RI = 0;
			RxBuff[RxCnt] = SBUF;
			if(++RxCnt >= 1032)	RxCnt = 0;
			j = 0;	//重新定时5ms
		}
	}
	#if (WDT_Enable > 0)
		WDT_reset(D_WDT_SCALE_256);
	#endif
}

//========================================================================
// 函数: void	main(void)
// 描述: 主函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	main(void)
{
	u8	i;

	EA = 0;
	Timer0_Stop();
	Timer1_Stop();
	#if (WDT_Enable > 0)
		WDT_reset(D_WDT_SCALE_256);
	#endif

	S1_USE_P30P31();
	S1_8bit();
	S1_RX_Enable();
	S1_BRT_UseTimer1();
	S1_TXD_RXD_OPEN();
	Timer1_AsTimer();
	Timer1_1T();
	Timer1_16bitAutoReload();
	Timer1_InterruptDisable();
	TH1 = D_BRT0_1T_16bit / 256;
	TL1 = D_BRT0_1T_16bit % 256;
	Timer1_Run();

//	PrintString1("\r\n STC ISP Demo\r\n");
	
	while(1)
	{
	//	TX1_write2buff('A');
		HandCnt = 0;
		for(WaitTime = 0; WaitTime < 300; WaitTime++)		//1.5秒超时, 等待接收u	
		{
			UART1_RxPackage();
			if((RxCnt == 1) && (RxBuff[0] == 'd'))
			{
				if(++HandCnt >= 10)
				{
					i = UART_Download();
					WaitTime = 250;
					if(i == 1)			PrintString1("\r\n User abort!\r\n");
					else if(i == 2)		PrintString1("\r\n PC Cancel!\r\n");
					else if(i == 4)		PrintString1("\r\n Programming Error!\r\n");
					else if(i == 0)
					{
						PrintString1("\r\n\r\n Programming Completed Successfully!\r\n");
						ReturnNameAndLength();
					}
				}
			}
		//	else	HandCnt = 0;
		}

		if(ISP_ReadByte(ISP_ADDRESS-3) == 0x02)
		{
            SCON = 0;
			AUXR = 0;
            TMOD = 0;
            TL0 = 0;
            TH0 = 0;
            TH1 = 0;
            TL1 = 0;
            TCON = 0;
			IAP_CMD = 0;

			JumpAddress = ISP_ReadByte(ISP_ADDRESS-2);
			JumpAddress = (JumpAddress << 8) | ISP_ReadByte(ISP_ADDRESS-1);	//Jump to user application
			Jump_To_Application = (pFunction) JumpAddress;
			Jump_To_Application();
		}
		PrintString1(" No AP\r\n");
	}
}


//========================================================================
// 函数: u16 Str2Int(u8 *inputstr)
// 描述: 字符串转整型.
// 参数: *inputstr: 字符串指针.
// 返回: 16位整型数.
// 版本: V1.0, 2013-4-29
//========================================================================
u16 Str2Int(u8 *inputstr)
{
	u16	val;
	u8	i;
	
	val = 0;
	for (i = 0; i < 5; i++)
	{
		if((inputstr[i] < '0') || (inputstr[i] > '9'))	break;
		val = val * 10 + inputstr[i] - '0';
	}
	return val;
}

//========================================================================
// 函数: void	ISP_WriteByte(u16 addr, u8 dat)
// 描述: 对一个地址写FLASH一个字节.
// 参数: addr: 16位FLASH地址.
//       dat:  要写入的一字节数据.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	ISP_WriteByte(u16 addr, u8 dat)
{
	IAP_CONTR = ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY);	//使能IAP功能
	ISP_WRITE();			//编程命令
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_DATA  = dat;		//将当前数据送IAP数据寄存器
	ISP_TRIG();				//触发ISP命令
}

//========================================================================
// 函数: u8	ISP_ReadByte(u16 addr)
// 描述: 从一个地址读FLASH一个字节.
// 参数: addr: 16位FLASH地址.
// 返回: 读出的一字节数据.
// 版本: V1.0, 2013-4-29
//========================================================================
u8	ISP_ReadByte(u16 addr)
{
	IAP_CONTR = ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY);	//使能IAP功能
	ISP_READ();			//编程命令
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	ISP_TRIG();				//触发ISP命令
	return(IAP_DATA);
}

//========================================================================
// 函数: void	ISP_EraseSector(u16 addr)
// 描述: 对FLASH擦除一个扇区.
// 参数: addr: 16位FLASH地址.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
void	ISP_EraseSector(u16 addr)
{
	IAP_CONTR = ISP_CONTR = (ISP_EN + ISP_WAIT_FREQUENCY);		//使能IAP功能
	ISP_ERASE();				//擦除命令
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	ISP_TRIG();				//触发ISP命令
}

//========================================================================
// 函数: u8	UART_Download(void)
// 描述: 按Ymodem接收文件数据并写入用户FLASH.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2013-4-29
//========================================================================
u8	UART_Download(void)
{
	u16	i;
	u8	j;

	PrintString1("\r\n\r\n Waiting for the file to be sent ... (press 'a' to abort)\r\n");
	
	Y_TimeOut = 0;
	packets_received = 0;
	session_begin = 0;
	WaitTime = 40;
	DownCheckSum = 0;
	
	while(WaitTime > 0)
	{
		if(Y_TimeOut == 0)
		{
			TX1_write2buff(CRC16);
			Y_TimeOut = 300;
			if(WaitTime > 0) WaitTime--;
		}

		while(Y_TimeOut > 0)
		{
			UART1_RxPackage();
			if(RxCnt == 0)	Y_TimeOut--;
			else
			{
				if(RxCnt == 1)	//1字节
				{
					if(RxBuff[0] == EOT)	//收到EOT, 结束
					{
						TX1_write2buff(ACK);
						Y_TimeOut = 40;
					}
					else if((RxBuff[0] == ABORT1) || (RxBuff[0] == ABORT2))	//收到A或a, 取消
					{
						return 1;    //取消
					}
				}
				else if(RxCnt <= 5)	//小于5字节
				{
					if((RxBuff[0] == CANCEL) && (RxBuff[1] == CANCEL))	//收到两个CANCEL
					{
						return 2;	//PC端取消
					}
				}
				else if((RxCnt == 133) || (RxCnt == 1029))	//数据包
				{
					if (RxBuff[PACKET_SEQNO_INDEX] != (RxBuff[PACKET_SEQNO_COMP_INDEX] ^ 0xff))	//判断发送序号是否正确
					{
						TX1_write2buff(NAK);	//错误, 请求重发
						Y_TimeOut = 300;
					}
					else
					{
						WaitTime = 5;
						if (packets_received == 0)	//发送序号为0, 为文件名数据包
						{
							if (RxBuff[PACKET_HEADER] != 0)  //文件名不为空
							{
								for (i = 0; i < FILE_NAME_LENGTH; i++)	file_name[i] = 0;
								for (i = 0; i < FILE_SIZE_LENGTH; i++)	file_size[i] = 0;
								j = PACKET_HEADER;
								for (i = 0; (i < FILE_NAME_LENGTH) && (RxBuff[j] != 0); i++)
									file_name[i] = RxBuff[j++];		//保存文件名

								for (i=0, j++; (RxBuff[j] != ' ') && (i < FILE_SIZE_LENGTH); i++)
									file_size[i] = RxBuff[j++];	//保存文件长度
								FileLength = Str2Int(file_size);	//文件长度由字符串转成十六进制数据

								if (FileLength >= UserflashLenth)	//长度过长错误
								{
									TX1_write2buff(CANCEL);	//错误返回2个 CA
									TX1_write2buff(CANCEL);
									return 3;		//长度过长
								}
								
								StartCode[0] = ISP_ReadByte(0);	//保存ISP跳转地址
								StartCode[1] = ISP_ReadByte(1);
								StartCode[2] = ISP_ReadByte(2);
								ISP_EraseSector(0);
								ISP_WriteByte(0,StartCode[0]);	//回写ISP跳转地址
								ISP_WriteByte(1,StartCode[1]);
								ISP_WriteByte(2,StartCode[2]);
								for(i=0x200; i < UserflashLenth; i+=0x200)	//擦除N页
									ISP_EraseSector(i);
							
								TX1_write2buff(ACK);	//擦除完成, 返回应答
								Y_TimeOut = 40;
								packets_received ++;
								session_begin = 1;
								FlashDestination = 0;
								DownCheckSum = 0;
							}
						}

						/* Filename packet is empty, end session 全0数据包,会话结束*/
						else if(session_begin == 1)	//收过文件名
						{
							if(RxBuff[PACKET_SEQNO_INDEX] == 0)	//全0数据帧
							{
								ISP_WriteByte(ISP_ADDRESS-3,StartCode[0]);	//全部下载结束,最后写用户入口地址
								ISP_WriteByte(ISP_ADDRESS-2,StartCode[1]);
								ISP_WriteByte(ISP_ADDRESS-1,StartCode[2]);
								TX1_write2buff(ACK);
								
								FlashCheckSum  = ISP_ReadByte(ISP_ADDRESS-3);
								FlashCheckSum += ISP_ReadByte(ISP_ADDRESS-2);
								FlashCheckSum += ISP_ReadByte(ISP_ADDRESS-1);
								for(i = 3;	i < FileLength; i++)	FlashCheckSum += ISP_ReadByte(i);	//计算FLASH累加和
								if(FlashCheckSum == DownCheckSum)	return 0;	//正确
								else
								{
									ISP_EraseSector(ISP_ADDRESS-0x200);
									return 4;	//写入错误
								}
							}
							else	//数据帧
							{
								RxCnt -= 2;
								for(i = PACKET_HEADER; (i < RxCnt) && (FlashDestination < FileLength); i++)
								{
									if(FlashDestination == 0)
									{
										StartCode[0] = RxBuff[3];
										StartCode[1] = RxBuff[4];
										StartCode[2] = RxBuff[5];
										FlashDestination = 3;
										i += 3;
										DownCheckSum += RxBuff[3]; DownCheckSum += RxBuff[4]; DownCheckSum += RxBuff[5];
									}
									ISP_WriteByte(FlashDestination,RxBuff[i]);
									DownCheckSum += RxBuff[i];
									FlashDestination ++;
								}
								TX1_write2buff(ACK);	//保存完成, 返回应答
								Y_TimeOut = 300;
								packets_received ++;
							}
						}
					}
				}
			}
		}
	}
	return 100;	//其他错误
}

