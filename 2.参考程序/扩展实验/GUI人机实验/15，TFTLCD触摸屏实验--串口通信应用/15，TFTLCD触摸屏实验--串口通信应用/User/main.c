/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中																				  
***************************************************************************************/
/**  
注意：涉及到串口通信的请将开发板上12M晶振换成11.0592MHZ。
	由于在串口初始化函数内将TI=1注释掉，所以本实验不能使用printf函数，故在tftlcd.c内
	TFTLCD初始化函数内将ID打印的printf注释掉了。
	如果要使用printf就需打开TI=1，但是当触摸按钮按下后，TFTLCD第一次刷新会很慢，
	所以这是个问题。
*/

#include "public.h"  
#include "uart.h"
#include "tftlcd.h"
#include "touch.h"
#include "gui.h"


sbit LED1=P3^6;

#define MULTIEDIT_START_X		10	  		//起始X坐标
#define MULTIEDIT_START_Y		40			//起始Y坐标
#define MULTIEDIT_WIDTH			150			//长
#define MULTIEDIT_HEIGHT		40			//高

#define MULTIEDIT1_START_X		10	  		//起始X坐标
#define MULTIEDIT1_START_Y		HEIGHT-MULTIEDIT1_HEIGHT-10			//起始Y坐标
#define MULTIEDIT1_WIDTH		150			//长
#define MULTIEDIT1_HEIGHT		60			//高

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






#define BUTTON_START_X		(MULTIEDIT_START_X+MULTIEDIT_WIDTH)/2-BUTTON_WIDTH	  		//起始X坐标
#define BUTTON_START_Y		MULTIEDIT_START_Y+MULTIEDIT_HEIGHT+5			//起始Y坐标
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


//按键触摸比较值
#define CONTROL_START_X		BUTTON_START_X
#define CONTROL_START_Y		BUTTON_START_Y
#define CONTROL_END_X		BUTTON_START_X+BUTTON_WIDTH
#define CONTROL_END_Y		BUTTON_START_Y+BUTTON_HEIGHT


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




const u8 Button_text[]={"Send"}; //按钮显示文本
const u8 Send_testbuf[]={"Hello everyone,I am MuYi"};//发送内容
const u8 rec_clearbuf[]={"                                 "};//接收区清屏

//串口通信应用测试
void UART_Test(void)
{
	u8 i=0;
	u8 Button_PressFlag=0;
	u8 rec_len=0;
	
	FRONT_COLOR=RED;
	LCD_ShowString(10,0,tftlcd_data.width,tftlcd_data.height,16,"UART Test");
	FRONT_COLOR=GREEN;
	LCD_ShowString(10,20,tftlcd_data.width,tftlcd_data.height,16,"Send Message:");
   	MultiEdit_Create(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,MULTIEDIT_BACKCOLOR); 
	MultiEdit_ShowString(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,4,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,Send_testbuf);
	Button_Create(BUTTON_START_X,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text);  
	LCD_ShowString(10,BUTTON_START_Y+BUTTON_HEIGHT+5,tftlcd_data.width,tftlcd_data.height,16,"Receive Message:");
   	MultiEdit_Create(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT1_WIDTH,MULTIEDIT1_HEIGHT,MULTIEDIT_BACKCOLOR); 
	
	UART_Cmd(ENABLE);
	while(1)
	{
		TOUCH_Scan();
		if(xpt_xy.sta)
		{
			if((xpt_xy.lcdx>=CONTROL_START_X) && (xpt_xy.lcdx<=CONTROL_END_X) && 
				(xpt_xy.lcdy>=CONTROL_START_Y) && (xpt_xy.lcdy<=CONTROL_END_Y))
			{
				xpt_xy.lcdx=0;
				xpt_xy.lcdy=0;
				Button_PressFlag=1;
			}
		}
		else
		{
			if(Button_PressFlag)
			{
				Button_PressFlag=0;
				Button_Create(BUTTON_START_X,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text); 		  					
			}
		}
		if(Button_PressFlag)//发送数据
		{
			Button_PressCreate(BUTTON_START_X,BUTTON_START_Y,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text);			
			UART_SendString(Send_testbuf);
			UART_SendString("\r\n");
		}

		if((UART_RX_STA&0x8000))//接收到数据  上位机发送数据时要以换行结尾发送
		{
			MultiEdit_ShowString(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT1_WIDTH,MULTIEDIT1_HEIGHT,5,4,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,rec_clearbuf);//清除之前的显示内容
			rec_len=UART_RX_STA&0X3FFF;
			UART_RX_BUF[rec_len]='\0';
			MultiEdit_ShowString(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT1_WIDTH,MULTIEDIT1_HEIGHT,5,4,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,UART_RX_BUF);//显示新接收的内容			
			UART_RX_STA=0;
		}
		
		i++;
		if(i%20==0)
			LED1=!LED1;
		delay_ms(10);	
	}	
}

void main()
{	

	UART_Init();
	TFTLCD_Init();

	UART_Test();
	while(1)
	{
		
				
	}		
}
