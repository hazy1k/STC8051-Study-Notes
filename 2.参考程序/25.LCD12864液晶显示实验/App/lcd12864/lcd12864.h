#ifndef _lcd12864_H
#define _lcd12864_H

#include "public.h"

// 管脚定义
sbit LCD12864_RS = P2^6; // 数据命令选择
sbit LCD12864_WR = P2^5; // 读写选择
sbit LCD12864_E = P2^7; // 使能信号
#define LCD12864_DATAPORT	P0	//LCD12864数据端口定义
sbit LCD12864_PSB = P3^2; // 8位或4并口/串口选择

//函数声明
void lcd12864_init(void);
void lcd12864_clear(void);
void lcd12864_show_string(unsigned char x, unsigned char y, unsigned char *str);

#endif
