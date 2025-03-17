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

;程序使用P4.7 P4.6 P1.6 P1.7 来演示跑马灯，输出低驱动。

;******************************************

Fosc_KHZ	EQU	22118	;22118KHZ

STACK_POIRTER	EQU		0D0H	;堆栈开始地质

;*******************************************************************
;*******************************************************************
P4   DATA 0C0H

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


;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		0003H				;0 INT0 interrupt
		RETI
		LJMP	F_INT0_Interrupt      

		ORG		000BH				;1  Timer0 interrupt
		LJMP	F_Timer0_Interrupt

		ORG		0013H				;2  INT1 interrupt
		LJMP	F_INT1_Interrupt      

		ORG		001BH				;3  Timer1 interrupt
		LJMP	F_Timer1_Interrupt

		ORG		0023H				;4  UART1 interrupt
		LJMP	F_UART1_Interrupt

		ORG		002BH				;5  ADC and SPI interrupt
		LJMP	F_ADC_Interrupt

		ORG		0033H				;6  Low Voltage Detect interrupt
		LJMP	F_LVD_Interrupt

		ORG		003BH				;7  PCA interrupt
		LJMP	F_PCA_Interrupt

		ORG		0043H				;8  UART2 interrupt
		LJMP	F_UART2_Interrupt

		ORG		004BH				;9  SPI interrupt
		LJMP	F_SPI_Interrupt

		ORG		0053H				;10  INT2 interrupt
		LJMP	F_INT2_Interrupt

		ORG		005BH				;11  INT3 interrupt
		LJMP	F_INT3_Interrupt

		ORG		0063H				;12  Timer2 interrupt
		LJMP	F_Timer2_Interrupt

		ORG		0083H				;16  INT4 interrupt
		LJMP	F_INT4_Interrupt


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
	MOV		PSW, #0		;选择第0组R0~R7

L_MainLoop:
	CLR		P1.7
	MOV		A, #250
	LCALL	F_delay_ms		;延时250ms
	LCALL	F_delay_ms		;延时250ms
	SETB	P1.7

	CLR		P1.6
	MOV		A, #250
	LCALL	F_delay_ms		;延时250ms
	LCALL	F_delay_ms		;延时250ms
	SETB	P1.6

	CLR		P4.7
	MOV		A, #250
	LCALL	F_delay_ms		;延时250ms
	LCALL	F_delay_ms		;延时250ms
	SETB	P4.7

	CLR		P4.6
	MOV		A, #250
	LCALL	F_delay_ms		;延时250ms
	LCALL	F_delay_ms		;延时250ms
	SETB	P4.6

	SJMP	L_MainLoop

;*******************************************************************
;*******************************************************************



;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_delay_ms:
	PUSH	02H		;入栈R2
	PUSH	03H		;入栈R3
	PUSH	04H		;入栈R4

	MOV		R2,A

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
	
	DJNZ	R2, L_delay_ms_1

	POP		04H		;出栈R2
	POP		03H		;出栈R3
	POP		02H		;出栈R4
	RET


;**************** 中断函数 ***************************************************

F_Timer0_Interrupt:	;Timer0 1ms中断函数
	RETI
	
F_Timer1_Interrupt:
	RETI

F_Timer2_Interrupt:
	RETI

F_INT0_Interrupt:
	RETI
	
F_INT1_Interrupt:
	RETI

F_INT2_Interrupt:
	RETI

F_INT3_Interrupt:
	RETI

F_INT4_Interrupt:
	RETI

F_UART1_Interrupt:
	RETI

F_UART2_Interrupt:
	RETI

F_ADC_Interrupt:
	RETI

F_LVD_Interrupt:
	RETI

F_PCA_Interrupt:
	RETI

F_SPI_Interrupt:
	RETI


	END



