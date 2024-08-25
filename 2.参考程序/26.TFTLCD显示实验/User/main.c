#include "public.h"  
#include "uart.h"
#include "tftlcd.h"
#include "gui.h"
#include "picture.h"

void main()
{
	UART_Init(); // 串口初始化，方便使用printf调试
	TFTLCD_Init(); // TFTLCD初始化
		
	FRONT_COLOR = WHITE;
	LCD_ShowString(10,80,tftlcd_data.width,tftlcd_data.height,16,"0123456789");
	//LCD_ShowFontHZ(10,100,"好好好");
	LCD_ShowPicture(10,60,56,56,gImage_picture_snow);
	
	while(1)
	{
		
	}					
}
