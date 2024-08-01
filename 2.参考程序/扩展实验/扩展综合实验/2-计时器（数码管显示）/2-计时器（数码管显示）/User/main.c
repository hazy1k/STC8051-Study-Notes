/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 
实验名称：计时器（数码管显示）
接线说明：	
实验现象：下载程序后，按下K1键开始计时，再次按下K1键停止计时，按下K2键清零
注意事项：																				  
***************************************************************************************/
#include "public.h"
#include "key.h"
#include "beep.h"
#include "smg.h"
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
	u8 time_buf[8];
	u8 time_flag=0;

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
	
		time_buf[0]=gsmg_code[gmin/10];
		time_buf[1]=gsmg_code[gmin%10];//分
		time_buf[2]=0x40;//-
		time_buf[3]=gsmg_code[gsec/10];
		time_buf[4]=gsmg_code[gsec%10];//秒
		time_buf[5]=0x40;//-
		time_buf[6]=gsmg_code[gmsec/10];
		time_buf[7]=gsmg_code[gmsec%10];//毫秒
		smg_display(time_buf,1);//显示	
	}
}
