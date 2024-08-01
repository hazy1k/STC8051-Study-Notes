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
#include "time.h"
#include "math.h"




//π值定义
#define	app_pi	3.1415926535897932384626433832795

#define CALENDAR_CENTER_X			85
#define CALENDAR_CENTER_Y			130
#define CALENDAR_DIAMETER			160
#define CALENDAR_SCALE				10
#define CALENDAR_BACKCOLOR			BLACK
#define CALENDAR_POINTCOLOR			YELLOW
#define CALENDAR_SCALECOLOR			WHITE
#define CALENDAR_CIRCLECOLOR		GREEN
#define CALENDAR_NUMCOLOR			RED

u8 rome[]={12,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } ; //表盘数字

//画圆形指针表盘
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;//得到半径 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1; 
	u16 i;
	u16 fcolor;

	fcolor=FRONT_COLOR;
	FRONT_COLOR=CALENDAR_CIRCLECOLOR;
	LCD_Draw_Circle(x,y,r);		//画外圈
	LCD_Draw_Circle(x,y,r-1);		//画内圈

	for(i=0;i<60;i++)//画秒钟格
	{ 
		px0=sx+r+(r-4)*sin((app_pi/30)*i); 
		py0=sy+r-(r-4)*cos((app_pi/30)*i); 
		px1=sx+r+(r-d)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d)*cos((app_pi/30)*i);  
		gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_SCALECOLOR);		
	}
	for(i=0;i<12;i++)//画小时格
	{ 
		px0=sx+r+(r-5)*sin((app_pi/6)*i); 
		py0=sy+r-(r-5)*cos((app_pi/6)*i); 
		px1=sx+r+(r-d)*sin((app_pi/6)*i); 
		py1=sy+r-(r-d)*cos((app_pi/6)*i);  
		gui_draw_bline1(px0,py0,px1,py1,1,CALENDAR_SCALECOLOR);		
	}
	for(i=0;i<4;i++)//画3小时格
	{ 
		px0=sx+r+(r-5)*sin((app_pi/2)*i); 
		py0=sy+r-(r-5)*cos((app_pi/2)*i); 
		px1=sx+r+(r-d-3)*sin((app_pi/2)*i); 
		py1=sy+r-(r-d-3)*cos((app_pi/2)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,CALENDAR_SCALECOLOR);		
	}
	for(i=0;i<60;i++)//显示数字
	{  
		px1=sx+r+(r-d-10)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d-10)*cos((app_pi/30)*i);  
		if(i%5==0)
		{
			FRONT_COLOR=CALENDAR_NUMCOLOR;
			LCD_ShowxNum(px1-8,py1-5,rome[i/5],2,12,1);	
		}		
	}
	gui_fill_circle(x,y,d/2,CALENDAR_CIRCLECOLOR);		//画中心圈
	FRONT_COLOR=fcolor;
}

//显示时间
//x,y:坐标中心点
//size:表盘大小(直径)
//d:表盘分割,秒钟的高度
//hour:时钟
//min:分钟
//sec:秒钟
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec)
{
	static u8 oldhour=0;	//最近一次进入该函数的时分秒信息
	static u8 oldmin=0;
	static u8 oldsec=0;
	float temp;
	u16 r=size/2;//得到半径 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1;  
	u8 r1; 
	if(hour>11)hour-=12;
/////////////////////////////////////////////
	//清除秒钟 
	r1=d/2+3;  //3
	//清除上一次的数据
	px0=sx+r+(r-2*d-7)*sin((app_pi/30)*oldsec); 
	py0=sy+r-(r-2*d-7)*cos((app_pi/30)*oldsec); 
	px1=sx+r+r1*sin((app_pi/30)*oldsec); 
	py1=sy+r-r1*cos((app_pi/30)*oldsec); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_BACKCOLOR);	

	//清除分钟
	r1=d/2+3;	//3
	temp=(float)oldsec/60;
	temp+=oldmin;
	//清除上一次的数据
	px0=sx+r+(r-3*d-7)*sin((app_pi/30)*temp); 
	py0=sy+r-(r-3*d-7)*cos((app_pi/30)*temp); 
	px1=sx+r+r1*sin((app_pi/30)*temp); 
	py1=sy+r-r1*cos((app_pi/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_BACKCOLOR);		

	//清除小时
	r1=d/2+4;
	//清除上一次的数据
	temp=(float)oldmin/60;
	temp+=oldhour;
	px0=sx+r+(r-4*d-7)*sin((app_pi/6)*temp); 
	py0=sy+r-(r-4*d-7)*cos((app_pi/6)*temp); 
	px1=sx+r+r1*sin((app_pi/6)*temp); 
	py1=sy+r-r1*cos((app_pi/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_BACKCOLOR);	
		
	
///////////////////////////////////////////////
	//显示秒钟 
	r1=d/2+3;
	//显示新的秒钟
	px0=sx+r+(r-2*d-7)*sin((app_pi/30)*sec); 
	py0=sy+r-(r-2*d-7)*cos((app_pi/30)*sec); 
	px1=sx+r+r1*sin((app_pi/30)*sec); 
	py1=sy+r-r1*cos((app_pi/30)*sec); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_POINTCOLOR);
	
	
	//显示分钟
	r1=d/2+3; 
	temp=(float)sec/60;
	temp+=min;
	//显示新的分钟
	px0=sx+r+(r-3*d-7)*sin((app_pi/30)*temp); 
	py0=sy+r-(r-3*d-7)*cos((app_pi/30)*temp); 
	px1=sx+r+r1*sin((app_pi/30)*temp); 
	py1=sy+r-r1*cos((app_pi/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_POINTCOLOR);		

	//显示小时 
	r1=d/2+4; 
	//显示新的时钟
	temp=(float)min/60;
	temp+=hour;
	px0=sx+r+(r-4*d-7)*sin((app_pi/6)*temp); 
	py0=sy+r-(r-4*d-7)*cos((app_pi/6)*temp); 
	px1=sx+r+r1*sin((app_pi/6)*temp); 
	py1=sy+r-r1*cos((app_pi/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,0,CALENDAR_POINTCOLOR);	
	 
	oldhour=hour;	//保存时
	oldmin=min;		//保存分
	oldsec=sec;		//保存秒
}	 

void calendar_circle_clock_shownum(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;//得到半径 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px1,py1; 
	u8 i;
	u16	fcolor;
	fcolor=FRONT_COLOR;
	for(i=0;i<60;i++)//显示数字
	{  
		px1=sx+r+(r-d-10)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d-10)*cos((app_pi/30)*i);  
		if(i%5==0)
		{
			FRONT_COLOR=CALENDAR_NUMCOLOR;
			LCD_ShowxNum(px1-8,py1-5,rome[i/5],2,12,1);	
		}		
	}
	FRONT_COLOR=fcolor;	
}

//多功能数字时钟测试
void MultiFunction_Digital_Clock_Test(void)
{

	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"Digital Clock Test");

   	calendar_circle_clock_drawpanel(CALENDAR_CENTER_X,CALENDAR_CENTER_Y,CALENDAR_DIAMETER,CALENDAR_SCALE);

	Timer0Init();
	 
	while(1)
	{
		if(t_secflag)
		{
			t_secflag=0;
			calendar_circle_clock_showtime(CALENDAR_CENTER_X,CALENDAR_CENTER_Y,CALENDAR_DIAMETER,CALENDAR_SCALE,t_hour,t_min,t_sec);
		}
	}	
}

void main()
{	

	UART_Init();
	TFTLCD_Init();

	MultiFunction_Digital_Clock_Test();
	while(1)
	{
		
				
	}		
}
