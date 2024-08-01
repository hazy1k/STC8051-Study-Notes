/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 
实验名称：计时器（LCD1602显示）
接线说明：	
实验现象：下载程序后，按下K1键开始计时，再次按下K1键停止计时，按下K2键清零
注意事项：按照LCD1602实验章节插好LCD1602到开发板																				  
***************************************************************************************/
#include "public.h"
#include "key.h"
#include "beep.h"
#include "lcd1602.h"
#include "time.h"

/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	u8 key=0;
	u8 time_buf[9];
	u8 time_flag=0;

	lcd1602_init();
	time0_init();
	while(1)
	{
		key=key_scan(0);
		if(key==KEY1_PRESS)//开始和停止计时
		{
			time_flag=!time_flag;
			beep_alarm(1000,10);//短暂提示音
		}	
		else if(key==KEY2_PRESS)//清除计时
		{
			time0_stop();
			time_flag=0;
			gmin=0;
			gsec=0;
			gmsec=0;
			beep_alarm(1000,10);//短暂提示音		
		}
		if(time_flag)
			time0_start();//开始计时
		else
			time0_stop();//停止计时
	
		time_buf[0]=gmin/10+0x30;
		time_buf[1]=gmin%10+0x30;//分
		time_buf[2]='-';//-
		time_buf[3]=gsec/10+0x30;
		time_buf[4]=gsec%10+0x30;//秒
		time_buf[5]='-';//-
		time_buf[6]=gmsec/10+0x30;
		time_buf[7]=gmsec%10+0x30;//毫秒
		time_buf[8]='\0';
		lcd1602_show_string(0,0,time_buf);//显示	
	}
}
