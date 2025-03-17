
/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-755-82905966 -------------------------------------------*/
/* --- Tel: 86-755-82948412 -------------------------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/



;*************	功能说明	**************

; 输出3路9~16位PWM信号。

; PWM频率 = MAIN_Fosc / PWM_DUTY, 假设 MAIN_Fosc = 24MHZ, PWM_DUTY = 6000, 则输出PWM频率为4000HZ.

; ******************************************

;***************************用户宏定义*******************************************************
Fosc_KHZ		EQU		24000	//定义主时钟, KHZ

PWM_DUTY		EQU		6000		//定义PWM的周期，数值为PCA所选择的时钟脉冲个数。
PWM_HIGH_MIN	EQU		80			//限制PWM输出的最小占空比, 避免中断里重装参数时间不够。
PWM_HIGH_MAX	EQU		(PWM_DUTY - PWM_HIGH_MIN)		//限制PWM输出的最大占空比。

;********************************************************************************************

PCA0					EQU		0
PCA1					EQU		1
PCA2					EQU		2
PCA_Counter				EQU		3
PCA_P12_P11_P10_P37		EQU		(0 SHL 4)
PCA_P34_P35_P36_P37		EQU		(1 SHL 4)
PCA_P24_P25_P26_P27		EQU		(2 SHL 4)
PCA_Mode_Capture		EQU		0
PCA_Mode_SoftTimer		EQU		048H
PCA_Mode_HighPulseOutput	EQU	04CH
PCA_Clock_1T			EQU		(4 SHL 1)
PCA_Clock_2T			EQU		(1 SHL 1)
PCA_Clock_4T			EQU		(5 SHL 1)
PCA_Clock_6T			EQU		(6 SHL 1)
PCA_Clock_8T			EQU		(7 SHL 1)
PCA_Clock_12T			EQU		(0 SHL 1)
PCA_Clock_ECI			EQU		(3 SHL 1)
PCA_Rise_Active			EQU		(1 SHL 5)
PCA_Fall_Active			EQU		(1 SHL 4)

ENABLE					EQU		1

AUXR1  DATA 0xA2
CCON   DATA 0xD8
CMOD   DATA 0xD9
CCAPM0 DATA 0xDA	; PCA模块0的工作模式寄存器。
CCAPM1 DATA 0xDB	; PCA模块1的工作模式寄存器。
CCAPM2 DATA 0xDC	; PCA模块2的工作模式寄存器。

CL     DATA 0xE9
CCAP0L DATA 0xEA	; PCA模块0的捕捉/比较寄存器低8位。
CCAP1L DATA 0xEB	; PCA模块1的捕捉/比较寄存器低8位。
CCAP2L DATA 0xEC	; PCA模块2的捕捉/比较寄存器低8位。

CH     DATA 0xF9
CCAP0H DATA 0xFA	; PCA模块0的捕捉/比较寄存器高8位。
CCAP1H DATA 0xFB	; PCA模块1的捕捉/比较寄存器高8位。
CCAP2H DATA 0xFC	; PCA模块2的捕捉/比较寄存器高8位。

CCF0  BIT CCON.0	; PCA 模块0中断标志，由硬件置位，必须由软件清0。
CCF1  BIT CCON.1	; PCA 模块1中断标志，由硬件置位，必须由软件清0。
CCF2  BIT CCON.2	; PCA 模块2中断标志，由硬件置位，必须由软件清0。
CR    BIT CCON.6	; 1: 允许PCA计数器计数，0: 禁止计数。
CF    BIT CCON.7	; PCA计数器溢出（CH，CL由FFFFH变为0000H）标志。PCA计数器溢出后由硬件置位，必须由软件清0。
PPCA  BIT IP.7		; PCA 中断 优先级设定位

P0M1	DATA	0x93	; P0M1.n,P0M0.n 	=00--->Standard,	01--->push-pull
P0M0	DATA	0x94	; 					=10--->pure input,	11--->open drain
P1M1	DATA	0x91	; P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull
P1M0	DATA	0x92	; 					=10--->pure input,	11--->open drain
P2M1	DATA	0x95	; P2M1.n,P2M0.n 	=00--->Standard,	01--->push-pull
P2M0	DATA	0x96	; 					=10--->pure input,	11--->open drain
P3M1	DATA	0xB1	; P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
P3M0	DATA	0xB2	; 					=10--->pure input,	11--->open drain
P4M1	DATA	0xB3	; P4M1.n,P4M0.n 	=00--->Standard,	01--->push-pull
P4M0	DATA	0xB4	; 					=10--->pure input,	11--->open drain
P5M1	DATA	0xC9	; P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
P5M0	DATA	0xCA	; 					=10--->pure input,	11--->open drain
P6M1	DATA	0xCB	; P6M1.n,P6M0.n 	=00--->Standard,	01--->push-pull
P6M0	DATA	0xCC	; 					=10--->pure input,	11--->open drain
P7M1	DATA	0xE1	;
P7M0	DATA	0xE2	;

;================================================================

P25 BIT P2.5
P26 BIT P2.6
P27 BIT P2.7

PWM0_high_H		DATA	030H
PWM0_high_L		DATA	031H
PWM0_low_H		DATA	032H
PWM0_low_L		DATA	033H

PWM1_high_H		DATA	034H
PWM1_high_L		DATA	035H
PWM1_low_H		DATA	036H
PWM1_low_L		DATA	037H

PWM2_high_H		DATA	038H
PWM2_high_L		DATA	039H
PWM2_low_H		DATA	03AH
PWM2_low_L		DATA	03BH

pwm0_H			DATA	03CH
pwm0_L			DATA	03DH
pwm1_H			DATA	03EH
pwm1_L			DATA	03FH
pwm2_H			DATA	040H
pwm2_L			DATA	041H

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		003BH				;7  PCA interrupt
		LJMP	F_PCA_Interrupt


		ORG		0100H
;******************** 主程序 **************************/
F_Main:
	CLR		A
	MOV		P0M1, A 	;设置为准双向口
 	MOV		P0M0, A
	MOV		P1M1, A 	;设置为准双向口
 	MOV		P1M0, A
	MOV		P2M1, A 	;设置为准双向口
 	MOV		P2M0, A
	MOV		P3M1, A 	;设置为准双向口
 	MOV		P3M0, A
	MOV		P4M1, A 	;设置为准双向口
 	MOV		P4M0, A
	MOV		P5M1, A 	;设置为准双向口
 	MOV		P5M0, A
	MOV		P6M1, A 	;设置为准双向口
 	MOV		P6M0, A
	MOV		P7M1, A 	;设置为准双向口
 	MOV		P7M0, A
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	LCALL	F_PCA_Init	;PCA初始化
	SETB	EA
	ANL		P2M1, #NOT 0E0H		; P2.7 P2.6 P2.5 设置为推挽输出
	ORL		P2M0, #0E0H

