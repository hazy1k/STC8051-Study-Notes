#ifndef __ds18b20_H_
#define __ds18b20_H_

#include "public.h"


//--定义使用的IO口--//
sbit DS18B20_DQ_OUT=P3^7;

//--声明全局函数--//
u8 DS18B20_Init(void);			//初始化DS18B20
float DS18B20_GetTemperture(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);		//读出一个字节
u8 DS18B20_Read_Bit(void);		//读出一个位
u8 DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Reset(void);			//复位DS18B20 

#endif
