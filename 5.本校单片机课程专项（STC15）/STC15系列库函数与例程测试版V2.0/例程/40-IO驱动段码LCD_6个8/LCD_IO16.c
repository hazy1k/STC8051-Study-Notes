/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/

#include	"config.h"


/*************	功能说明	**************

用STC15系列测试IO直接驱动段码LCD(6个8字LCD, 1/4 Dutys, 1/3 bias)。

LCD_IO16.c

******************************************/

/*************	本地常量声明	**************/
u8 code t_display[]={						//标准字库
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -   
	0x00,0x40
};


/*************	本地变量声明	**************/
u8	LCD_buff[8];
u8	scan_index;



/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/


/****************** 初始化LCD显存函数 ***************************/

void	Init_LCD_Buffer(void)
{
	u8	i;

	for(i=0; i<8; i++)	LCD_buff[i] = 0;
}


/****************** LCD段码扫描函数 ***************************/
u8	code T_COM[4]={0x08,0x04,0x02,0x01};

void	LCD_scan(void)	//5us @22.1184MHZ
{
	u8	j;

	j = scan_index >> 1;
	P2n_pure_input(0x0f);		//全部COM输出高阻, COM为中点电压
	if(scan_index & 1)				//反相扫描
	{
		P1 = ~LCD_buff[j];			//
		P2 = ~(LCD_buff[j|4] & 0xf0);
	}
	else							//正相扫描
	{
		P1 = LCD_buff[j];
		P2 = LCD_buff[j|4] & 0xf0;
	}
	P2n_push_pull(T_COM[j]);		//某个COM设置为推挽输出
	if(++scan_index >= 8)	scan_index = 0;

}

/*
MCU PIN        P17   P16   P15  P14 P13  P12  P11  P10  P27  P26  P25  P24   P23  P22  P21  P20
LCD PIN         1     2     3    4   5    6    7    8    9    10   11   12    13   14   15   16
LCD PIN name  SEG11 SEG10 SEG9 SEG8 SEG7 SEG6 SEG5 SEG4 SEG3 SEG2 SEG1 SEG0  COM3 COM2 COM1 COM0
               --    1D    2:   2D   2.   3D   4:   4D   4.   5D   5.   6D   COM3
			   1E    1C    2E   2C   3E   3C   4E   4C   5E   5C   6E   6C        COM2
			   1G    1B    2G   2B   3G   3B   4G   4B   5G   5B   6G   6B             COM1
			   1F    1A    2F   2A   3F   3A   4F   4A   5F   5A   6F   6A                  COM0

			B7	B6	B5	B4	B3	B2	B1	B0

buff[0]:	--	1D	2:	2D	2.	3D	4:	4D
buff[1]:	1E	1C	2E	2C	3E	3C	4E	4C
buff[2]:	1G	1B	2G	2B	3G	3B	4G	4B
buff[3]:	1F	1A	2F	2A	3F	3A	4F	4A
buff[4]:	4.	5D	5.	6D	--	--	--	--
buff[5]:	5E	5C	6E	6C	--	--	--	--
buff[6]:	5G	5B	6G	6B	--	--	--	--
buff[7]:	5F	5A	6F	6A	--	--	--	--

*/

/****************** 对第2~5数字装载显示函数 ***************************/

u8	code T_LCD_mask[4]  = {~0xc0,~0x30,~0x0c,~0x03};
u8	code T_LCD_mask4[4] = {~0x40,~0x10,~0x04,~0x01};

void	LCD_load(u8 n,u8 dat)		//n为第几个数字，为1~6，dat为要显示的数字	10us@22.1184MHZ
{
	u8	i,k;
	u8	*p;

	if((n == 0) || (n > 6))	return;
	i = t_display[dat];

	if(n <= 4)	//1~4
	{
		n--;
		p = LCD_buff;
	}
	else
	{
		n = n - 5;
		p = &LCD_buff[4];
	}

	k = 0;
	if(i & 0x08)	k |= 0x40;		//D
	*p = (*p & T_LCD_mask4[n]) | (k>>2*n);
	p++;

	k = 0;
	if(i & 0x04)	k |= 0x40;		//C
	if(i & 0x10)	k |= 0x80;		//E
	*p = (*p & T_LCD_mask[n]) | (k>>2*n);
	p++;

	k = 0;
	if(i & 0x02)	k |= 0x40;		//B
	if(i & 0x40)	k |= 0x80;		//G
	*p = (*p & T_LCD_mask[n]) | (k>>2*n);
	p++;

	k = 0;
	if(i & 0x01)	k |= 0x40;		//A
	if(i & 0x20)	k |= 0x80;		//F
	*p = (*p & T_LCD_mask[n]) | (k>>2*n);
}



