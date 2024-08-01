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


sbit LED1=P3^5;
sbit LED2=P3^6;
sbit LED3=P3^7;	


#define BUTTON_START_X		2	  		//起始X坐标
#define BUTTON_START_Y		70			//起始Y坐标
#define BUTTON_WIDTH		50			//长
#define BUTTON_HEIGHT		40			//高
#define BUTTON_ARCSIZE		4  			//默认圆角的半径

#define BUTTON_SPACE_X		10  			//X间隔
#define BUTTON_SPACE_Y		10  			//Y间隔

//按钮颜色定义
#define BTN_DFU_BCFUC		0X0000		//默认松开的颜色
#define BTN_DFU_BCFDC		0X0000		//默认按下的颜色

#define ARC_BTN_RIMC		0X0000		//圆角按钮边框颜色
#define ARC_BTN_TP1C		0XD6BB		//第一行的颜色
#define ARC_BTN_UPHC		0X8452		//上半部分颜色
#define ARC_BTN_DNHC		0X52CD		//下半部分颜色

#define BUTTON_BACKCOLOR	0XEF5D  	//背景色
#define BUTTON_FRONTCOLOR	BLACK   	//前景色
#define BUTTON_FONTSIZE		16   		//字体大小


//LED1按键触摸比较值
#define LED1_START_X		BUTTON_START_X
#define LED1_START_Y		BUTTON_START_Y
#define LED1_END_X			BUTTON_START_X+BUTTON_WIDTH
#define LED1_END_Y			BUTTON_START_Y+BUTTON_HEIGHT

//LED2按键触摸比较值
#define LED2_START_X		BUTTON_START_X+BUTTON_WIDTH+BUTTON_SPACE_X
#define LED2_START_Y		BUTTON_START_Y
#define LED2_END_X			BUTTON_START_X+BUTTON_WIDTH*2+BUTTON_SPACE_X
#define LED2_END_Y			BUTTON_START_Y+BUTTON_HEIGHT

//LED3按键触摸比较值
#define LED3_START_X		BUTTON_START_X+BUTTON_WIDTH*2+BUTTON_SPACE_X*2
#define LED3_START_Y		BUTTON_START_Y
#define LED3_END_X			BUTTON_START_X+BUTTON_WIDTH*3+BUTTON_SPACE_X*2
#define LED3_END_Y			BUTTON_START_Y+BUTTON_HEIGHT


//LED图形参数
#define LED_START_X		30	  			//LED起始X坐标
#define LED_START_Y		150				//LED起始Y坐标
#define LED_SPACE_X		30				//LED间隔X坐标
#define LED_RADIUS		15				//LED图形圆半径
#define LED_DIAMETER	LED_RADIUS*2	//LED图形圆直径

#define LED_BACKCOLOR	WHITE  			//背景色
#define LED_FRONTCOLOR	GREEN   		//前景色

//按钮创建
//x0,y0:矩形的左上角坐标
//width,height:文本框的尺寸
//backcolor:背景颜色
void Button_Create(u16 x0,u16 y0,u16 width,u16 height,u8 r,u16 backcolor,u16 frontcolor,u8 size,u8*str)
{
	gui_draw_arcrectangle(x0,y0,width,height,r,1,ARC_BTN_UPHC,ARC_BTN_DNHC);//填充圆角按钮
	gui_draw_arcrectangle(x0,y0,width,height,r,0,ARC_BTN_RIMC,ARC_BTN_RIMC);//画圆角边框
	gui_draw_hline (x0+r,y0+1,width-2*r,ARC_BTN_TP1C);//第一行
	gui_show_strmid(x0+1,y0+1,width,height,frontcolor,size,str);
}

