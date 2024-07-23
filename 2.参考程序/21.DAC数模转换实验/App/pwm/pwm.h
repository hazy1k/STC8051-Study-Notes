#ifndef _pwm_H
#define _pwm_H

#include "public.h"

//管脚定义
sbit PWM = P2^1;

//变量声明
extern unsigned char gtim_scale;

//函数声明
void pwm_init(unsigned char tim_h, unsigned char tim_l, unsigned int tim_scale, unsigned char duty);
void pwm_set_duty_cycle(unsigned char duty);

#endif
