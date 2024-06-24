#include <reg51.h>

// 延时函数，us为延时时间（微秒）
void delay_us(unsigned int us)
{
    while(us--)
        ;
}

void main()
{
    unsigned char i;

    while(1)
    {
        for(i = 0; i < 8 ;i++)
        {
            P2 = ~(0x01 << i); // 按位取反实现流水灯
            delay_us(50000); // 延时操作
        }
    }
}
