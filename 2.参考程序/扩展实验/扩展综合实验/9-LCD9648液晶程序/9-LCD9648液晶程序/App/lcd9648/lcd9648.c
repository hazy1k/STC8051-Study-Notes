#include "lcd9648.h"
#include "font.h"


//全局变量定义
xdata u8 gdata_buf[96][6];

/*******************************************************************************
* 函 数 名       : lcd9648_spi_write_byte
* 函数功能		 : LCD9648 SPI传输数据
* 输    入       : dat：数据
* 输    出    	 : 无
*******************************************************************************/
void lcd9648_spi_write_byte(u8 dat)
{
	u8 i=0;

   	for(i=0;i<8;i++)//循环8次将一个字节传输			
   	{  
      	if((dat&0x80)!=0) 
			SDA = 1;
        else 
			SDA = 0;
      	dat<<= 1;//先写高位再写低位
	  	SCL = 0;
      	SCL = 1;//时钟上升沿写入数据			
   	}
}

/*******************************************************************************
* 函 数 名       : lcd9648_write_cmd
* 函数功能		 : LCD9648写命令
* 输    入       : cmd：命令
* 输    出    	 : 无
*******************************************************************************/
void lcd9648_write_cmd(u8 cmd)
{
	CS0 = 0;//使能
	RS  = 0;//选择命令
	lcd9648_spi_write_byte(cmd);//发送命令
	CS0 = 1;//失能	
}

/*******************************************************************************
* 函 数 名       : lcd9648_write_dat
* 函数功能		 : LCD9648写数据
* 输    入       : dat：数据
* 输    出    	 : 无
*******************************************************************************/
void lcd9648_write_dat(u8 dat)
{
	CS0 = 0;//使能
	RS  = 1;//选择数据
	lcd9648_spi_write_byte(dat);//发送数据
	CS0 = 1;//失能	
}

/*******************************************************************************
* 函 数 名       : lcd9648_init
* 函数功能		 : LCD9648初始化
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void lcd9648_init(void)
{
	RST=1;  
	delay_ms(10);
	RST=0;
	delay_ms(10);
	RST=1;
	delay_ms(10);//复位

    lcd9648_write_cmd(0xe2);		  //软件复位
	lcd9648_write_cmd(0xc8);		  //0xC8普通方向选择选择反向，0xC0为正常方向
	lcd9648_write_cmd(0xa0);		  //0xA0段方向选择正常方向（0xA1为反方向
	lcd9648_write_cmd(0x2f);
	lcd9648_write_cmd(0x26);
	lcd9648_write_cmd(0x81);		  //背景光对比度
	lcd9648_write_cmd(0x10);
	lcd9648_write_cmd(0xaf);		  //开启显示
	lcd9648_write_cmd(0x40);		  //设置滚动一行命令
}

/*******************************************************************************
* 函 数 名       : lcd9648_clear
* 函数功能		 : LCD9648清屏
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void lcd9648_clear(void)
{
	u8 i,j;

	for(i=0;i<9;i++)//一共9页
	{
//		lcd9648_write_cmd(0x40);//设置滚动一行命令
		lcd9648_write_cmd(0xb0+i);//页地址
		lcd9648_write_cmd(0x10);//列高字节地址
		lcd9648_write_cmd(0x00);//列低字节地址
		
		for(j = 0; j < LCD_WIDTH; j++)//每一行的像素点
		{
			lcd9648_write_dat(0x00);//不显示	
		}	
	}
	for(i=0;i<6;i++)  
	{    
		for(j=0;j<LCD_WIDTH;j++)
		{
			gdata_buf[j][i]=0;
		}
	}
	lcd_reflash_gram();//更新显示	
}

void lcd_reflash_gram(void)
{
	u8 i,n;		    
	for(i=0;i<6;i++)  
	{  
		lcd9648_write_cmd(0xb0+i);    //设置页地址（0~5）
		lcd9648_write_cmd(0x00);      //设置显示位置—列低地址
		lcd9648_write_cmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<LCD_WIDTH;n++)lcd9648_write_dat(gdata_buf[n][i]); 
	}
}

//画点
//x,y:坐标
//sta:1：显示，0：不显示
void lcd_draw_dot(u8 x,u8 y,u8 sta)
{
	u8 posy1,posy2;

	if(x>=LCD_WIDTH||y>=LCD_HEIGHT)return;//超出范围了
	posy1=y/8;//判断是第几页
	posy2=y%8;//判断当页的字节数中第几位（字节低位在上，高位在下显示）

	if(sta)
		gdata_buf[x][posy1]|=1<<posy2;
	else
		gdata_buf[x][posy1]&=~(1<<posy2);	
}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void lcd_show_char(u8 x,u8 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=ascii_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=ascii_1608[num][t];	//调用1608字体
		else if(size==24)temp=ascii_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)lcd_draw_dot(x,y,1);
			else if(mode==0)lcd_draw_dot(x,y,0);
			temp<<=1;
			y++;
			if(y>=LCD_HEIGHT)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=LCD_WIDTH)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n函数
//返回值:m^n次方.
u32 lcd_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			  

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void lcd_show_num(u8 x,u8 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/lcd_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)lcd_show_char(x+(size/2)*t,y,'0',size,mode&0X01);  
				else lcd_show_char(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	lcd_show_char(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void lcd_show_string(u8 x,u8 y,u8 width,u8 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        lcd_show_char(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

//显示汉字
//x,y:起点坐标
//*cn:字符串起始地址
void lcd_show_fontHZ(u8 x, u8 y, u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNum扫描字库的字数
			if ((CnChar16x16[wordNum].Index[0]==*cn)
			     &&(CnChar16x16[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<32; i++) 
				{	//MSK的位数
					color=CnChar16x16[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							lcd_draw_dot(x,y,1);
						} 						
						else
						{
							lcd_draw_dot(x,y,0);
						} 
						color<<=1;
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							if((y-y0)==16)
							{
								y=y0;
							}
						}
					}//for(j=0;j<8;j++)结束
				}	
			}
			
		} //for (wordNum=0; wordNum<20; wordNum++)结束 	
		cn += 2;
		x += 16;
		x0=x;
	}
}


