#include "iic.h"

// iic起始信号
void iic_start(void)
{
	IIC_SDA = 1; // 如果把该条语句放在SCL后面，第二次读写会出现问题
	delay_10us(1);

	IIC_SCL = 1;
	delay_10us(1);

	IIC_SDA = 0; // 当SCL为高电平时，SDA由高变为低
	delay_10us(1);

	IIC_SCL = 0; // 钳住I2C总线，准备发送或接收数据
	delay_10us(1);
}

// iic停止信号
void iic_stop(void)
{	
	IIC_SDA = 0; // 如果把该条语句放在SCL后面，第二次读写会出现问题
	delay_10us(1);

	IIC_SCL = 1;
	delay_10us(1);

	IIC_SDA = 1; // 当SCL为高电平时，SDA由低变为高
	delay_10us(1);			
}

// iic产生ACK应答
void iic_ack(void)
{
	IIC_SCL = 0;
	IIC_SDA = 0; // SDA为低电平
	delay_10us(1);	

   	IIC_SCL = 1;
	delay_10us(1);
	IIC_SCL = 0;
}

// 产生NACK非应答  
void iic_nack(void)
{
	IIC_SCL = 0;
	IIC_SDA = 1; // SDA为高电平
	delay_10us(1);

   	IIC_SCL = 1;
	delay_10us(1);
	IIC_SCL = 0;	
}

// iic等待应答
unsigned char iic_wait_ack(void)
{
	unsigned char time_temp = 0;
	
	IIC_SCL = 1; // 拉高SCL，准备发送应答
	delay_10us(1);
	while(IIC_SDA) // 等待SDA为低电平
	{
		time_temp++;
		if(time_temp > 100) // 超时则强制结束IIC通信
		{	
			iic_stop();
			return 1; // 超时返回1-异常	
		}			
	}
	IIC_SCL = 0; // 拉低SCL，准备接收应答
	return 0; // 正常返回0
}

// iic发送一个字节
void iic_write_byte(unsigned char dat)
{                        
    unsigned char i = 0; 
	   	    
    IIC_SCL = 0;  // 将时钟线拉低，准备发送数据

    for(i = 0; i < 8; i++) // 循环8次将一个字节传出，先传高位再传低位
    {              
        if((dat & 0x80) > 0) // 检查dat的最高位是否为1
            IIC_SDA = 1;     // 如果最高位为1，则数据线拉高（发送逻辑1）
        else
            IIC_SDA = 0;     // 如果最高位为0，则数据线拉低（发送逻辑0）
        
        dat <<= 1;  // 将数据dat向左移动一位，准备发送下一个位
        delay_10us(1);  // 稍作延时，保证时序满足要求

        IIC_SCL = 1;  // 将时钟线拉高，通知接收方可以读取数据
        delay_10us(1);  // 稍作延时，保证时序满足要求

        IIC_SCL = 0;  // 将时钟线再次拉低，为发送下一位数据做准备
        delay_10us(1);  // 稍作延时，保证时序满足要求
    }	 
}


// IIC读一个字节 ack=1时，发送ACK，ack=0，发送nACK 
unsigned char iic_read_byte(unsigned char ack)
{
	unsigned char i = 0, receive = 0;
   	
    for(i =0; i < 8; i++ ) // 循环8次将一个字节读出，先读高再传低位
	{
        IIC_SCL = 0; 
        delay_10us(1);

		IIC_SCL = 1;
        receive <<= 1;

        if(IIC_SDA)
			receive++;   
		delay_10us(1); 
    }					 
    if (!ack)
        iic_nack(); // 发送nACK
    else
        iic_ack(); // 发送ACK
		  
    return receive; // 返回读出的字节
}
