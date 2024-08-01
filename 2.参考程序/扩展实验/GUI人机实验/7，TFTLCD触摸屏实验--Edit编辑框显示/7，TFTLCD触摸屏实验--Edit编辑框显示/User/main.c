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



#define EDIT_START_X		65	  	//编辑框起始X坐标
#define EDIT_START_Y		50		//编辑框起始Y坐标
#define EDIT_WIDTH			60		//编辑框长
#define EDIT_HEIGHT			20		//编辑框高

#define EDIT_RIM_LTOC 		0XA535	    //左上外线颜色
#define EDIT_RIM_LTIC 		0X8431		//左上内线颜色
#define EDIT_RIM_RBOC 		0XFFFF		//右下外线颜色
#define EDIT_RIM_RBIC 		0XDEFC		//右下内线颜色

#define EDIT_BACKCOLOR		WHITE  	//背景色
#define EDIT_FRONTCOLOR		BLUE   //前景色
#define EDIT_FONTSIZE		16   	//文本框内字体大小

//编辑框创建
//x0,y0:矩形的左上角坐标
//width,height:文本框的尺寸
//backcolor:背景颜色
void Edit_Create(u16 x0,u16 y0,u16 width,u16 height,u16 backcolor)
{
	gui_draw_vline(x0,y0,height,EDIT_RIM_LTOC);			//左外线
	gui_draw_hline(x0,y0,width,EDIT_RIM_LTOC);			//上外线
	gui_draw_vline(x0+1,y0+1,height-2,EDIT_RIM_LTIC);	//左内线
	gui_draw_hline(x0+1,y0+1,width-2,EDIT_RIM_LTIC);	//上内线
	gui_draw_vline(x0+width-1,y0,height,EDIT_RIM_RBOC);		//右外线
	gui_draw_hline(x0,y0+height-1,width,EDIT_RIM_RBOC);		//下外线
	gui_draw_vline(x0+width-2,y0+1,height-2,EDIT_RIM_RBIC);	//右内线
	gui_draw_hline(x0+1,y0+height-2,width-2,EDIT_RIM_RBIC);	//下内线
	gui_fill_rectangle(x0+2,y0+2,width-3,height-3,backcolor);//填充内部	
}

//编辑框显示字符串
//x0,y0:矩形的左上角坐标
//width,height:文本框的尺寸
//backcolor:背景颜色
//frontcolor:前景颜色
//size:文字大小
//str:字符串
void Edit_ShowString(u16 x0,u16 y0,u16 width,u16 height,u16 backcolor,u16 frontcolor,u8 size,u8 *str)
{
	static u16 color_temp=0;

	color_temp=BACK_COLOR;
	BACK_COLOR=backcolor;
	gui_show_strmidex(x0,y0,width,height,frontcolor,size,str,0);
	BACK_COLOR=color_temp;	
}

//编辑框显示数字
//x0,y0:矩形的左上角坐标
//len :数字的位数
//backcolor:背景颜色
//frontcolor:前景颜色
//size:字体大小
//num:数值(0~2^64);
void Edit_ShowNum(u16 x0,u16 y0,u8 len,u16 backcolor,u16 frontcolor,u8 size,long num)
{
	static u16 color_temp=0;

	color_temp=BACK_COLOR;
	BACK_COLOR=backcolor;
	gui_show_num(x0+4,y0+3,len,frontcolor,size,num,0);
	BACK_COLOR=color_temp;	
}

//编辑框测试
void Edit_Test(void)
{
	u16 t=0;

	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"Edit Test");
	FRONT_COLOR=GREEN;
	LCD_ShowString(0,50,tftlcd_data.width,tftlcd_data.height,16,"Counter:"); 
	LCD_ShowString(0,80,tftlcd_data.width,tftlcd_data.height,16,"String :");
	Edit_Create(EDIT_START_X,EDIT_START_Y,EDIT_WIDTH,EDIT_HEIGHT,EDIT_BACKCOLOR); 
	Edit_Create(EDIT_START_X,EDIT_START_Y+30,EDIT_WIDTH,EDIT_HEIGHT,EDIT_BACKCOLOR);
	while(1)
	{
		t++;
		Edit_ShowNum(EDIT_START_X,EDIT_START_Y,6,EDIT_BACKCOLOR,EDIT_FRONTCOLOR,EDIT_FONTSIZE,t);			
	 	delay_ms(100);
		Edit_ShowString(EDIT_START_X,EDIT_START_Y+30,EDIT_WIDTH,EDIT_HEIGHT,EDIT_BACKCOLOR,EDIT_FRONTCOLOR,EDIT_FONTSIZE,"Hello");
	}	
}

void main()
{

	UART_Init();
	TFTLCD_Init();

	Edit_Test();
	
	while(1)
	{
				
	}					
}
