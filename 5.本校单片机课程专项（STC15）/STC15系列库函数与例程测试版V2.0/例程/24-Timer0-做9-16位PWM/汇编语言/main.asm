
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

;本程序演示使用定时器做软件PWM。

;定时器0做16位自动重装，中断，从T0CLKO高速输出PWM。

;本例程是使用STC15F/L系列MCU的定时器T0做模拟PWM的例程。

;PWM可以是任意的量程。但是由于软件重装需要一点时间，所以PWM占空比最小为32T/周期，最大为(周期-32T)/周期, T为时钟周期。

;PWM频率为周期的倒数。假如周期为6000, 使用24MHZ的主频，则PWM频率为4000HZ。

;******************************************/

;***************************用户宏定义*******************************************************
Fosc_KHZ		EQU		24000	//定义主时钟, KHZ

PWM_DUTY		EQU		6000		//定义PWM的周期，数值为PCA所选择的时钟脉冲个数。
PWM_HIGH_MIN	EQU		32			//限制PWM输出的最小占空比, 避免中断里重装参数时间不够。
PWM_HIGH_MAX	EQU		(PWM_DUTY - PWM_HIGH_MIN)		//限制PWM输出的最大占空比。

;********************************************************************************************

P3M1		DATA 0B1H	; P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
P3M0		DATA 0B2H	; 					=10--->pure input,	11--->open drain
AUXR		DATA 08EH	;
INT_CLKO	DATA 08FH	;

P_PWM	BIT	P3.5	; 定义PWM输出引脚。
;P_PWM	BIT	P1.4	; 定义PWM输出引脚。STC15W204S

pwm_H		DATA	030H		; 定义PWM输出高电平的时间的变量。用户操作PWM的变量。
pwm_L		DATA	031H
PWM_high_H	DATA	032H	; 中间变量，用户请勿修改
PWM_high_L	DATA	033H
PWM_low_H	DATA	034H
PWM_low_L	DATA	035H

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;*******************************************************************
;*******************************************************************

	ORG		00H				;reset
	LJMP	F_Main

	ORG		0BH				;1  Timer0 interrupt
	LJMP	F_Timer0_Interrupt


;******************** 主程序 **************************/
F_Main:
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	CLR		P_PWM
	ANL		P3M1, #NOT (1 SHL 5)	; P3.5 设置为推挽输出
	ORL		P3M0, #(1 SHL 5);

;	ANL		P1M1, #NOT (1 SHL 4)	; P1.4 设置为推挽输出	STC15W204S
;	ORL		P1M0, #(1 SHL 4)		;

	CLR		TR0		; 停止计数
	SETB	ET0		; 允许中断
	SETB	PT0		; 高优先级中断
	ANL		TMOD, #NOT 003H			; 工作模式,0: 16位自动重装
	ORL		AUXR, #080H			; 1T
	ANL		TMOD, #NOT 004H		; 定时
	ORL		INT_CLKO, #001H		; 输出时钟

	MOV		TH0, #0
	MOV		TL0, #0
	SETB	TR0		; 开始运行

	SETB	EA

	MOV		pwm_H, #HIGH (PWM_DUTY / 10)	;给PWM一个初值，这里为10%占空比
	MOV		pwm_L, #LOW  (PWM_DUTY / 10)
	MOV		R6, pwm_H
	MOV		R7, pwm_L
	LCALL	F_PWMn_Update		; 计算PWM重装值

;=================== 主循环 ==================================
L_MainLoop1:
	MOV		R7, #2
	LCALL	F_delay_ms

	MOV		A, pwm_L		; if(++pwm >= PWM_HIGH_MAX)
	ADD		A, #1
	MOV		pwm_L, A
	MOV		A, pwm_H
	ADDC	A, #0
	MOV		pwm_H, A
	MOV		A, pwm_L
	CLR		C
	SUBB	A, #LOW PWM_HIGH_MAX
	MOV		A, pwm_H
	SUBB	A, #HIGH PWM_HIGH_MAX
	JC		L_PWM_NotUpOverFollow		; PWM逐渐加到最大
	MOV		pwm_H, #HIGH PWM_HIGH_MAX
	MOV		pwm_L, #LOW PWM_HIGH_MAX
	SJMP	L_MainLoop2
L_PWM_NotUpOverFollow:
	MOV		R6, pwm_H
	MOV		R7, pwm_L
	LCALL	F_PWMn_Update
	SJMP	L_MainLoop1
	
L_MainLoop2:
	MOV		R7, #2
	LCALL	F_delay_ms

	MOV		A, pwm_L		; if(++pwm < PWM_HIGH_MIN)
	CLR		C
	SUBB	A, #1
	MOV		pwm_L, A
	MOV		A, pwm_H
	SUBB	A, #0
	MOV		pwm_H, A
	MOV		A, pwm_L
	CLR		C
	SUBB	A, #LOW PWM_HIGH_MIN
	MOV		A, pwm_H
	SUBB	A, #HIGH PWM_HIGH_MIN
	JNC		L_PWM_NotDnOverFollow		; PWM逐渐减到最小
	MOV		pwm_H, #HIGH PWM_HIGH_MIN
	MOV		pwm_L, #LOW PWM_HIGH_MIN
	SJMP	L_MainLoop1
L_PWM_NotDnOverFollow:
	MOV		R6, pwm_H
	MOV		R7, pwm_L
	LCALL	F_PWMn_Update
	SJMP	L_MainLoop2



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
; 参数: R6,R7: PWM值。
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
	MOV		A, R7	;计算并保存PWM输出低电平的T0时钟脉冲个数
	SUBB	A, #LOW PWM_DUTY
	MOV		R4, A
	MOV		A, R6
	SUBB	A, #HIGH PWM_DUTY
	MOV		R3, A

	CLR		C
	MOV		A, #0	;计算并保存PWM输出高电平的T0时钟脉冲个数
	SUBB	A, R7
	MOV		R7, A
	MOV		A, #0
	SUBB	A, R6
	MOV		R6, A

	CLR		EA		; 禁止一会中断， 一般不会影响PWM。
	MOV		PWM_high_H, R6		; 数据装入占空比变量。
	MOV		PWM_high_L, R7
	MOV		PWM_low_H, R3
	MOV		PWM_low_L, R4
	SETB	EA
	
	POP		AR4
	POP		AR3
	RET



;********************* Timer0中断函数************************/
F_Timer0_Interrupt:
	PUSH	PSW
	PUSH	ACC
	JNB		P_PWM, L_T0_LoadLow
	MOV		TH0, PWM_low_H		; 如果是输出高电平，则装载低电平时间。
	MOV		TL0, PWM_low_L
	SJMP	L_QuitTimer0
L_T0_LoadLow:
	MOV		TH0, PWM_high_H		; 如果是输出低电平，则装载高电平时间。
	MOV		TL0, PWM_high_L

L_QuitTimer0:

	POP		ACC
	POP		PSW

	RETI
	

	END



