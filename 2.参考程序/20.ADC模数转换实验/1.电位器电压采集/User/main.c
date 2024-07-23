#include "public.h"
#include "smg.h"
#include "xpt2046.h"

void main()
{	
	unsigned int adc_value = 0; // 初始化ADC值
	float adc_vol; // ADC电压值
	unsigned char adc_buf[3]; // 3位数码管显示

	while(1)
	{				
		adc_value = xpt2046_read_adc_value(0x94); // 测量电位器

		adc_vol = 5.0*adc_value/4096; // 将读取的AD值转换为电压
		adc_value = adc_vol*10; // 放大10倍，即保留小数点后一位
		
		adc_buf[0] = gsmg_code[adc_value/10]|0x80;
		adc_buf[1] = gsmg_code[adc_value%10];
	   	adc_buf[2] = 0x3e; // 显示单位V
		smg_display(adc_buf, 6);		
	}		
}
