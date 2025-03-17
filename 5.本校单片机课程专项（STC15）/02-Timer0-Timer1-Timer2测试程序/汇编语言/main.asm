;/*---------------------------------------------------------------------*/
;/* --- STC MCU International Limited ----------------------------------*/
;/* --- STC 1T Series MCU Demo Programme -------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
;/* --- Web: www.GXWMCU.com --------------------------------------------*/
;/* --- QQ:  800003751 -------------------------------------------------*/
;/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/


;*************	功能说明	**************

; 本程序演示3个定时器的使用, 本例程均使用16位自动重装.

; 下载时, 选择时钟 24MHZ (用户可自行修改频率).

; 定时器0做16位自动重装, 中断频率为1000HZ，中断函数从P1.7取反输出500HZ方波信号.

; 定时器1做16位自动重装, 中断频率为2000HZ，中断函数从P1.6取反输出1000HZ方波信号.

; 定时器2做16位自动重装, 中断频率为3000HZ，中断函数从P4.7取反输出1500HZ方波信号.

;******************************************/

;/****************************** 用户定义宏 ***********************************/


STACK_POIRTER	EQU		0D0H	;堆栈开始地址


;*******************************************************************
;*******************************************************************
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

AUXR	DATA	08EH
INT_CLKO DATA   0x8F
IE2		DATA	0xAF
P4		DATA	0xC0
T2H		DATA	0xD6
T2L		DATA	0xD7



;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		000BH				;1  Timer0 interrupt
		LJMP	F_Timer0_Interrupt

		ORG		001BH				;3  Timer1 interrupt
		LJMP	F_Timer1_Interrupt

		ORG		0063H				;12  Timer2 interrupt
		LJMP	F_Timer2_Interrupt

;*******************************************************************
;*******************************************************************




;******************** 主程序 **************************/
		ORG		0100H		;reset
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

	LCALL	F_Timer0_init
	LCALL	F_Timer1_init
	LCALL	F_Timer2_init
	SETB	EA
	

;=================== 主循环 ==================================
L_Main_Loop:

	LJMP	L_Main_Loop

;**********************************************/

;========================================================================
; 函数: F_Timer0_init
; 描述: timer0初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer0_init:
	CLR		TR0	; 停止计数
	SETB	ET0	; 允许中断
;	SETB	PT0	; 高优先级中断
	ANL		TMOD, #NOT 0x03		;
	ORL		TMOD, #0			; 工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
;	ORL		TMOD, #0x04			; 对外计数或分频
	ANL		TMOD, #NOT 0x04		; 定时
;	ORL		INT_CLKO, #0x01		; 输出时钟
	ANL		INT_CLKO, #NOT 0x01	; 不输出时钟

;	ANL		AUXR, #NOT 0x80		; 12T mode
	ORL		AUXR, #0x80			; 1T mode
	MOV		TH0, #HIGH (-24000)	; - 24000000 / 1000
	MOV		TL0, #LOW  (-24000)	;
	SETB	TR0	; 开始运行
	RET


;========================================================================
; 函数: F_Timer1_init
; 描述: timer1初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer1_init:
	CLR		TR1	; 停止计数
	SETB	ET1	; 允许中断
;	SETB	PT1	; 高优先级中断
	ANL		TMOD, #NOT 0x30		;
	ORL		TMOD, #(0 SHL 4)	; 工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
;	ORL		TMOD, #0x40			; 对外计数或分频
	ANL		TMOD, #NOT 0x40		; 定时
;	ORL		INT_CLKO, #0x02		; 输出时钟
	ANL		INT_CLKO, #NOT 0x02	; 不输出时钟

;	ANL		AUXR, #NOT 0x40		; 12T mode
	ORL		AUXR, #0x40			; 1T mode
	MOV		TH1, #HIGH (-12000)	; - 24000000 / 2000
	MOV		TL1, #LOW  (-12000)	;
	SETB	TR1	; 开始运行
	RET

;========================================================================
; 函数: F_Timer2_init
; 描述: timer2初始化函数.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2015-1-12
;========================================================================
F_Timer2_init:
	ANL		AUXR, #NOT 0x1c		; 停止计数, 定时模式, 12T模式

;	ANL		IE2, #NOT (1 SHL 2)	; 禁止中断
	ORL		IE2, #(1 SHL 2)		; 允许中断
;	ORL		INT_CLKO, #0x04		; 输出时钟
	ANL		INT_CLKO, #NOT 0x04	; 不输出时钟

;	ORL		AUXR, #(1 SHL 3)	; 对外计数或分频
;	ORL		INT_CLKO, #0x02		; 输出时钟
	ANL		INT_CLKO, #NOT 0x02	; 不输出时钟

	ORL		AUXR, #(1 SHL 2)	; 1T mode
	MOV		T2H, #HIGH (-8000)	; - 24000000 / 3000
	MOV		T2L, #LOW  (-8000)	;

	ORL		AUXR, #(1 SHL 4)	; 开始运行
	RET


;**************** 中断函数 ***************************************************
F_Timer0_Interrupt:	;Timer0 1ms中断函数
	PUSH	PSW		;PSW入栈
	PUSH	ACC		;ACC入栈

	CPL		P1.7

	POP		ACC		;ACC出栈
	POP		PSW		;PSW出栈
	RETI
	
F_Timer1_Interrupt:	;Timer1 1ms中断函数
	PUSH	PSW		;PSW入栈
	PUSH	ACC		;ACC入栈

	CPL		P1.6

	POP		ACC		;ACC出栈
	POP		PSW		;PSW出栈
	RETI
	
F_Timer2_Interrupt:	;Timer2 1ms中断函数
	PUSH	PSW		;PSW入栈
	PUSH	ACC		;ACC入栈

	CPL		P4.7

	POP		ACC		;ACC出栈
	POP		PSW		;PSW出栈
	RETI
	


	END



