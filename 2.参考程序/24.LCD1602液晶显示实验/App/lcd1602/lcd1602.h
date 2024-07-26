#ifndef _lcd1602_H
#define _lcd1602_H

#include "public.h"

// LCD1602数据口4位和8位定义，若为1，则为LCD1602四位数据口驱动，反之为8位
#define LCD1602_4OR8_DATA_INTERFACE	0 // 默认使用8位数据口LCD1602

//管脚定义
sbit LCD1602_RS = P2^6; // 数据命令选择
sbit LCD1602_RW = P2^5; // 读写选择
sbit LCD1602_E = P2^7; // 使能信号
#define LCD1602_DATAPORT P0	// 宏定义LCD1602数据端口


//函数声明
void lcd1602_init(void); // 初始化
void lcd1602_clear(void); // 清屏指令
void lcd1602_show_string(unsigned char x,unsigned char y,unsigned char *str); // 字符显示函数

#endif