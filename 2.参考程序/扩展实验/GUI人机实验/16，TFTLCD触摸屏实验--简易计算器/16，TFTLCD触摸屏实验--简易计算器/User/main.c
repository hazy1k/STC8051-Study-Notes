/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中																				  
***************************************************************************************/
/**  
注意：涉及到串口通信的请将开发板上12M晶振换成11.0592MHZ。
*/

#include "public.h"  
#include "uart.h"
#include "tftlcd.h"
#include "touch.h"
#include "gui.h"
#include "stdlib.h"

sbit LED1=P3^6;

#define MULTIEDIT_START_X		10	  		//起始X坐标
#define MULTIEDIT_START_Y		20			//起始Y坐标
#define MULTIEDIT_WIDTH			150			//长
#define MULTIEDIT_HEIGHT		20			//高

#define MULTIEDIT1_START_X		MULTIEDIT_START_X	  		//起始X坐标
#define MULTIEDIT1_START_Y		MULTIEDIT_START_Y+MULTIEDIT_HEIGHT+1			//起始Y坐标

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






#define BUTTON_START_X		MULTIEDIT_START_X	  		//起始X坐标
#define BUTTON_START_Y		MULTIEDIT1_START_Y+MULTIEDIT_HEIGHT+5			//起始Y坐标
#define BUTTON_WIDTH		30			//长
#define BUTTON_HEIGHT		30			//高
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




const u8 Button_text[][2]={"1","2","3","+",
						   "4","5","6","-",
						   "7","8","9","*",
						   "0","=","C","/"}; //按钮显示文本

#define ADD		1  //加
#define MINUS	2  //减
#define RIDE	3  //乘
#define EXCEPT	4  //除
#define EQUAL	5  //等于
#define CLEAR	6  //清除

