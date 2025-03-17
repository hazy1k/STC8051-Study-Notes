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

#ifndef	__LCD_IO16_H
#define	__LCD_IO16_H


void	LCD_load(u8 n,u8 dat);
void	LCD_scan(void);	//5us @22.1184MHZ
void	Init_LCD_Buffer(void);

extern	unsigned char	LCD_buff[8];
extern	unsigned char	scan_index;


#define DIS_BLACK	0x10
#define DIS_		0x11
#define DIS_A		0x0A
#define DIS_B		0x0B
#define DIS_C		0x0C
#define DIS_D		0x0D
#define DIS_E		0x0E
#define DIS_F		0x0F

#define LCD_SET_DP2		LCD_buff[0] |=  0x08
#define LCD_CLR_DP2		LCD_buff[0] &= ~0x08
#define LCD_FLASH_DP2	LCD_buff[0] ^=  0x08

#define LCD_SET_DP4		LCD_buff[4] |=  0x80
#define LCD_CLR_DP4		LCD_buff[4] &= ~0x80
#define LCD_FLASH_DP4	LCD_buff[4] ^=  0x80

#define LCD_SET_2M		LCD_buff[0] |=  0x20
#define LCD_CLR_2M		LCD_buff[0] &= ~0x20
#define LCD_FLASH_2M	LCD_buff[0] ^=  0x20

#define LCD_SET_4M		LCD_buff[0] |=  0x02
#define LCD_CLR_4M		LCD_buff[0] &= ~0x02
#define LCD_FLASH_4M	LCD_buff[0] ^=  0x02

#define LCD_SET_DP5		LCD_buff[4] |=  0x20
#define LCD_CLR_DP5		LCD_buff[4] &= ~0x20
#define LCD_FLASH_DP5	LCD_buff[4] ^=  0x20


#endif
