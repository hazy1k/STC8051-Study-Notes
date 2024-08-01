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



//汉字显示测试
void HZFontShow_Test(void)
{
	FRONT_COLOR=RED;
	LCD_ShowFontHZ(10,10,"普中科技");
	FRONT_COLOR=WHITE;
	LCD_ShowFontHZ(10,50,"普中科技");
	FRONT_COLOR=GREEN;
	LCD_ShowFontHZ(10,90,"普中科技");	
}

void main()
{

	UART_Init();
	TFTLCD_Init();

	HZFontShow_Test();
	
	while(1)
	{
				
	}					
}
