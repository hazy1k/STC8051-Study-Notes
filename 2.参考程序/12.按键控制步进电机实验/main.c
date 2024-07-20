#include <REGX52.H>

//定义ULN2003控制步进电机管脚
sbit IN1_A = P1^0;
sbit IN2_B = P1^1;
sbit IN3_C = P1^2;
sbit IN4_D = P1^3;

//定义独立按键控制脚
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

// 定义步进电机速度，值越小，速度越快（1~5）
#define STEPMOTOR_MAXSPEED	1  
#define STEPMOTOR_MINSPEED	5  	

// 定义延时函数
void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

// 定义延时函数
void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

// 发送脉冲函数
void motor_28BYJ48_send(unsigned char step, unsigned char dir) // step:步进序号，dir:方向
{
	unsigned char temp = step; // 定义临时变量为步进序号
	
	if(dir == 0) // 如果为逆时针旋转
	{
		temp = 7 - step; // 调换节拍信号
	}

	switch(temp) // 8个节拍控制：A->AB->B->BC->C->CD->D->DA
	{
		// 这里我们直接复制粘贴就行
		case 0: 
			IN1_A=1;IN2_B=0;IN3_C=0;IN4_D=0;
			break;
		case 1: 
			IN1_A=1;IN2_B=1;IN3_C=0;IN4_D=0;
			break;
		case 2: 
			IN1_A=0;IN2_B=1;IN3_C=0;IN4_D=0;
			break;
		case 3: 
			IN1_A=0;IN2_B=1;IN3_C=1;IN4_D=0;
			break;
		case 4: 
			IN1_A=0;IN2_B=0;IN3_C=1;IN4_D=0;
			break;
		case 5: 
			IN1_A=0;IN2_B=0;IN3_C=1;IN4_D=1;
			break;
		case 6: 
			IN1_A=0;IN2_B=0;IN3_C=0;IN4_D=1;
			break;
		case 7: 
			IN1_A=1;IN2_B=0;IN3_C=0;IN4_D=1;
			break;
		default: 
			IN1_A=0;IN2_B=0;IN3_C=0;IN4_D=0;
			break; // 停止相序	
	}			
}

// 按键扫描函数（这个我们需要重点理解）
// mode: 0-单次扫描，1-连续扫描
unsigned char key_scan(int mode)
{
	static int key = 1; // 静态变量-目的是为了实现连续扫描按键,不会使key的初始值改变

	if(mode)
		key = 1; // 连续扫描按键
	if(key == 1 && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0|| KEY4 == 0)) // 代表任意按键按下
	{
		delay_10us(1000); // 消抖
		key = 0;
		if(KEY1 == 0) // 代表按键1按下
			return KEY1_PRESS;
		else if(KEY2 == 0) // 代表按键1按下
			return KEY2_PRESS;
		else if(KEY3 == 0) // 代表按键3按下
			return KEY3_PRESS;
		else if(KEY4 == 0) // 代表按键4按下
			return KEY4_PRESS;	
	}
	else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1) // 代表无按键按下
	{
		key = 1;			
	}
	return KEY_UNPRESS;		
}

void main()
{	
	int key = 0;
	int dir = 0; // 默认逆时针方向
	int speed = STEPMOTOR_MAXSPEED; // 默认最大速度旋转
	int step = 0;

	while(1)
	{			
		key = key_scan(0);
		if(key == KEY1_PRESS) // 换向
		{
			dir = !dir; // 方向反转    
		}
		else if(key == KEY2_PRESS) // 加速
		{
			// 若速度已经最大，此时加速会导致速度减1
			if(speed>STEPMOTOR_MAXSPEED)
				speed -= 1;			
		}
		else if(key == KEY3_PRESS)//减速
		{
			// 若速度已经最小，此时减速会导致速度加1
			if(speed < STEPMOTOR_MINSPEED)
				speed +=1;			
		}

		motor_28BYJ48_send(step++, dir); // 发送脉冲
		if(step == 8) // 步进序号到达8，回到0
		{
			step=0;	
		}	
		delay_ms(speed); // 延时					
	}		
}
// 总的来说，这个程序实现了按键控制步进电机的功能，通过按键控制方向、速度、步进序号