;=================== 主循环 ==================================
L_MainLoop:
	MOV		R7, #2
	LCALL	F_delay_ms

	MOV		A, pwm0_L		; if(++pwm0 >= PWM_HIGH_MAX)	pwm0 = PWM_HIGH_MIN
	ADD		A, #1
	MOV		pwm0_L, A
	MOV		A, pwm0_H
	ADDC	A, #0
	MOV		pwm0_H, A
	MOV		A, pwm0_L
	CLR		C
	SUBB	A, #LOW PWM_HIGH_MAX
	MOV		A, pwm0_H
	SUBB	A, #HIGH PWM_HIGH_MAX
	JC		L_PWM0_NotOverFollow
	MOV		pwm0_H, #HIGH PWM_HIGH_MIN
	MOV		pwm0_L, #LOW PWM_HIGH_MIN
L_PWM0_NotOverFollow:
	MOV		R5, #PCA0
	MOV		R6, pwm0_H
	MOV		R7, pwm0_L
	LCALL	F_PWMn_Update

	MOV		A, pwm1_L		; if(++pwm1 >= PWM_HIGH_MAX)	pwm1 = PWM_HIGH_MIN
	ADD		A, #1
	MOV		pwm1_L, A
	MOV		A, pwm1_H
	ADDC	A, #0
	MOV		pwm1_H, A
	MOV		A, pwm1_L
	CLR		C
	SUBB	A, #LOW PWM_HIGH_MAX
	MOV		A, pwm1_H
	SUBB	A, #HIGH PWM_HIGH_MAX
	JC		L_PWM1_NotOverFollow
	MOV		pwm1_H, #HIGH PWM_HIGH_MIN
	MOV		pwm1_L, #LOW PWM_HIGH_MIN
