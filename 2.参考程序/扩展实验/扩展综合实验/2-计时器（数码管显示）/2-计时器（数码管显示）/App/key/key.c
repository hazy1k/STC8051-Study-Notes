#include "key.h"

/*******************************************************************************
* 函 数 名       : key_scan
* 函数功能		 : 检测独立按键是否按下，按下则返回对应键值
* 输    入       : mode=0：单次扫描按键
				   mode=1：连续扫描按键
* 输    出    	 : KEY1_PRESS：K1按下
				   KEY2_PRESS：K2按下
				   KEY3_PRESS：K3按下
				   KEY4_PRESS：K4按下
				   KEY_UNPRESS：未有按键按下
*******************************************************************************/
u8 key_scan(u8 mode)
{
	static u8 key=1;

	if(mode)key=1;//连续扫描按键
	if(key==1&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))//任意按键按下
	{
		delay_10us(1000);//消抖
		key=0;
		if(KEY1==0)
			return KEY1_PRESS;
		else if(KEY2==0)
			return KEY2_PRESS;
		else if(KEY3==0)
			return KEY3_PRESS;
		else if(KEY4==0)
			return KEY4_PRESS;	
	}
	else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)	//无按键按下
	{
		key=1;			
	}
	return KEY_UNPRESS;		
}