#ifndef _smg_H
#define _smg_H

#include "public.h"


#define SMG_A_DP_PORT	P0	//使用宏定义数码管段码口

//定义数码管位选信号控制脚
sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;

extern unsigned char gsmg_code[17];

void smg_display(unsigned char dat[], unsigned char pos);

#endif