L_PWM1_NotOverFollow:
	MOV		R5, #PCA1
	MOV		R6, pwm1_H
	MOV		R7, pwm1_L
	LCALL	F_PWMn_Update

	MOV		A, pwm2_L		; if(++pwm2 >= PWM_HIGH_MAX)	pwm2 = PWM_HIGH_MIN
	ADD		A, #1
	MOV		pwm2_L, A
	MOV		A, pwm2_H
	ADDC	A, #0
	MOV		pwm2_H, A
	MOV		A, pwm2_L
	CLR		C
	SUBB	A, #LOW PWM_HIGH_MAX
	MOV		A, pwm2_H
	SUBB	A, #HIGH PWM_HIGH_MAX
	JC		L_PWM2_NotOverFollow
	MOV		pwm2_H, #HIGH PWM_HIGH_MIN
	MOV		pwm2_L, #LOW PWM_HIGH_MIN
L_PWM2_NotOverFollow:
	MOV		R5, #PCA2
	MOV		R6, pwm2_H
	MOV		R7, pwm2_L
	LCALL	F_PWMn_Update

	LJMP	L_MainLoop


;//========================================================================
;// 函数: F_delay_ms
;// 描述: 延时子程序。
;// 参数: R7: 延时ms数.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
F_delay_ms:
	PUSH	AR3		;入栈R3
	PUSH	AR4		;入栈R4

L_delay_ms_1:
	MOV		R3, #HIGH (Fosc_KHZ / 13)
	MOV		R4, #LOW (Fosc_KHZ / 13)
	
L_delay_ms_2:
	MOV		A, R4			;1T		Total 13T/loop
	DEC		R4				;2T
	JNZ		L_delay_ms_3	;4T
	DEC		R3
L_delay_ms_3:
	DEC		A				;1T
	ORL		A, R3			;1T
	JNZ		L_delay_ms_2	;4T
	
	DJNZ	R7, L_delay_ms_1

	POP		AR4		;出栈R2
	POP		AR3		;出栈R3
	RET


;========================================================================
; 函数: F_PWMn_Update
; 描述: 更新占空比数据。
; 参数: R5: PCA通道数。
;		R6,R7: PWM值.
; 返回: 无
; 版本: VER1.0
; 日期: 2014-2-15
; 备注: 
;========================================================================
F_PWMn_Update:
	PUSH	AR3
	PUSH	AR4

	CLR		C
	MOV		A, R7
	SUBB	A, #LOW PWM_HIGH_MAX
	MOV		A, R6
	SUBB	A, #HIGH PWM_HIGH_MAX
	JC		L_QuitCheckPwm_1
	MOV		R6, #HIGH PWM_HIGH_MAX	; 如果写入大于最大占空比数据，强制为最大占空比。
	MOV		R7, #LOW  PWM_HIGH_MAX