//简易计算器测试
void Calculator_Test(void)
{
	u8 i=0;
	u8 j=0;
	u8 Button_PressFlag=0;
	u8 cx,cy;
	u8 total_cnt=0;
	u8 dat1_numstr[6];
	u8 dat1_cnt=0;
	u8 dat2_numstr[6];
	u8 dat2_cnt=0;
	u8 fuhao_value=0;
	u8 fuhao_flag=0;
	int dat1_num=0;
	int dat2_num=0;
	int result=0;
	char result_buf[15];
	
	FRONT_COLOR=RED;
	LCD_ShowString(10,0,tftlcd_data.width,tftlcd_data.height,16,"Calculator Test");
   	MultiEdit_Create(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,MULTIEDIT_BACKCOLOR); 
	MultiEdit_ShowString(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,2,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE," ");
	MultiEdit_Create(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,MULTIEDIT_BACKCOLOR); 
	MultiEdit_ShowString(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,2,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,"0");
	
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			Button_Create(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*j,BUTTON_START_Y+(BUTTON_SPACE_Y+BUTTON_HEIGHT)*i,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[4*i+j]);	
		}	
	}
	
	while(1)
	{
		TOUCH_Scan();
		if(xpt_xy.sta)
		{
			for(i=0;i<4;i++)   //扫描按钮，获取对应的位置信息
			{
				for(j=0;j<4;j++)
				{
					if((xpt_xy.lcdx>=BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*j) && 
						(xpt_xy.lcdx<=BUTTON_START_X+BUTTON_WIDTH+(BUTTON_SPACE_X+BUTTON_WIDTH)*j)
						 && (xpt_xy.lcdy>=BUTTON_START_Y+(BUTTON_SPACE_Y+BUTTON_HEIGHT)*i) && 
						(xpt_xy.lcdy<=BUTTON_START_Y+BUTTON_HEIGHT+(BUTTON_SPACE_Y+BUTTON_HEIGHT)*i))
					{
						cx=j;
						cy=i;
						Button_PressFlag=1;			
					}
				}	
			}
			//确认是否是数字键按下，并且是第1个运算数，将按下数值组合到字符数组dat1_numstr中
			if((dat1_cnt<6) && (!fuhao_flag))
			{
				if(!strcmp(Button_text[4*cy+cx],"1") || !strcmp(Button_text[4*cy+cx],"2") || 
					!strcmp(Button_text[4*cy+cx],"3") || !strcmp(Button_text[4*cy+cx],"4") || 
					!strcmp(Button_text[4*cy+cx],"5") || !strcmp(Button_text[4*cy+cx],"6") || 
					!strcmp(Button_text[4*cy+cx],"7") || !strcmp(Button_text[4*cy+cx],"8") ||
					!strcmp(Button_text[4*cy+cx],"9") || !strcmp(Button_text[4*cy+cx],"0") )
				{									
					dat1_numstr[dat1_cnt++]=atoi(Button_text[4*cy+cx])+0x30;
					printf("dat1_numstr[dat1_cnt-1]=%c\r\n",dat1_numstr[dat1_cnt-1]);
				}	
			}
			//确认是否是数字键按下，并且是第2个运算数，将按下数值组合到字符数组dat2_numstr中
			else if((dat2_cnt<6) && (fuhao_flag))
			{
				if(!strcmp(Button_text[4*cy+cx],"1") || !strcmp(Button_text[4*cy+cx],"2") || 
					!strcmp(Button_text[4*cy+cx],"3") || !strcmp(Button_text[4*cy+cx],"4") || 
					!strcmp(Button_text[4*cy+cx],"5") || !strcmp(Button_text[4*cy+cx],"6") || 
					!strcmp(Button_text[4*cy+cx],"7") || !strcmp(Button_text[4*cy+cx],"8") ||
					!strcmp(Button_text[4*cy+cx],"9") || !strcmp(Button_text[4*cy+cx],"0") )
				{									
					dat2_numstr[dat2_cnt++]=atoi(Button_text[4*cy+cx])+0x30;
					printf("dat2_numstr[dat2_cnt-1]=%c\r\n",dat2_numstr[dat2_cnt-1]);
				}	
			}
			//判断是否按下运算符号，并记录运算符
			if(!strcmp(Button_text[4*cy+cx],"+") || !strcmp(Button_text[4*cy+cx],"-") ||
				!strcmp(Button_text[4*cy+cx],"*") || !strcmp(Button_text[4*cy+cx],"/"))
			{
				fuhao_flag=1;//标记有运算符按下
				dat1_num=atoi(dat1_numstr);//将获取的第1个运算数字符串转换为数值，等待参与运算
				printf("dat1_num=%d\r\n",dat1_num);
				if(!strcmp(Button_text[4*cy+cx],"+"))
					fuhao_value=ADD;
				else if(!strcmp(Button_text[4*cy+cx],"-"))
					fuhao_value=MINUS;
				else if(!strcmp(Button_text[4*cy+cx],"*"))
					fuhao_value=RIDE;
				else if(!strcmp(Button_text[4*cy+cx],"/"))
					fuhao_value=EXCEPT;
			}
			
			//将计算结果存储后再转换为字符串进行显示
			if(!strcmp(Button_text[4*cy+cx],"="))
			{
				dat2_num=atoi(dat2_numstr);//将获取的第2个运算数字符串转换为数值，等待参与运算	
				printf("dat2_num=%d\r\n",dat2_num);
				if(fuhao_value==ADD)//加法
				{
					result=dat1_num+dat2_num;		
				}
				else if(fuhao_value==MINUS)//减法
				{
					result=dat1_num-dat2_num;		
				}
				else if(fuhao_value==RIDE)//乘法
				{
					result=dat1_num*dat2_num;		
				}
				else if(fuhao_value==EXCEPT)//除法
				{
					result=dat1_num/dat2_num;		
				}
				printf("result=%d\r\n",result);
				gui_num2strex(result,result_buf);//将十进制运算结果转换为字符串进行显示
			  	printf("result_buf=%s\r\n",result_buf);
				MultiEdit_ShowString(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,2,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,result_buf);
				fuhao_value=EQUAL;
			}

			//将计算过程显示在第1个文本框中
			if(strcmp(Button_text[4*cy+cx],"C")!=0)
			{
				MultiEdit_ShowString(MULTIEDIT_START_X+(total_cnt*MULTIEDIT_FONTSIZE/2),MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,2,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,Button_text[4*cy+cx]);
				total_cnt++;	
			}
				
			if(!strcmp(Button_text[4*cy+cx],"C"))
			{
				cx=0;
				cy=0;
				fuhao_value=0;
				fuhao_flag=0;
				total_cnt=0;
				dat1_cnt=0;
				dat2_cnt=0;
				dat1_num=0;
				dat2_num=0;
				result=0;
				strcpy(dat1_numstr,"      ");
				strcpy(dat2_numstr,"      ");
				strcpy(result_buf,"      ");

				MultiEdit_ShowString(MULTIEDIT_START_X,MULTIEDIT_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,2,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,"                 ");
				MultiEdit_ShowString(MULTIEDIT1_START_X,MULTIEDIT1_START_Y,MULTIEDIT_WIDTH,MULTIEDIT_HEIGHT,5,2,MULTIEDIT_BACKCOLOR,MULTIEDIT_FRONTCOLOR,MULTIEDIT_FONTSIZE,result_buf);	
			}
	
		}
		else
		{
			if(Button_PressFlag)
			{
				Button_PressFlag=0;
				Button_Create(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*cx,BUTTON_START_Y+(BUTTON_SPACE_Y+BUTTON_HEIGHT)*cy,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[4*cy+cx]);
			}
		}
		if(Button_PressFlag)//发送数据
		{
			Button_PressCreate(BUTTON_START_X+(BUTTON_SPACE_X+BUTTON_WIDTH)*cx,BUTTON_START_Y+(BUTTON_SPACE_Y+BUTTON_HEIGHT)*cy,BUTTON_WIDTH,BUTTON_HEIGHT,BUTTON_ARCSIZE,BUTTON_BACKCOLOR,BUTTON_FRONTCOLOR,16,Button_text[4*cy+cx]);			
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

	Calculator_Test();
	while(1)
	{
		
				
	}		
}
