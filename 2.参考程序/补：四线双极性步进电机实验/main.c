#include "reg52.h"

//定义TC1508S控制步进电机管脚
sbit IN_A = P1^0;
sbit IN_B = P1^1;
sbit IN_C = P1^2;
sbit IN_D = P1^3;

// 定义独立按键控制脚
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

//使用宏定义独立按键按下的键值
#define KEY1_PRESS	1
#define KEY2_PRESS	2
#define KEY3_PRESS	3
#define KEY4_PRESS	4
#define KEY_UNPRESS	0

// 定义步进电机速度，值越小，速度越快
// 最小不能小于1
#define STEPMOTOR_MAXSPEED	1  
#define STEPMOTOR_MINSPEED  5  	


void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

// 发送节拍信号
void step_motor_send_pulse(unsigned char step, unsigned char dir)
{
	unsigned char temp = step;
	
	if(dir == 0) // 如果为逆时针旋转
		temp = 7 - step; // 调换节拍信号

	switch(temp)//8个节拍控制：(A+)->(A+B+)->(B+)->(B+A-)->(A-)->(A-B-)->(B-)>(B-A+)
	{
		case 0: 
			IN_A=1;IN_B=0;IN_C=0;IN_D=0;
			break;
		case 1: 
			IN_A=1;IN_B=0;IN_C=1;IN_D=0;
			break;
		case 2: 
			IN_A=0;IN_B=0;IN_C=1;IN_D=0;
			break;
		case 3: 
			IN_A=0;IN_B=1;IN_C=1;IN_D=0;
			break;
		case 4: 
			IN_A=0;IN_B=1;IN_C=0;IN_D=0;
			break;
		case 5: 
			IN_A=0;IN_B=1;IN_C=0;IN_D=1;
			break;
		case 6: 
			IN_A=0;IN_B=0;IN_C=0;IN_D=1;
			break;
		case 7: 
			IN_A=1;IN_B=0;IN_C=0;IN_D=1;
			break;
		default:
			IN_A=0;IN_B=0;IN_C=0;IN_D=0;
			break; // 停止相序	
	}			
}

// 按键扫描
u8 key_scan(unsigned char mode)
{
	static unsigned char key = 1;

	if(mode)
		key = 1; // 连续扫描按键
	if(key == 1 && ( KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0)) // 任意按键按下
	{
		delay_10us(1000);//消抖
		key = 0;
		if(KEY1 == 0)
			return KEY1_PRESS;
		else if(KEY2 == 0)
			return KEY2_PRESS;
		else if(KEY3 == 0)
			return KEY3_PRESS;
		else if(KEY4 == 0)
			return KEY4_PRESS;	
	}
	else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1) // 无按键按下
	{
		key = 1;			
	}
	return KEY_UNPRESS;		
}

void main()
{	
	unsigned char key = 0;
	unsigned char dir = 0; // 默认逆时针方向
	unsigned char speed = STEPMOTOR_MAXSPEED; // 默认最大速度旋转
	unsigned char step = 0;

	while(1)
	{			
		key = key_scan(0);
		if(key == KEY1_PRESS) // 换向
		{
			dir =! dir;    
		}
		else if(key == KEY2_PRESS) // 加速
		{
			if(speed > STEPMOTOR_MAXSPEED)
				speed -= 1;			
		}
		else if(key == KEY3_PRESS) // 减速
		{
			if(speed < STEPMOTOR_MINSPEED)
				speed += 1;			
		}
		step_motor_send_pulse(step++, dir);
		if(step == 8)
			step = 0;		
		delay_ms(speed);						
	}		
}