L_QuitCheckPwm_1:

	CLR		C
	MOV		A, R7
	SUBB	A, #LOW PWM_HIGH_MIN
	MOV		A, R6
	SUBB	A, #HIGH PWM_HIGH_MIN
	JNC		L_QuitCheckPwm_2
	MOV		R6, #HIGH PWM_HIGH_MIN	; 如果写入小于最小占空比数据，强制为最小占空比。
	MOV		R7, #LOW  PWM_HIGH_MIN
L_QuitCheckPwm_2:

	CLR		C
	MOV		A, #LOW PWM_DUTY	;计算并保存PWM输出低电平的PCA时钟脉冲个数
	SUBB	A, R7
	MOV		R4, A
	MOV		A, #HIGH PWM_DUTY
	SUBB	A, R6
	MOV		R3, A

	CJNE	R5, #PCA0, L_NotLoadPCA0
	CLR		CR		; 停止PCA一会， 一般不会影响PWM。
	MOV		PWM0_high_H, R6		; 数据装入占空比变量。
	MOV		PWM0_high_L, R7
	MOV		PWM0_low_H, R3
	MOV		PWM0_low_L, R4
	SETB	CR				; 启动PCA。
L_NotLoadPCA0:

	CJNE	R5, #PCA1, L_NotLoadPCA1
	CLR		CR		; 停止PCA一会， 一般不会影响PWM。
	MOV		PWM1_high_H, R6		; 数据装入占空比变量。
	MOV		PWM1_high_L, R7
	MOV		PWM1_low_H, R3
	MOV		PWM1_low_L, R4
	SETB	CR				; 启动PCA。
L_NotLoadPCA1:

	CJNE	R5, #PCA2, L_NotLoadPCA2
	CLR		CR		; 停止PCA一会， 一般不会影响PWM。
	MOV		PWM2_high_H, R6		; 数据装入占空比变量。
	MOV		PWM2_high_L, R7
	MOV		PWM2_low_H, R3
	MOV		PWM2_low_L, R4
	SETB	CR				; 启动PCA。
L_NotLoadPCA2:
	
	POP		AR4
	POP		AR3
	RET

;========================================================================
; 函数: F_PCA_Init
; 描述: PCA初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_PCA_Init:
	CLR		CR
	MOV		CH, #0
	MOV		CL, #0
	MOV		A, AUXR1
	ANL		A, # NOT(3 SHL 4)
	ORL		A, #PCA_P24_P25_P26_P27		;切换IO口
	MOV		AUXR1, A
	ANL		A, #NOT(7 SHL 1)
	ORL		A, #PCA_Clock_1T		;选择时钟源
	MOV		CMOD, A

	MOV		CCAPM0, #(PCA_Mode_HighPulseOutput OR ENABLE)	; 16位软件定时、高速脉冲输出、中断模式
	MOV		CCAPM1, #(PCA_Mode_HighPulseOutput OR ENABLE)	; 16位软件定时、高速脉冲输出、中断模式
	MOV		CCAPM2, #(PCA_Mode_HighPulseOutput OR ENABLE)	; 16位软件定时、高速脉冲输出、中断模式

	MOV		pwm0_H, #HIGH (PWM_DUTY / 4 * 1)	;给PWM一个初值
	MOV		pwm0_L, #LOW  (PWM_DUTY / 4 * 1)
	MOV		pwm1_H, #HIGH (PWM_DUTY / 4 * 2)
	MOV		pwm1_L, #LOW  (PWM_DUTY / 4 * 2)
	MOV		pwm2_H, #HIGH (PWM_DUTY / 4 * 3)
	MOV		pwm2_L, #LOW  (PWM_DUTY / 4 * 3)

	MOV		R5, #PCA0
	MOV		R6, pwm0_H
	MOV		R7, pwm0_L
	LCALL	F_PWMn_Update
	MOV		R5, #PCA1
	MOV		R6, pwm1_H
	MOV		R7, pwm1_L
	LCALL	F_PWMn_Update
	MOV		R5, #PCA2
	MOV		R6, pwm2_H
	MOV		R7, pwm2_L
	LCALL	F_PWMn_Update

	SETB	PPCA		; 高优先级中断
	SETB	CR			; 运行PCA定时器
	RET

