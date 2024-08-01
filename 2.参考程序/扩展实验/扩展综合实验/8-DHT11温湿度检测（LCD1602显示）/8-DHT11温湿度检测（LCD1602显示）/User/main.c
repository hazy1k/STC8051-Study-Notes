/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net
PRECHIN
 普中
 
实验名称：DHT11温湿度检测（LCD1602显示）
接线说明：DHT11温湿度模块-->单片机IO
		  VCC-->5V
		  DATA-->P2.3
		  GND-->GND	
实验现象：下载程序后，LCD1602上显示DHT11温湿度传感器采集的温度和湿度值
注意事项：按照LCD1602实验章节插好LCD1602到开发板																				  
***************************************************************************************/
#include "public.h"
#include "lcd1602.h"
#include "dht11.h"

/*******************************************************************************
* 函 数 名       : main
* 函数功能		 : 主函数
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void main()
{	
	u8 temp=0,humi=0;
	u8 i=0;
	u8 temp_buf[3],humi_buf[3];

	lcd1602_init();
	while(DHT11_Init())	//检测DHT11是否存在
	{
		lcd1602_show_string(0,0,"Error");		
	}
	lcd1602_show_string(0,0,"Temp:   C");
	lcd1602_show_string(0,1,"Humi:   %RH");
	while(1)
	{
		i++;
		if(i%200==0)
		{
			DHT11_Read_Data(&temp,&humi);
			temp_buf[0]=temp/10+0x30;	
			temp_buf[1]=temp%10+0x30;
			temp_buf[2]='\0';
			lcd1602_show_string(6,0,temp_buf);

			humi_buf[0]=humi/10+0x30;	
			humi_buf[1]=humi%10+0x30;
			humi_buf[2]='\0';
			lcd1602_show_string(6,1,humi_buf);	
		}
			
		delay_ms(1);		
	}
}
