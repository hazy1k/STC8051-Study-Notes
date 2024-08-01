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
#include <stdlib.h>

sbit LED1=P3^6;


#define GRAPH_XSTART				5
#define GRAPH_YSTART				40
#define GRAPH_WIDTH					160
#define GRAPH_HEIGHT				130
#define GRAPH_BACKCOLOR				BLACK
#define GRAPH_FRONTCOLOR			GRAY
#define GRAPH_RIM_LTOC 				0XA535	    //左上外线颜色
#define GRAPH_RIM_LTIC 				0X8431		//左上内线颜色
#define GRAPH_RIM_RBOC 				0XFFFF		//右下外线颜色
#define GRAPH_RIM_RBIC 				0XDEFC		//右下内线颜色


#define VALID_AREA_SPACE_LEFT		30
#define VALID_AREA_SPACE_RIGHT		5
#define VALID_AREA_SPACE_TOP		5
#define VALID_AREA_SPACE_BOTTOM		5
#define VALID_AREA_GRID_XSTART		0
#define VALID_AREA_GRID_YSTART		0	
#define VALID_AREA_GRID_SPACE_X		20
#define VALID_AREA_GRID_SPACE_Y		20
#define VALID_AREA_GRID_LINE_COLOR	GRAY
#define VAILD_AREA_GRID_WIDTH		(GRAPH_WIDTH-VALID_AREA_SPACE_LEFT-VALID_AREA_SPACE_RIGHT)
#define VAILD_AREA_GRID_HEIGHT		(GRAPH_HEIGHT-VALID_AREA_SPACE_TOP-VALID_AREA_SPACE_BOTTOM)
#define VAILD_AREA_GRID_XNUM		(VAILD_AREA_GRID_WIDTH/VALID_AREA_GRID_SPACE_X)
#define VAILD_AREA_GRID_YNUM		(VAILD_AREA_GRID_HEIGHT/VALID_AREA_GRID_SPACE_Y)


#define TEXT_XSTART					GRAPH_XSTART
#define TEXT_YSTART					GRAPH_YSTART+VALID_AREA_SPACE_TOP
#define TEXT_XSPACE					VALID_AREA_GRID_SPACE_X
#define TEXT_YSPACE					VALID_AREA_GRID_SPACE_Y
#define TEXT_WIDTH					GRAPH_WIDTH-VAILD_AREA_GRID_WIDTH
#define TEXT_HEIGHT					GRAPH_HEIGHT-VAILD_AREA_GRID_HEIGHT
#define TEXT_COLOR					BLUE
#define TEXT_FONT					12

#define GRAPH_OBJECT_XSTART			GRAPH_XSTART+VALID_AREA_SPACE_LEFT+VALID_AREA_GRID_XSTART
#define GRAPH_OBJECT_YSTART			VAILD_AREA_GRID_HEIGHT+(GRAPH_YSTART+VALID_AREA_SPACE_TOP+VALID_AREA_GRID_YSTART)
#define GRAPH_OBJECT_COLOR			GREEN		

const u16 text_value[]={60,50,40,30,20,10};

//图形绘制
//x0,y0:图形的左上角坐标
//width,height:图形的尺寸
//backcolor:背景颜色
//frontcolor:前景颜色
void Graph_Create(u16 x0,u16 y0,u16 width,u16 height,u16 backcolor,u16 frontcolor)
{
	u8 i=0;

	gui_draw_vline(x0,y0,height,GRAPH_RIM_LTOC);				//左外线
	gui_draw_hline(x0,y0,width,GRAPH_RIM_LTOC);					//上外线
	gui_draw_vline(x0+1,y0+1,height-2,GRAPH_RIM_LTIC);			//左内线
	gui_draw_hline(x0+1,y0+1,width-2,GRAPH_RIM_LTIC);			//上内线
	gui_draw_vline(x0+width-1,y0,height,GRAPH_RIM_RBOC);		//右外线
	gui_draw_hline(x0,y0+height-1,width,GRAPH_RIM_RBOC);		//下外线
	gui_draw_vline(x0+width-2,y0+1,height-2,GRAPH_RIM_RBIC);	//右内线
	gui_draw_hline(x0+1,y0+height-2,width-2,GRAPH_RIM_RBIC);	//下内线
	gui_fill_rectangle(x0+2,y0+2,width-3,height-3,frontcolor);	//填充内部
	gui_fill_rectangle(x0+VALID_AREA_SPACE_LEFT,y0+VALID_AREA_SPACE_TOP,width-VALID_AREA_SPACE_LEFT-VALID_AREA_SPACE_RIGHT,height-VALID_AREA_SPACE_TOP-VALID_AREA_SPACE_BOTTOM,backcolor);
	
	for(i=0;i<VAILD_AREA_GRID_XNUM;i++)
	{
		gui_draw_vline(x0+VALID_AREA_SPACE_LEFT+VALID_AREA_GRID_XSTART+(VALID_AREA_GRID_SPACE_X*i),y0+VALID_AREA_SPACE_TOP,height-VALID_AREA_SPACE_TOP-VALID_AREA_SPACE_BOTTOM,VALID_AREA_GRID_LINE_COLOR);	
	}
	for(i=0;i<VAILD_AREA_GRID_YNUM;i++)
	{
		gui_draw_hline(x0+VALID_AREA_SPACE_LEFT+VALID_AREA_GRID_XSTART,y0+VALID_AREA_SPACE_TOP+(VALID_AREA_GRID_SPACE_Y*i),width-VALID_AREA_SPACE_LEFT-VALID_AREA_SPACE_RIGHT,VALID_AREA_GRID_LINE_COLOR);	
	}			
}

