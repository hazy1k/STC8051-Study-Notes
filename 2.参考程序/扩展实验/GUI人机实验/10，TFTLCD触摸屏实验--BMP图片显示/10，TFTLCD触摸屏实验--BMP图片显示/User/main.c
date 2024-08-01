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
#include "9dtest_56_pic.h"
#include "calc_56_pic.h"
#include "earthnet_56_pic.h"
#include "picture_56_pic.h"



#define PICTURE_START_X			30
#define PICTURE_START_Y			50
#define PICTURE_SPACE_X			10
#define PICTURE_SPACE_Y			10
#define PICTURE_WIDTH			56
#define PICTURE_HEIGHT			56


const u8 *pic_buf[]={gImage_9dtest_56_pic,gImage_calc_56_pic,gImage_earthnet_56_pic,gImage_picture_56_pic};

void main()
{
	u8 i=0;
	u8 j=0;

	UART_Init();
	TFTLCD_Init();

	FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"BMP Picture Test");

	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++)
		{
			LCD_ShowPicture(PICTURE_START_X+(PICTURE_WIDTH+PICTURE_SPACE_X)*j,
							PICTURE_START_Y+(PICTURE_HEIGHT+PICTURE_SPACE_Y)*i,
							PICTURE_WIDTH,PICTURE_HEIGHT,pic_buf[2*i+j]);	
		}		
	} 	
	
	while(1)
	{
					
	}					
}
