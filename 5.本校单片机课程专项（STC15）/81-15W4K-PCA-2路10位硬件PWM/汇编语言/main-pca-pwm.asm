
;/*------------------------------------------------------------------*/
;/* --- STC MCU International Limited -------------------------------*/
;/* --- STC 1T Series MCU RC Demo -----------------------------------*/
;/* --- Mobile: (86)13922805190 -------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
;/* --- Web: www.GXWMCU.com -----------------------------------------*/
;/* --- QQ:  800003751 ----------------------------------------------*/
;/* If you want to use the program or the program referenced in the  */
;/* article, please specify in which data and procedures from STC    */
;/*------------------------------------------------------------------*/

;/*************	功能说明	**************

;STC15W4K60S4 2路10位PWM基本应用.

;PWM0  为10位PWM.
;PWM1  为10位PWM.

;******************************************/

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

PCA0				EQU	0
PCA1				EQU	1
PCA_Counter			EQU	3
PCA_P12_P11_P10		EQU	(0 SHL 4)
PCA_P34_P35_P36		EQU	(1 SHL 4)
PCA_P24_P25_P26		EQU	(2 SHL 4)
PCA_Mode_PWM		EQU	0x42
PCA_Mode_Capture	EQU	0
PCA_Mode_SoftTimer	EQU	0x48
PCA_Mode_HighPulseOutput	EQU		0x4c
PCA_Clock_1T		EQU	(4 SHL 1)
PCA_Clock_2T		EQU	(1 SHL 1)
PCA_Clock_4T		EQU	(5 SHL 1)
PCA_Clock_6T		EQU	(6 SHL 1)
PCA_Clock_8T		EQU	(7 SHL 1)
PCA_Clock_12T		EQU	(0 SHL 1)
PCA_Clock_Timer0_OF	EQU	(2 SHL 1)
PCA_Clock_ECI		EQU	(3 SHL 1)
PCA_Rise_Active		EQU	(1 SHL 5)
PCA_Fall_Active		EQU	(1 SHL 4)
PCA_PWM_8bit		EQU	(0 SHL 6)
PCA_PWM_7bit		EQU	(1 SHL 6)
PCA_PWM_6bit		EQU	(2 SHL 6)
PCA_PWM_10bit		EQU	(3 SHL 6)


PCA_PWM0 DATA 0F2H	;	//PCA模块0 PWM寄存器。
PCA_PWM1 DATA 0F3H	;	//PCA模块1 PWM寄存器。

AUXR1  DATA 0xA2
CCON   DATA 0xD8
CMOD   DATA 0xD9
CCAPM0 DATA 0xDA	; PCA模块0的工作模式寄存器。
CCAPM1 DATA 0xDB	; PCA模块1的工作模式寄存器。

CL     DATA 0xE9
CCAP0L DATA 0xEA	; PCA模块0的捕捉/比较寄存器低8位。
CCAP1L DATA 0xEB	; PCA模块1的捕捉/比较寄存器低8位。

CH     DATA 0xF9
CCAP0H DATA 0xFA	; PCA模块0的捕捉/比较寄存器高8位。
CCAP1H DATA 0xFB	; PCA模块1的捕捉/比较寄存器高8位。

CCF0  BIT CCON.0	; PCA 模块0中断标志，由硬件置位，必须由软件清0。
CCF1  BIT CCON.1	; PCA 模块1中断标志，由硬件置位，必须由软件清0。
CR    BIT CCON.6	; 1: 允许PCA计数器计数，0: 禁止计数。
CF    BIT CCON.7	; PCA计数器溢出（CH，CL由FFFFH变为0000H）标志。PCA计数器溢出后由硬件置位，必须由软件清0。
PPCA  BIT IP.7		; PCA 中断 优先级设定位

P1M1		DATA	091H	;	P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull
P1M0		DATA	092H	;					=10--->pure input,	11--->open drain
P2M1		DATA	0x95;	
P2M0		DATA	0x96;	
P3M1		DATA	0xB1;	
P3M0		DATA	0xB2;	



;************************************************

		ORG		00H				;reset
		LJMP	F_Main


;******************** 主程序 **************************/
F_Main:
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================

	LCALL	F_PCA_config	; 初始化PCA

	MOV		R6, #HIGH 800	; PWM值高字节
	MOV		R7, #LOW  800	; PWM值低字节
	MOV		A, #PCA0		; PCA 号
	LCALL	F_UpdatePwm		; 更新PWM值

	MOV		R6, #HIGH 400	; PWM值高字节
	MOV		R7, #LOW  400	; PWM值低字节
	MOV		A, #PCA1		; PCA 号
	LCALL	F_UpdatePwm		; 更新PWM值

;=================== 主循环 ==================================
L_Main_Loop:

	LJMP	L_Main_Loop
;===================================================================



