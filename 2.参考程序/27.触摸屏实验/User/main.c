
#include "public.h"  
#include "uart.h"
#include "tftlcd.h"
#include "touch.h"
#include "gui.h"


u8 Touch_RST=0;


// 触摸测试
void TOUCH_Test(void)
{
	
	static u16 penColor = BLUE;

	TOUCH_Scan();
	if(xpt_xy.sta)
	{	
		
		if(xpt_xy.lcdx>tftlcd_data.width)
			xpt_xy.lcdx=tftlcd_data.width-1;
		if(xpt_xy.lcdy>tftlcd_data.height)
			xpt_xy.lcdy=tftlcd_data.height-1;
		if((xpt_xy.lcdx>=(tftlcd_data.width-3*12))&&(xpt_xy.lcdy<24))
			Touch_RST=1;
		if(xpt_xy.lcdy > tftlcd_data.height - 20)
		{
			if(xpt_xy.lcdx>100)
            {
                penColor = YELLOW;
            }
            else if(xpt_xy.lcdx>80)
            {
                penColor = CYAN;
            }
            else if(xpt_xy.lcdx>60)
            {
                penColor = GREEN;
            }
            else if(xpt_xy.lcdx>40)
            {
               penColor = MAGENTA;
            }
            else if(xpt_xy.lcdx>20)
            {
                penColor = RED;

            }
            else if(xpt_xy.lcdx>0)
            {
                penColor = BLUE;
            }       
		}
		else
		{
			LCD_Fill(xpt_xy.lcdx-2,xpt_xy.lcdy-2,xpt_xy.lcdx+2,xpt_xy.lcdy+2,penColor);	
		}
							
	}		
}

//GUI测试
void GUI_Test(void)
{
	FRONT_COLOR=BLACK;
	LCD_ShowFontHZ(tftlcd_data.width/2-2*24-12, 0,"普中科技");
	LCD_ShowString(tftlcd_data.width/2-7*12,30,tftlcd_data.width,tftlcd_data.height,24,"www.prechin.cn");
	LCD_ShowString(tftlcd_data.width/2-7*8,55,tftlcd_data.width,tftlcd_data.height,24,"GUI Test");
	
	FRONT_COLOR=RED;
	gui_draw_bigpoint(10,55,GREEN);
	gui_draw_bline(10,80,120,80,10,GREEN);
	gui_draw_rectangle(10,95,30,30,GREEN);
	gui_draw_arcrectangle(50,95,30,30,5,1,BLUE,GREEN);
	gui_fill_rectangle(90,95,30,30,GREEN);
	//gui_fill_circle(90,120,20,GREEN);
	gui_fill_ellipse(30,150,20,10,GREEN);

	//更多的GUI等待大家来发掘和编写
	
	delay_ms(2000);
	LCD_Clear(BACK_COLOR);		
}

void main()
{
	
	u16 color=0;

	UART_Init();
	TFTLCD_Init();

Start:
	//GUI_Test();
		
	FRONT_COLOR = BLACK;
	LCD_ShowFontHZ(tftlcd_data.width/2-2*24-12, 0,"普中科技"); // 显示标题
	LCD_ShowString(tftlcd_data.width/2-7*12,30,tftlcd_data.width,tftlcd_data.height,24,"www.prechin.cn"); // 显示地址
	FRONT_COLOR = RED;
	LCD_ShowString(tftlcd_data.width-3*12,0,tftlcd_data.width,tftlcd_data.height,24,"RST"); // 显示触摸按键提示
	LCD_Fill(0, tftlcd_data.height - 20, 20, tftlcd_data.height, BLUE); // 显示颜色选择提示
    LCD_Fill(20, tftlcd_data.height - 20, 40, tftlcd_data.height, RED); 
    LCD_Fill(40, tftlcd_data.height - 20, 60, tftlcd_data.height, MAGENTA);
    LCD_Fill(60, tftlcd_data.height - 20, 80, tftlcd_data.height, GREEN);
    LCD_Fill(80, tftlcd_data.height - 20, 100, tftlcd_data.height, CYAN);
    LCD_Fill(100, tftlcd_data.height - 20, 120, tftlcd_data.height, YELLOW);
	
	while(1)
	{
		if(Touch_RST) 
		{
			Touch_RST = 0;
			LCD_Clear(BACK_COLOR);
			goto Start;
		}
		
		TOUCH_Test();
			
	}					
}
