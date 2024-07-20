#include <REGX52.H>

// 定义数码管段码口
#define SMG P0 

// 定义数码管位选信号控制脚
sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;

// 共阴极数码管显示 0~F 的段码数据
unsigned char gsmg_code[16] = 
{
    0x3f,  // 0
    0x06,  // 1
    0x5b,  // 2
    0x4f,  // 3
    0x66,  // 4
    0x6d,  // 5
    0x7d,  // 6
    0x07,  // 7
    0x7f,  // 8
    0x6f,  // 9
    0x77,  // A
    0x7c,  // B
    0x39,  // C
    0x5e,  // D
    0x79,  // E
    0x71   // F
};

// 延时函数，延时十微秒
void delay_us(unsigned int ten_us) 
{
    while(ten_us--);
} 

// 数码管显示函数
void smg_display() 
{
    unsigned int i = 0;
    for(i = 0; i < 8; i++) 
	{	
        // 设置位选信号
        switch(7 - i) 
		{
            case 0: 
                LSC = 1; LSB = 1; LSA = 1;
                break;
            case 1: 
                LSC = 1; LSB = 1; LSA = 0;
                break;
            case 2:	
                LSC = 1; LSB = 0; LSA = 1;
                break;
            case 3: 
                LSC = 1; LSB = 0; LSA = 0;
                break;
            case 4: 
                LSC = 0; LSB = 1; LSA = 1;
                break;
            case 5: 
                LSC = 0; LSB = 1; LSA = 0;
                break;
            case 6: 
                LSC = 0; LSB = 0; LSA = 1;
                break;
            case 7: 	
                LSC = 0; LSB = 0; LSA = 0;
                break;
        }
        
        // 传送段选数据并延时
        SMG = gsmg_code[i]; 
        delay_us(100); 
        // 消隐
        SMG = 0x00; 
    }
}

// 主函数
void main() 
{
    while(1) 
	{
        smg_display(); // 循环显示数码管
    }
}
