#ifndef _lcd9648_H
#define _lcd9648_H

#include "public.h"



//管脚定义
sbit CS0  = P0^3;		//片选
sbit RST  = P0^2;		//复位
sbit RS   = P0^4; 		//数据命令选择端
sbit SCL  = P0^6;		//SPI时钟端
sbit SDA  = P0^5;		//SPI数据端

//LCD显示范围
#define LCD_WIDTH	96
#define LCD_HEIGHT	48

//函数声明
void lcd9648_init(void);
void lcd9648_clear(void);
void lcd_reflash_gram(void);
void lcd_draw_dot(u8 x,u8 y,u8 sta);
void lcd_show_char(u8 x,u8 y,u8 num,u8 size,u8 mode);
void lcd_show_num(u8 x,u8 y,u32 num,u8 len,u8 size,u8 mode);
void lcd_show_string(u8 x,u8 y,u8 width,u8 height,u8 size,u8 *p);
void lcd_show_fontHZ(u8 x, u8 y, u8 *cn);

#endif
