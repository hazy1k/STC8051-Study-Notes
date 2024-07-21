#ifndef _ds18b20_H
#define _ds18b20_H

#include "public.h"

//管脚定义
sbit DS18B20_PORT = P3^7; // DS18B20数据口定义

//函数声明
unsigned char ds18b20_init(void);
float ds18b20_read_temperture(void);

#endif