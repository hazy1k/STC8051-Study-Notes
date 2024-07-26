#include "public.h"
#include "lcd12864.h"

void main()
{	
	lcd12864_init(); // LCD12864初始化

	lcd12864_show_string(0,0,"Hello World!"); // 第1行字符串显示
	lcd12864_show_string(0,1,"大家好！"); // 第2行字符串显示
	lcd12864_show_string(0,2,"欢迎使用51开发板"); // 第3行字符串显示
	lcd12864_show_string(0,3,"好好学习天天向上"); // 第4行字符串显示
	while(1)
	{
		
	}	
}
