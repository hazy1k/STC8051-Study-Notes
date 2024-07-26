#include "lcd1602.h"

// 8位驱动1602写入函数
#if (LCD1602_4OR8_DATA_INTERFACE == 0)

// 写入命令函数
void lcd1602_write_cmd(unsigned char cmd)
{
	LCD1602_RS = 0; // 选择命令
	LCD1602_RW = 0; // 选择写
	LCD1602_E = 0;

	LCD1602_DATAPORT = cmd; // 准备命令
	delay_ms(1);

	LCD1602_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);

	LCD1602_E = 0; // 使能脚E后负跳变完成写入	
}
#else	// 4位LCD
void lcd1602_write_cmd(unsigned char cmd)
{
	LCD1602_RS = 0;//选择命令
	LCD1602_RW = 0;//选择写
	LCD1602_E = 0;
	LCD1602_DATAPORT = cmd;//准备命令
	delay_ms(1);

	LCD1602_E = 1;//使能脚E先上升沿写入
	delay_ms(1);

	LCD1602_E = 0;//使能脚E后负跳变完成写入
	
	LCD1602_DATAPORT=cmd<<4; // 准备命令
	delay_ms(1);

	LCD1602_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);

	LCD1602_E = 0;//使能脚E后负跳变完成写入	
}
#endif

// 写入数据函数
#if (LCD1602_4OR8_DATA_INTERFACE == 0) // 8位LCD
void lcd1602_write_data(unsigned char dat) 
{
	LCD1602_RS = 1; // 选择数据
	LCD1602_RW = 0; // 选择写
	LCD1602_E = 0;

	LCD1602_DATAPORT = dat; // 准备数据
	delay_ms(1);

	LCD1602_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);

	LCD1602_E = 0; // 使能脚E后负跳变完成写入		
}
#else
void lcd1602_write_data(unsigned char dat) // 4位
{
	LCD1602_RS = 1; // 选择数据
	LCD1602_RW = 0; // 选择写
	LCD1602_E = 0;

	LCD1602_DATAPORT = dat;//准备数据
	delay_ms(1);

	LCD1602_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);

	LCD1602_E = 0; // 使能脚E后负跳变完成写入
	
	LCD1602_DATAPORT=dat<<4; // 准备数据
	delay_ms(1);

	LCD1602_E = 1; // 使能脚E先上升沿写入
	delay_ms(1);

	LCD1602_E = 0; // 使能脚E后负跳变完成写入		
}
#endif

// 1602初始化函数
#if (LCD1602_4OR8_DATA_INTERFACE == 0) // 8位LCD
void lcd1602_init(void)
{
	lcd1602_write_cmd(0x38); // 数据总线8位，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c); // 显示功能开，无光标，光标闪烁
	lcd1602_write_cmd(0x06); // 写入新数据后光标右移，显示屏不移动
	lcd1602_write_cmd(0x01); // 清屏	
}
#else
void lcd1602_init(void) // 4位
{
	lcd1602_write_cmd(0x28); // 数据总线4位，显示2行，5*7点阵/字符
	lcd1602_write_cmd(0x0c); // 显示功能开，无光标，光标闪烁
	lcd1602_write_cmd(0x06); // 写入新数据后光标右移，显示屏不移动
	lcd1602_write_cmd(0x01); // 清屏	
}
#endif

// 1602清屏函数
void lcd1602_clear(void)
{
	lcd1602_write_cmd(0x01);	
}

// 1602显示字符串函数（xy代表坐标，str代表字符串）
void lcd1602_show_string(unsigned char x,unsigned char y,unsigned char *str)
{
	unsigned char i=0;

	if(y>1 || x>15)
		return; // 行列参数不对则强制退出

	if(y < 1) // 第1行显示
	{	
		while(*str != '\0')//字符串是以'\0'结尾，只要前面有内容就显示
		{
			if(i <16 - x)//如果字符长度超过第一行显示范围，则在第二行继续显示
			{
				lcd1602_write_cmd(0x80+i+x); // 第一行显示地址设置	
			}
			else
			{
				lcd1602_write_cmd(0x40+0x80+i+x-16); // 第二行显示地址设置	
			}
			lcd1602_write_data(*str); // 显示内容

			str++; // 指针递增
			i++;	
		}	
	}
	else // 第2行显示
	{
		while(*str != '\0')
		{
			if(i < 16 - x) //如果字符长度超过第二行显示范围，则在第一行继续显示
			{
				lcd1602_write_cmd(0x80+0x40+i+x);	
			}
			else
			{
				lcd1602_write_cmd(0x80+i+x-16);	
			}
			lcd1602_write_data(*str);

			str++;
			i++;	
		}	
	}				
}