;======================================================================

;========================================================================
; 函数: F_PCA_Interrupt
; 描述: PCA中断处理程序.
; 参数: None
; 返回: none.
; 版本: V1.0, 2012-11-22
;========================================================================
F_PCA_Interrupt:
	PUSH	PSW
	PUSH	ACC

	;=============== PCA模块0中断 ===================
	JNB		CCF0, L_QuitPCA0	; PCA模块0中断
	CLR		CCF0				; 清PCA模块0中断标志

	JNB		P25, L_PCA0_LoadLow
	MOV		A, CCAP0L			; 输出为高电平，则给影射寄存器装载高电平时间长度
	ADD		A, PWM0_high_L		; 加上高电平时间，
	MOV		CCAP0L, A			; 先写CCAP0L
	MOV		A, CCAP0H			;
	ADDC	A, PWM0_high_H	;
	MOV		CCAP0H, A			; 后写CCAP0H
	SJMP	L_QuitPCA0
L_PCA0_LoadLow:
	MOV		A, CCAP0L			; 输出为低电平，则给影射寄存器装载低电平时间长度
	ADD		A, PWM0_low_L		; 加上低电平时间，
	MOV		CCAP0L, A			; 先写CCAP0L
	MOV		A, CCAP0H			;
	ADDC	A, PWM0_low_H	;
	MOV		CCAP0H, A			; 后写CCAP0H
L_QuitPCA0:

	;=============== PCA模块1中断 ===================
	JNB		CCF1, L_QuitPCA1	; PCA模块1中断
	CLR		CCF1				; 清PCA模块1中断标志

	JNB		P26, L_PCA1_LoadLow
	MOV		A, CCAP1L			; 输出为高电平，则给影射寄存器装载高电平时间长度
	ADD		A, PWM1_high_L		; 加上高电平时间，
	MOV		CCAP1L, A			; 先写CCAP1L
	MOV		A, CCAP1H			;
	ADDC	A, PWM1_high_H	;
	MOV		CCAP1H, A			; 后写CCAP1H
	SJMP	L_QuitPCA1
L_PCA1_LoadLow:
	MOV		A, CCAP1L			; 输出为低电平，则给影射寄存器装载低电平时间长度
	ADD		A, PWM1_low_L		; 加上低电平时间，
	MOV		CCAP1L, A			; 先写CCAP1L
	MOV		A, CCAP1H			;
	ADDC	A, PWM1_low_H	;
	MOV		CCAP1H, A			; 后写CCAP1H
L_QuitPCA1:

	;=============== PCA模块2中断 ===================
	JNB		CCF2, L_QuitPCA2	; PCA模块2中断
	CLR		CCF2				; 清PCA模块2中断标志

	JNB		P27, L_PCA2_LoadLow
	MOV		A, CCAP2L			; 输出为高电平，则给影射寄存器装载高电平时间长度
	ADD		A, PWM2_high_L		; 加上高电平时间，
	MOV		CCAP2L, A			; 先写CCAP2L
	MOV		A, CCAP2H			;
	ADDC	A, PWM2_high_H	;
	MOV		CCAP2H, A			; 后写CCAP2H
	SJMP	L_QuitPCA2
L_PCA2_LoadLow:
	MOV		A, CCAP2L			; 输出为低电平，则给影射寄存器装载低电平时间长度
	ADD		A, PWM2_low_L		; 加上低电平时间，
	MOV		CCAP2L, A			; 先写CCAP2L
	MOV		A, CCAP2H			;
	ADDC	A, PWM2_low_H	;
	MOV		CCAP2H, A			; 后写CCAP2H
L_QuitPCA2:

	POP		ACC
	POP		PSW

	RETI
	

	END
