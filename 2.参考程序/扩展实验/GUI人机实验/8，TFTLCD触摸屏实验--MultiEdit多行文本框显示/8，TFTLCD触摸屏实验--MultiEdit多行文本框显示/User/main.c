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



#define MULTIEDIT_START_X		10	  		//起始X坐标
#define MULTIEDIT_START_Y		70			//起始Y坐标
#define MULTIEDIT_WIDTH			150			//长
#define MULTIEDIT_HEIGHT		100			//高

#define MULTIEDIT_RIM_LTOC 		0XA535	    //左上外线颜色
#define MULTIEDIT_RIM_LTIC 		0X8431		//左上内线颜色
#define MULTIEDIT_RIM_RBOC 		0XFFFF		//右下外线颜色
#define MULTIEDIT_RIM_RBIC 		0XDEFC		//右下内线颜色

#define MULTIEDIT_BACKCOLOR		WHITE  		//背景色
#define MULTIEDIT_FRONTCOLOR	BLUE   		//前景色
#define MULTIEDIT_FONTSIZE		16   		//文本框内字体大小

//多行文本框创建
//x0,y0:矩形的左上角坐标
//width,height:文本框的尺寸
//backcolor:背景颜色
void MultiEdit_Create(u16 x0,u16 y0,u16 width,u16 height,u16 backcolor)
{
	gui_draw_vline(x0,y0,height,MULTIEDIT_RIM_LTOC);			//左外线
	gui_draw_hline(x0,y0,width,MULTIEDIT_RIM_LTOC);			//上外线
	gui_draw_vline(x0+1,y0+1,height-2,MULTIEDIT_RIM_LTIC);	//左内线
	gui_draw_hline(x0+1,y0+1,width-2,MULTIEDIT_RIM_LTIC);	//上内线
	gui_draw_vline(x0+width-1,y0,height,MULTIEDIT_RIM_RBOC);		//右外线
	gui_draw_hline(x0,y0+height-1,width,MULTIEDIT_RIM_RBOC);		//下外线
	gui_draw_vline(x0+width-2,y0+1,height-2,MULTIEDIT_RIM_RBIC);	//右内线
	gui_draw_hline(x0+1,y0+height-2,width-2,MULTIEDIT_RIM_RBIC);	//下内线
	gui_fill_rectangle(x0+2,y0+2,width-3,height-3,backcolor);//填充内部	
}

//多行文本框显示字符串
//x0,y0:矩形的左上角坐标
//width,height:文本框的尺寸
//offset:开始显示的偏移
//backcolor:背景颜色
//frontcolor:前景颜色
//size:文字大小
//str:字符串
void MultiEdit_ShowString(u16 x0,u16 y0,u16 width,u16 height,u16 xoffset,u16 yoffset,u16 backcolor,u16 frontcolor,u8 size,u8 *str)
{
	static u16 color_temp=0;

	color_temp=BACK_COLOR;
	BACK_COLOR=backcolor;
	gui_show_stringex(x0+xoffset,y0+yoffset,width,height,frontcolor,size,str,0);
	BACK_COLOR=color_temp;	
}

const u8 multiedit_testbuf[]={"Hello everyone,I am MuYi,I'm very happy to learn with you."};

//多行文本框测试
void MultiEdit_Test(void)
{
	u16 t=0;

	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"MultiEdit Test");
	FRONT_COLOR=GREEN;
	LCD_ShowString(10,50,tftlcd_data.width,tftlcd_data.height,16,"Send Message:"); 

	MultiEdit_Create(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,MULTIEDIT_BACKCOLOR); 
	MultiEdit_ShowString(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,4,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,multiedit_testbuf);
	while(1)
	{
			
	}	
}

void main()
{

	UART_Init();
	TFTLCD_Init();

	MultiEdit_Test();
	
	while(1)
	{
				
	}					
}
