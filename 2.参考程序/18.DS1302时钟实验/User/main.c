#include "public.h"
#include "smg.h"
#include "ds1302.h"

void main()
{	
	unsigned char time_buf[8];
	
	ds1302_init(); // 初始化DS1302

	while(1)
	{				
		ds1302_read_time(); // 写入数据

		time_buf[0] = gsmg_code[gDS1302_TIME[2]/16];
		time_buf[1] = gsmg_code[gDS1302_TIME[2]&0x0f];
		time_buf[2] = 0x40;
		time_buf[3] = gsmg_code[gDS1302_TIME[1]/16];
		time_buf[4] = gsmg_code[gDS1302_TIME[1]&0x0f];
		time_buf[5] = 0x40;
		time_buf[6] = gsmg_code[gDS1302_TIME[0]/16];
		time_buf[7] = gsmg_code[gDS1302_TIME[0]&0x0f];

		smg_display(time_buf,1);
	}		
}
