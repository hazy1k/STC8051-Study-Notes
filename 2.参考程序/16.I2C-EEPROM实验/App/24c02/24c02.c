#include "24c02.h"
#include "iic.h"

// AT24C02的写入数据的函数
void at24c02_write_one_byte(unsigned char addr, unsigned char dat)
{				   	  	    																 
    iic_start(); // iic开始信号 
	iic_write_byte(0XA0); // 发送写命令

	iic_wait_ack(); // iic等待应答				   
    iic_write_byte(addr); // 发送写地址

	iic_wait_ack(); 	 										  		   
	iic_write_byte(dat); // 发送字节

	iic_wait_ack();  		    	   
    iic_stop(); // 产生一个停止条件
	delay_ms(10);	 
}

// AT24C02的读取数据的函数
unsigned char at24c02_read_one_byte(unsigned char addr)
{				  
	unsigned char temp = 0; // 定义一个临时变量存储读取的数据		  

    iic_start(); // iic开始信号  
	iic_write_byte(0XA0); // 发送写命令

	iic_wait_ack(); // iic等待应答				   
    iic_write_byte(addr); // 发送写地址

	iic_wait_ack();	    
	iic_start();  	 	   
	iic_write_byte(0XA1); // 进入接收模式 

	iic_wait_ack();	 
    temp = iic_read_byte(0); // 读取字节

    iic_stop();	// 产生一个停止条件
	    
	return temp; // 返回读取的数据
}
