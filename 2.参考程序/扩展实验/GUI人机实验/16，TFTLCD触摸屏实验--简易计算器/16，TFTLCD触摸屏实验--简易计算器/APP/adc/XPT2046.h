#ifndef	  __XPT2046_H_
#define   __XPT2046_H_

//---包含头文件---//
#include "public.h"


//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint  unsigned int
#endif

#ifndef ulong
#define ulong  unsigned long
#endif

//---定义使用的IO口---//
sbit DOUT = P3^4;	  //输出
sbit CLK  = P3^7;	  //时钟
sbit DIN  = P3^5;	  //输入
sbit CS   = P3^6;	  //片选

uint Read_AD_Data(uchar cmd);
uint SPI_Read(void);
void SPI_Write(uchar dat);

#endif