//按钮按下显示
//x0,y0:矩形的左上角坐标
//width,height:文本框的尺寸
//backcolor:背景颜色
void Button_PressCreate(u16 x0,u16 y0,u16 width,u16 height,u8 r,u16 backcolor,u16 frontcolor,u8 size,u8*str)
{	
	gui_draw_arcrectangle(x0,y0,width,height,r,1,WHITE,WHITE);//填充圆角按钮
	gui_draw_arcrectangle(x0,y0,width,height,r,0,GREEN,GREEN);//画圆角边框
	gui_draw_hline (x0+r,y0+1,width-2*r,ARC_BTN_TP1C);//第一行
	gui_show_strmid(x0+1,y0+1,width,height,frontcolor,size,str);
}


const u8 Button_text[][5]={"LED1","LED2","LED3"};

//按钮测试
void Button_Test(void)
{
	u8 i=0;
	u8 LED1_PressFlag=0;
	u8 LED2_PressFlag=0;
	u8 LED3_PressFlag=0;

	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"Button Test");

	for(i=0;i<3;i++)
	{
		Button_Create(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*i,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[i]); 		
	  	gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*i,LED_START_Y,LED_RADIUS,LED_BACKCOLOR);
	}
	
	while(1)
	{
		TOUCH_Scan();

		if(xpt_xy.sta)
		{	
			//LED1
			if((xpt_xy.lcdx>=LED1_START_X) && (xpt_xy.lcdx<=LED1_END_X) && 
				(xpt_xy.lcdy>=LED1_START_Y) && (xpt_xy.lcdy<=LED1_END_Y))
			{
				xpt_xy.lcdx=0;
				xpt_xy.lcdy=0;
				LED1_PressFlag=1;
			}
		
			//LED2
			else if((xpt_xy.lcdx>=LED2_START_X) && (xpt_xy.lcdx<=LED2_END_X) && 
				(xpt_xy.lcdy>=LED2_START_Y) && (xpt_xy.lcdy<=LED2_END_Y))
			{
				xpt_xy.lcdx=0;
				xpt_xy.lcdy=0;
				LED2_PressFlag=1;
			}
			
			//LED3
			else if((xpt_xy.lcdx>=LED3_START_X) && (xpt_xy.lcdx<=LED3_END_X) && 
				(xpt_xy.lcdy>=LED3_START_Y) && (xpt_xy.lcdy<=LED3_END_Y))
			{
				xpt_xy.lcdx=0;
				xpt_xy.lcdy=0;
				LED3_PressFlag=1;
			}
		}
		else
		{
			if(LED1_PressFlag)
			{
				LED1_PressFlag=0;
				Button_Create(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*0,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[0]); 		
			  	LED1=1;
				gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*0,LED_START_Y,LED_RADIUS,LED_BACKCOLOR);  					
			}
			else if(LED2_PressFlag)
			{
				LED2_PressFlag=0;
				Button_Create(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*1,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[1]); 		
			  	LED2=1;
				gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*1,LED_START_Y,LED_RADIUS,LED_BACKCOLOR);  					
			}
			else if(LED3_PressFlag)
			{
				LED3_PressFlag=0;
				Button_Create(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*2,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[2]); 		
			  	LED3=1;
				gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*2,LED_START_Y,LED_RADIUS,LED_BACKCOLOR);  					
			}		
		}
		
		if(LED1_PressFlag)
		{
			Button_PressCreate(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*0,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[0]);
			LED1=0;
			gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*0,LED_START_Y,LED_RADIUS,LED_FRONTCOLOR);			
		}
		else if(LED2_PressFlag)
		{
			Button_PressCreate(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*1,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[1]);
			LED2=0;
			gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*1,LED_START_Y,LED_RADIUS,LED_FRONTCOLOR);			
		}
		else if(LED3_PressFlag)
		{
			Button_PressCreate(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*2,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[2]);
			LED3=0;
			gui_fill_circle(LED_START_X+(LED_DIAMETER+LED_SPACE_X)*2,LED_START_Y,LED_RADIUS,LED_FRONTCOLOR);			
		}			
	}	
}

void main()
{

	UART_Init();
	TFTLCD_Init();

	Button_Test();
	
	while(1)
	{
				
	}					
}
