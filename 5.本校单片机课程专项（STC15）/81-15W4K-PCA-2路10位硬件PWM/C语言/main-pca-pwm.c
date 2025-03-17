
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

/*************	功能说明	**************

STC15W4K60S4 2路10位PWM基本应用.

PWM0  为10位PWM.
PWM1  为10位PWM.

******************************************/

#define MAIN_Fosc		24000000L	//定义主时钟

#include	"STC15Fxxxx.H"

#define	PCA0			0
#define	PCA1			1
#define	PCA_Counter		3
#define	PCA_P12_P11_P10	(0<<4)
#define	PCA_P34_P35_P36	(1<<4)
#define	PCA_P24_P25_P26	(2<<4)
#define	PCA_Mode_PWM				0x42	//B0100_0010
#define	PCA_Mode_Capture			0
#define	PCA_Mode_SoftTimer			0x48	//B0100_1000
#define	PCA_Mode_HighPulseOutput	0x4c	//B0100_1100
#define	PCA_Clock_1T	(4<<1)
#define	PCA_Clock_2T	(1<<1)
#define	PCA_Clock_4T	(5<<1)
#define	PCA_Clock_6T	(6<<1)
#define	PCA_Clock_8T	(7<<1)
#define	PCA_Clock_12T	(0<<1)
#define	PCA_Clock_Timer0_OF	(2<<1)
#define	PCA_Clock_ECI	(3<<1)
#define	PCA_Rise_Active	(1<<5)
#define	PCA_Fall_Active	(1<<4)
#define	PCA_PWM_8bit	(0<<6)
#define	PCA_PWM_7bit	(1<<6)
#define	PCA_PWM_6bit	(2<<6)
#define	PCA_PWM_10bit	(3<<6)

void	PCA_config(void);
void	UpdatePwm(u8 PCA_id, u16 pwm_value);


/************************************************/

/**********************************************/
void main(void)
{
	PCA_config();

//	EA = 1;

	UpdatePwm(PCA0,800);
	UpdatePwm(PCA1,400);
	
	while (1)
	{
	
	}
}

//========================================================================
// 函数: void	PCA_config(void)
// 描述: PCA初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-12-15
// 备注: 
//========================================================================
void	PCA_config(void)
{
	CR = 0;
	CH = 0;
	CL = 0;

//	AUXR1 = (AUXR1 & ~(3<<4)) | PCA_P12_P11_P10;	P1n_standard(0x07);	//切换到P1.2 P1.1 P1.0 (ECI CCP0 CCP1)
//	AUXR1 = (AUXR1 & ~(3<<4)) | PCA_P24_P25_P26;	P2n_standard(0x70);	//切换到P2.4 P2.5 P2.6 (ECI CCP0 CCP1)
	AUXR1 = (AUXR1 & ~(3<<4)) | PCA_P34_P35_P36;	P3n_standard(0x70);	//切换到P3.4 P3.5 P3.6 (ECI CCP0 CCP1)

	CMOD  = (CMOD  & ~(7<<1)) | PCA_Clock_1T;	//选择时钟源
	CMOD  &= ~1;								//禁止溢出中断
//	PPCA = 1;	//高优先级中断

	CCAPM0 = PCA_Mode_PWM;	//工作模式
	PCA_PWM0  = (PCA_PWM0 & ~(3<<6)) | PCA_PWM_10bit;	//PWM宽度
	CCAP0L = 0xff;
	CCAP0H = 0xff;

	CCAPM1 = PCA_Mode_PWM;	//工作模式
	PCA_PWM1  = (PCA_PWM1 & ~(3<<6)) | PCA_PWM_10bit;	//PWM宽度
	CCAP1L = 0xff;
	CCAP1H = 0xff;
	CR = 1;
}

//========================================================================
// 函数: UpdatePwm(u8 PCA_id, u16 pwm_value)
// 描述: 更新PWM值. 
// 参数: PCA_id: PCA序号. 取值 PCA0,PCA1
//		 pwm_value: pwm值, 0~1024, 这个值是输出高电平的时间, 0对应连续的低电平, 1024对应连续的高电平.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void	UpdatePwm(u8 PCA_id, u16 pwm_value)
{
	if(pwm_value > 1024)	return;	//PWM值过大, 退出
	
	if(PCA_id == PCA0)
	{
		if(pwm_value == 0)
		{
			PCA_PWM0 |= 0x32;
			CCAP0H = 0xff;
		}
		else
		{
			pwm_value = ~(pwm_value-1) & 0x3ff;
			PCA_PWM0 = (PCA_PWM0 & ~0x32) | ((u8)(pwm_value >> 4) & 0x30);
			CCAP0H = (u8)pwm_value;
		}
	}
	else if(PCA_id == PCA1)
	{
		if(pwm_value == 0)
		{
			PCA_PWM1 |= 0x32;
			CCAP1H = 0xff;
		}
		else
		{
			pwm_value = ~(pwm_value-1) & 0x3ff;
			PCA_PWM1 = (PCA_PWM1 & ~0x32) | ((u8)(pwm_value >> 4) & 0x30);
			CCAP1H = (u8)pwm_value;
		}
	}
}


