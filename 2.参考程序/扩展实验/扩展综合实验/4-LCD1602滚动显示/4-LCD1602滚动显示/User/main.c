/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 
实验名称：LCD1602滚动显示
接线说明：	
实验现象：下载程序后，LCD1602上滚动显示
注意事项：按照LCD1602实验章节插好LCD1602到开发板																				  
***************************************************************************************/
#include "public.h"
#include "lcd1602.h"


extern void lcd1602_write_cmd(u8 cmd);
extern void lcd1602_write_data(u8 dat); 
/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	u8 i=0;
	u8 gshowbufa[16]=" Hello World... ";
	u8 gshowbufb[27]="welcome to the world of mcu";
	
	lcd1602_init();
	lcd1602_show_string(0,0,gshowbufa);
	lcd1602_show_string(0,1,gshowbufb);
	lcd1602_write_cmd(0x07);//每写一个数据屏幕就要右移一位，就相对于数据来说就是左移了
	while(1)
	{
		lcd1602_write_cmd(0x80);
		for(i=0;i<16;i++)
		{
			lcd1602_write_data(gshowbufa[i]);
			delay_ms(500);//滚动速度调节	
		}		
	}
}
