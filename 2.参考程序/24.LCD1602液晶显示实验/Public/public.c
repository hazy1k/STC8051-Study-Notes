#include "public.h"

void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

void delay_ms(unsigned int ms)
{
	unsigned int i,j;
	for(i = ms; i > 0; i--)
		for(j = 110; j > 0; j--);
}