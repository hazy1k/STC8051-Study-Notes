#ifndef _time_H
#define _time_H

#include "public.h"


//变量声明
extern u8 gmsec;//ms级定义
extern u8 gsec;//秒定义
extern u8 gmin;//分定义


//函数声明
void time0_init(void);
void time0_start(void);
void time0_stop(void);
#endif