;========================================================================
; 函数: F_PCA_config
; 描述: PCA初始化函数。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-12-15
; 备注: 
;========================================================================
F_PCA_config:

	CLR		CR;
	MOV		CH, #0;
	MOV		CL, #0;

	ANL		AUXR1, #NOT (3 SHL 4)

;	ORL		AUXR1, #PCA_P12_P11_P10	; 切换到P1.2 P1.1 P1.0 (ECI CCP0 CCP1)
;	ANL		P1M1, #NOT 0x07			; 把P1.2 P1.1 P1.0 (ECI CCP0 CCP1) 设置为准双双向口(P1.1 P1.0也可以设置为推挽输出)
;	ANL		P1M0, #NOT 0x07

	ORL		AUXR1, #PCA_P24_P25_P26	; 切换到P2.4 P2.5 P2.6 (ECI CCP0 CCP1)
	ANL		P2M1, #NOT 0x70			; 把P2.4 P2.5 P2.6 (ECI CCP0 CCP1) 设置为准双双向口(P2.5 P2.6也可以设置为推挽输出)
	ANL		P2M0, #NOT 0x70

;	ORL		AUXR1, #PCA_P34_P35_P36	; 切换到P3.4 P3.5 P3.6 (ECI CCP0 CCP1)
;	ANL		P3M1, #NOT 0x70			; P3.4 P3.5 P3.6 (ECI CCP0 CCP1) 设置为准双双向口(P3.5 P3.6也可以设置为推挽输出)
;	ANL		P3M0, #NOT 0x70
	
	ANL		CMOD, #NOT (7 SHL 1)
	ORL		CMOD, #PCA_Clock_1T		; 选择时钟源
	ANL		CMOD, #NOT 1			; 禁止溢出中断
;	SETB	PPCA					; 高优先级中断

	MOV		CCAPM0, #PCA_Mode_PWM	; 工作模式
	ANL		PCA_PWM0, #NOT (3 SHL 6)
	ORL		PCA_PWM0, #PCA_PWM_10bit	; PWM宽度
	MOV		CCAP0L, #0xff;
	MOV		CCAP0H, #0xff;

	MOV		CCAPM1, #PCA_Mode_PWM	; 工作模式
	ANL		PCA_PWM1, #NOT (3 SHL 6)
	ORL		PCA_PWM1, #PCA_PWM_10bit; PWM宽度
	MOV		CCAP1L, #0xff;
	MOV		CCAP1H, #0xff;
	SETB	CR
	RET


;========================================================================
; 函数: F_UpdatePwm
; 描述: 更新PWM值. 
; 参数: ACC: 	PCA序号. 取值 PCA0,PCA1
;		R6, R7: pwm值, 0~1024, 这个值是输出高电平的时间, 0对应连续的低电平, 1024对应连续的高电平.
; 返回: none.
; 版本: V1.0, 2012-11-22
;========================================================================
F_UpdatePwm:
	PUSH	ACC				; ID入栈
	CLR		F0				; 先判断是否 0, F0 = 0, 表示为0
	MOV		A, R6
	ORL		A, R7
	JZ		L_UpdatePWM_Is0
	SETB	F0		; F0=0, 表示非0
	MOV		A, R7	; pwm_value = ~(pwm_value-1) & 0x3ff;
	CLR		C
	SUBB	A, #1
	CPL		A
	MOV		R7, A
	MOV		A, R6
	SUBB	A, #0
	CPL		A
	SWAP	A
	ANL		A, #0x30
	MOV		R6, A
L_UpdatePWM_Is0:

	POP		ACC		; 恢复ID
	CJNE	A, #PCA0, L_NotUpdatePCA0
	JB		F0, L_UpdatePCA0_Not0
	ORL		PCA_PWM0, #0x32		; if(pwm_value == 0)
	MOV		CCAP0H, #0xff
	RET

L_UpdatePCA0_Not0:
	MOV		A, PCA_PWM0		; PCA_PWM0 = (PCA_PWM0 & ~0x32) | ((u8)(pwm_value >> 4) & 0x30);
	ANL		A, #NOT 0x32
	ORL		A, R6
	MOV		PCA_PWM0, A
	MOV		CCAP0H, R7		; CCAP0H = (u8)pwm_value;
	RET

L_NotUpdatePCA0:
	CJNE	A, #PCA1, L_NotUpdatePCA1
	JB		F0, L_UpdatePCA1_Not0
	ORL		PCA_PWM1, #0x32		; if(pwm_value == 0)
	MOV		CCAP1H, #0xff
	RET

L_UpdatePCA1_Not0:
	MOV		A, PCA_PWM1		; PCA_PWM0 = (PCA_PWM0 & ~0x32) | ((u8)(pwm_value >> 4) & 0x30);
	ANL		A, #NOT 0x32
	ORL		A, R6
	MOV		PCA_PWM1, A
	MOV		CCAP1H, R7		; CCAP0H = (u8)pwm_value;
L_NotUpdatePCA1:
	RET

	END
	

