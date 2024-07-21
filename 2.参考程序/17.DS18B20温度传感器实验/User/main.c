#include "public.h"
#include "smg.h"
#include "ds18b20.h"

void main()
{	
	unsigned char i = 0;
   	int temp_value;
	unsigned char temp_buf[5];

	ds18b20_init(); // 初始化DS18B20

	while(1)
	{				
		i++;
		if(i % 50 == 0) // 间隔一段时间读取温度值，间隔时间要大于温度传感器转换温度时间
			temp_value = ds18b20_read_temperture()*10; // 保留温度值小数后一位
		if(temp_value < 0) // 负温度
		{
			temp_value = -temp_value;
			temp_buf[0] = 0x40; // 显示负号	
		}
		else
			temp_buf[0]=0x00; // 不显示

		temp_buf[1] = gsmg_code[temp_value/1000]; // 百位
		temp_buf[2] = gsmg_code[temp_value%1000/100]; // 十位
		temp_buf[3] = gsmg_code[temp_value%1000%100/10]|0x80; // 个位+小数点
		temp_buf[4] = gsmg_code[temp_value%1000%100%10]; // 小数点后一位
		smg_display(temp_buf,4);
	}		
}