
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


#ifndef	__SPI_H
#define	__SPI_H

#include	"config.h"

#define	SPI_BUF_LENTH	64
#define	SPI_BUF_type	xdata


#define	SPI_Mode_Master		1
#define	SPI_Mode_Slave		0
#define	SPI_CPOL_High		1
#define	SPI_CPOL_Low		0
#define	SPI_CPHA_1Edge		1
#define	SPI_CPHA_2Edge		0
#define	SPI_Speed_4			0
#define	SPI_Speed_16		1
#define	SPI_Speed_64		2
#define	SPI_Speed_128		3
#define	SPI_MSB				0
#define	SPI_LSB				1
#define	SPI_P12_P13_P14_P15	(0<<2)
#define	SPI_P24_P23_P22_P21	(1<<2)
#define	SPI_P54_P40_P41_P43	(2<<2)

typedef struct
{
	u8	SPI_Module;		//ENABLE,DISABLE
	u8	SPI_SSIG;		//ENABLE, DISABLE
	u8	SPI_FirstBit;	//SPI_MSB, SPI_LSB
	u8	SPI_Mode;		//SPI_Mode_Master, SPI_Mode_Slave
	u8	SPI_CPOL;		//SPI_CPOL_High,   SPI_CPOL_Low
	u8	SPI_CPHA;		//SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	u8	SPI_Interrupt;		//ENABLE,DISABLE
	u8	SPI_Speed;		//SPI_Speed_4,      SPI_Speed_16,SPI_Speed_64,SPI_Speed_128
	u8	SPI_IoUse;		//SPI_P12_P13_P14_P15, SPI_P24_P23_P22_P21, SPI_P54_P40_P41_P43
} SPI_InitTypeDef;



extern	u8	SPI_TxRxMode;
extern	u8 	SPI_TxWrite;
extern	u8 	SPI_TxRead;
extern	u8 	SPI_RxCnt;
extern	u8 	SPI_RxTimerOut;
extern	u8 	SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];
extern	u8 	SPI_BUF_type SPI_TxBuffer[SPI_BUF_LENTH];
extern	bit 	B_SPI_RxOk;
extern	bit	B_RxSlave0_Master1;


void	SPI_Init(SPI_InitTypeDef *SPIx);
void	SPI_SetMode(u8 mode);
void	SPI_WriteToTxBuf(u8 dat);
void	SPI_TrigTx(void);

#endif

