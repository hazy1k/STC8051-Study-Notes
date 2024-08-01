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



//几何图形测试
void BaseGraphical_Test(void)
{
	FRONT_COLOR=RED;
	LCD_ShowString(tftlcd_data.width/2-7*12,0,tftlcd_data.width,tftlcd_data.height,16,"Base Graphical Test");
	
	FRONT_COLOR=WHITE;
	gui_draw_bigpoint(10,20,GREEN);
	gui_draw_bline(10,30,120,30,5,WHITE);
	gui_draw_rectangle(10,40,30,30,BLUE);
	gui_draw_arcrectangle(50,40,30,30,5,1,YELLOW,GREEN);
	gui_fill_rectangle(90,40,30,30,BRRED);
	gui_fill_ellipse(30,100,20,10,GBLUE);
	gui_fill_circle(80,100,20,MAGENTA);
	gui_draw_ellipse(30,130,20,10,WHITE);
	gui_draw_arc(60,140,100,180,80,160,20,WHITE,0);		
}

void main()
{

	UART_Init();
	TFTLCD_Init();

	BaseGraphical_Test();
	
	while(1)
	{
				
	}					
}
