#ifndef _iic_H
#define _iic_H

#include "public.h"

//定义EEPROM控制脚
sbit IIC_SCL = P2^1; // SCL时钟线
sbit IIC_SDA = P2^0; // SDA数据线


//IIC所有操作函数				 
void iic_start(void);			//发送IIC开始信号
void iic_stop(void);	  		//发送IIC停止信号
void iic_write_byte(u8 txd);	//IIC发送一个字节
u8 iic_read_byte(u8 ack);		//IIC读取一个字节
u8 iic_wait_ack(void); 			//IIC等待ACK信号
void iic_ack(void);				//IIC发送ACK信号
void iic_nack(void);			//IIC不发送ACK信号

#endif
