#include "public.h"
#include "lcd1602.h"

void main()
{	
	lcd1602_init(); // LCD1602初始化

	lcd1602_show_string(0, 0, "Hello hazy1k"); // 第一行显示字符
	lcd1602_show_string(0, 1, "0123456789"); // 第二行显示数字

	while(1)
	{
		
	}	
}
