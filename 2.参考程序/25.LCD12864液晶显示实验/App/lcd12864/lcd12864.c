#include "lcd12864.h"

// 写入命令函数
void lcd12864_write_cmd(unsigned char cmd)
{
	// 1. 选择命令
	LCD12864_RS = 0;

	// 2. 选择写命令
	LCD12864_WR = 0;

	// 3. 使能信号
	LCD12864_E = 0;

	// 4. 准备写入命令
	LCD12864_DATAPORT = cmd;
	delay_ms(1);

	LCD12864_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);

	LCD12864_E = 0; // 使能脚E后负跳变完成写入
}

// 写入数据函数
void lcd12864_write_data(unsigned char dat)
{
	// 1. 选择数据
	LCD12864_RS = 1;

    // 2. 选择写命令
	LCD12864_WR = 0;

	// 3. 使能信号
	LCD12864_E = 0;

	// 4. 准备写入数据
	LCD12864_DATAPORT = dat; // 准备数据
	delay_ms(1);

	LCD12864_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);
	LCD12864_E = 0; // 使能脚E后负跳变完成写入
}

// 初始化函数
void lcd12864_init(void)
{
	// 1. 选择8位模式
	LCD12864_PSB = 1;

	// 以下写入命令是根据芯片来的，不知道可以网上查一下
	lcd12864_write_cmd(0x30); // 数据总线8位，基本指令操作
	lcd12864_write_cmd(0x0c); // 整体显示关，游标显示关，游标正常显示
	lcd12864_write_cmd(0x06); // 写入新数据后光标右移，显示屏不移动
	lcd12864_write_cmd(0x01); // 清屏	
}

// 清屏函数
void lcd12864_clear(void)
{
	lcd12864_write_cmd(0x01);	
} 

// 显示字符串函数
// x,y: 显示位置坐标 str：代表要显示的字符串
void lcd12864_show_string(unsigned char x,unsigned char y,unsigned char *str)
{
	if(y <= 0) // 限制y不能小于0
		y = 0;
	if( y > 3) // 限制y不能大于3
		y = 3;

	x &= 0x0f; // 限制x,y不能大于显示范围

	switch(y)
	{
		case 0: 
			x |= 0x80; // 第1行地址+x的偏移
			break; 
		case 1: 
			x |= 0x90; // 第2行地址+x的偏移
			break;
		case 2: 
			x |= 0x88; // 第3行地址+x的偏移
			break;
		case 3: 
			x |= 0x98; // 第4行地址+x的偏移
			break;
	}
	lcd12864_write_cmd(x); // 写入显示位置坐标
	while(*str != '\0') // 当字符串不为空时
	{
		lcd12864_write_data(*str); // 写入显示字符
		str++;		
	}		
} 