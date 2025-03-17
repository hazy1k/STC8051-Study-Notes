
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


#include	"spi.h"

u8	SPI_TxRxMode;	//
u8 	SPI_TxWrite;
u8 	SPI_TxRead;
u8 	SPI_RxCnt;
u8 	SPI_RxTimerOut;
u8 	SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];
u8 	SPI_BUF_type SPI_TxBuffer[SPI_BUF_LENTH];
bit 	B_SPI_RxOk;


//========================================================================
// 函数: void	SPI_Init(SPI_InitTypeDef *SPIx)
// 描述: SPI初始化程序.
// 参数: SPIx: 结构参数,请参考spi.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	SPI_Init(SPI_InitTypeDef *SPIx)
{
	if(SPIx->SPI_SSIG == ENABLE)			SPCTL &= ~(1<<7);	//enable SS, conform Master or Slave by SS pin.
	else									SPCTL |=  (1<<7);	//disable SS, conform Master or Slave by SPI_Mode
	if(SPIx->SPI_Module == ENABLE)			SPCTL |=  (1<<6);	//SPI enable
	else									SPCTL &= ~(1<<6);	//SPI disable
	if(SPIx->SPI_FirstBit == SPI_LSB)		SPCTL |= ~(1<<5);	//LSB first
	else									SPCTL &= ~(1<<5);	//MSB first
	if(SPIx->SPI_Mode == SPI_Mode_Slave)	SPCTL &= ~(1<<4);	//slave
	else									SPCTL |=  (1<<4);	//master
	if(SPIx->SPI_CPOL == SPI_CPOL_High)		SPCTL |=  (1<<3);	//SCLK Idle High, Low Active.
	else									SPCTL &= ~(1<<3);	//SCLK Idle Low, High Active.
	if(SPIx->SPI_CPHA == SPI_CPHA_2Edge)	SPCTL |=  (1<<2);	//sample at the second edge
	else									SPCTL &= ~(1<<2);	//sample at the first  edge
	if(SPIx->SPI_Interrupt == ENABLE)		IE2 |=  (1<<1);
	else									IE2 &= ~(1<<1);
	SPCTL = (SPCTL & ~3) | (SPIx->SPI_Speed & 3);					//set speed
	AUXR1 = (AUXR1 & ~(3<<2)) | SPIx->SPI_IoUse;	
}

//========================================================================
// 函数: void	SPI_SetMode(u8 mode)
// 描述: SPI设置主从模式函数.
// 参数: mode: 指定模式, 取值 SPI_Mode_Master 或 SPI_Mode_Slave.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	SPI_SetMode(u8 mode)
{
	SPI_TxRxMode = mode;
	if(mode == SPI_Mode_Slave)	SPCTL &= ~(1<<4);	//slave
	else						SPCTL |=  (1<<4);	//master
}


//========================================================================
// 函数: void SPI_WriteToTxBuf(u8 dat)
// 描述: SPI装载发送缓冲函数.
// 参数: dat: 要发送的值.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	SPI_WriteToTxBuf(u8 dat)	//写入发送缓冲，指针+1
{
	SPI_TxBuffer[SPI_TxWrite] = dat;
	if(++SPI_TxWrite >= SPI_BUF_LENTH)	SPI_TxWrite = 0;
}

//========================================================================
// 函数: void	SPI_TrigTx(void)
// 描述: SPI触发发送函数, 将SPI模式设置为发送模式并将发送缓冲的数据发出.
// 参数: dat: 要发送的值.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	SPI_TrigTx(void)
{
	u8	i;
	if(SPI_TxRead == SPI_TxWrite)	return;
	SPI_SetMode(SPI_Mode_Master);
	SPI_SS = 0;
	i = SPI_TxBuffer[SPI_TxRead];
	if(++SPI_TxRead >= SPI_BUF_LENTH)	SPI_TxRead = 0;
	SPDAT = i;
}


//========================================================================
// 函数: void SPI_Transivion (void) interrupt SPI_VECTOR
// 描述: SPI中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void SPI_Transivion (void) interrupt SPI_VECTOR
{
	if(SPI_TxRxMode == SPI_Mode_Slave)
	{
	//	if(!B_SPI_RxOk)
		{
			if(SPI_RxCnt >= SPI_BUF_LENTH)		SPI_RxCnt = 0;
			SPI_RxBuffer[SPI_RxCnt++] = SPDAT;
			SPI_RxTimerOut = 5;
		}
	}

	if(SPI_TxRxMode == SPI_Mode_Master)
	{
		if(SPI_TxRead != SPI_TxWrite)
		{
			SPDAT = SPI_TxBuffer[SPI_TxRead];
			if(++SPI_TxRead >= SPI_BUF_LENTH)		SPI_TxRead = 0;
		}
		else
		{
			SPI_TxRxMode = SPI_Mode_Slave;
			SPCTL &= ~(1<<4);	//slave
			SPI_SS = 1;
		}
	}
	SPSTAT = SPIF + WCOL;	//清0 SPIF和WCOL标志
}

