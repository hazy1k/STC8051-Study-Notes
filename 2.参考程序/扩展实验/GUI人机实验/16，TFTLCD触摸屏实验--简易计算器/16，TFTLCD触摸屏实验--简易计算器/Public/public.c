#include "public.h"




void delay_10us(u16 us)
{
	while(us--);
}

void delay_us(u16 n)                  
{
    while (n--)                        //DJNZ  4
    {
        _nop_();                       //NOP  1+1=12
//        _nop_();   
//        _nop_();                       //NOP  1+1=12
//        _nop_();   
//        _nop_();                       //NOP  1+1=12
//        _nop_();   			
    }
}

void delay_ms(u16 ms)
{
	u16 i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