void ReDraw_ValidAreaGird(u16 x0,u16 y0,u16 width,u16 height,u16 backcolor,u16 frontcolor)
{
	u8 i=0;

	gui_fill_rectangle(x0+VALID_AREA_SPACE_LEFT,y0+VALID_AREA_SPACE_TOP,width-VALID_AREA_SPACE_LEFT-VALID_AREA_SPACE_RIGHT,height-VALID_AREA_SPACE_TOP-VALID_AREA_SPACE_BOTTOM,backcolor);
	
	for(i=0;i<VAILD_AREA_GRID_XNUM;i++)
	{
		gui_draw_vline(x0+VALID_AREA_SPACE_LEFT+VALID_AREA_GRID_XSTART+(VALID_AREA_GRID_SPACE_X*i),y0+VALID_AREA_SPACE_TOP,height-VALID_AREA_SPACE_TOP-VALID_AREA_SPACE_BOTTOM,VALID_AREA_GRID_LINE_COLOR);	
	}
	for(i=0;i<VAILD_AREA_GRID_YNUM;i++)
	{
		gui_draw_hline(x0+VALID_AREA_SPACE_LEFT+VALID_AREA_GRID_XSTART,y0+VALID_AREA_SPACE_TOP+(VALID_AREA_GRID_SPACE_Y*i),width-VALID_AREA_SPACE_LEFT-VALID_AREA_SPACE_RIGHT,VALID_AREA_GRID_LINE_COLOR);	
	}		
}



void Graph_AddText(u16 *value)
{
	u8 i=0;
	char text_str[VAILD_AREA_GRID_YNUM+1];

	for(i=0;i<VAILD_AREA_GRID_YNUM;i++)
	{	
		gui_num2strex(value[i], &text_str[i]);
		gui_show_strmid(TEXT_XSTART,TEXT_YSTART+(TEXT_YSPACE*i),TEXT_WIDTH,TEXT_HEIGHT,TEXT_COLOR,TEXT_FONT,&text_str[i]);	
	}
}

void Graph_AddValue(u16 x0,u16 y0,u16 value,u16 color,u16 sampletime)
{
	static u16 xtemp=GRAPH_OBJECT_XSTART;
	static u16 ytemp=GRAPH_OBJECT_YSTART;

	gui_draw_bline1(xtemp,ytemp,x0+sampletime,y0-value,1,color);
	xtemp=x0+sampletime;
	ytemp=y0-value;

	if((sampletime>VAILD_AREA_GRID_WIDTH) || (value>VAILD_AREA_GRID_HEIGHT))
	{
		xtemp=GRAPH_OBJECT_XSTART;
		ytemp=GRAPH_OBJECT_YSTART;
		ReDraw_ValidAreaGird(GRAPH_XSTART,GRAPH_YSTART,GRAPH_WIDTH,GRAPH_HEIGHT,GRAPH_BACKCOLOR,GRAPH_FRONTCOLOR);						
	}
}



//图形测试
void Graph_Test(void)
{
	u8 i=0;
	u8 value=0;

	Graph_Create(GRAPH_XSTART,GRAPH_YSTART,GRAPH_WIDTH,GRAPH_HEIGHT,GRAPH_BACKCOLOR,GRAPH_FRONTCOLOR);
	Graph_AddText(text_value);
	while(1)
	{
		i++;
		value++;
		Graph_AddValue(GRAPH_OBJECT_XSTART,GRAPH_OBJECT_YSTART,value,GRAPH_OBJECT_COLOR,value);
		if(value>VAILD_AREA_GRID_WIDTH||value>VAILD_AREA_GRID_HEIGHT)value=0;
		if(i%1==0)
		{
			LED1=!LED1;
		}
		delay_ms(200);			
	}	
}



void main()
{

	UART_Init();
	TFTLCD_Init();

	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"Graph Test");

	Graph_Test();
	
	while(1)
	{
					
	}					
}
