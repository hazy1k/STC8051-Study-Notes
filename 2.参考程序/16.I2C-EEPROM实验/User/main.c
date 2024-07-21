#include "public.h"
#include "24c02.h"
#include "key.h"
#include "smg.h"

#define EEPROM_ADDRESS	0 // 定义数据存入EEPROM的起始地址为0

void main()
{	
	unsigned char key_temp = 0; // 定义按键临时变量
   	unsigned char save_value=0; // 定义保存值变量
	unsigned char save_buf[3]; // 定临时数码管显示变量

	while(1)
	{			
		key_temp = key_scan(0); // key的值等于我们写的按键扫描函数返回值

		if(key_temp == KEY1_PRESS)
		{
			at24c02_write_one_byte(EEPROM_ADDRESS, save_value); // 按键1按下时写入数据到at24c02
		}
		else if(key_temp == KEY2_PRESS) // 按键2按下时读取数据
		{
			save_value = at24c02_read_one_byte(EEPROM_ADDRESS);
		}
		else if(key_temp == KEY3_PRESS) // 按键3按下时增加数据
		{
			save_value++; // 每按一次数据增加1
			if(save_value == 255) // 防止数据溢出
				save_value = 255;
		}
		else if(key_temp == KEY4_PRESS) // 按键4清除数据
		{
			save_value=0;	
		}

		// 数码管显示
		save_buf[0] = save_value/100;
		save_buf[1] = save_value%100/10;
		save_buf[2] = save_value%100%10;
		smg_display(save_buf,6);
	}		
}
