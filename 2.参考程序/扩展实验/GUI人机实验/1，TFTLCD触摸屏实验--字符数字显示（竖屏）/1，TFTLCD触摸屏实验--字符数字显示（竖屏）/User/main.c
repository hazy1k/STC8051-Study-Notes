/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 																				  
***************************************************************************************/
#include "public.h"  
#include "uart.h"
#include "tftlcd.h"
#include "touch.h"
#include "gui.h"



//字符数字显示测试
void CharacterShow_Test(void)
{
	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,12,"Hello World!");
	LCD_ShowString(10,30,tftlcd_data.width,tftlcd_data.height,16,"Hello World!");
	LCD_ShowString(10,50,tftlcd_data.width,tftlcd_data.height,24,"Hello World!");
	FRONT_COLOR=WHITE;
	LCD_ShowString(10,80,tftlcd_data.width,tftlcd_data.height,12,"1234567890");
	LCD_ShowString(10,100,tftlcd_data.width,tftlcd_data.height,16,"1234567890");
	LCD_ShowString(10,120,tftlcd_data.width,tftlcd_data.height,24,"1234567890");		
}

void main()
{

	UART_Init();
	TFTLCD_Init();

	CharacterShow_Test();
	
	while(1)
	{
				
	}					
}
