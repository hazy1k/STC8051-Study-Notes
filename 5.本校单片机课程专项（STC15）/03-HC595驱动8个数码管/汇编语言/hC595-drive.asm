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

;用STC的MCU的IO方式控制74HC595驱动8位数码管。

;显示效果为: 数码时钟.

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;******************************************/

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ	EQU	22118	;22118KHZ

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

Timer0_Reload	EQU		(65536 - Fosc_KHZ)	; Timer 0 中断频率, 1000次/秒

DIS_DOT			EQU		020H
DIS_BLACK		EQU		010H
DIS_			EQU		011H

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

AUXR      DATA 08EH
P4        DATA 0C0H
P5        DATA 0C8H

        
;*************	IO口定义	**************/
P_HC595_SER   BIT P4.0	;	//pin 14	SER		data input
P_HC595_RCLK  BIT P5.4	;	//pin 12	RCLk	store (latch) clock
P_HC595_SRCLK BIT P4.3	;	//pin 11	SRCLK	Shift data clock

;*************	本地变量声明	**************/
Flag0			DATA	20H
B_1ms			BIT		Flag0.0	;	1ms标志

LED8			DATA	30H		;	显示缓冲 30H ~ 37H
display_index	DATA	38H		;	显示位索引

hour			DATA	39H		;
minute			DATA	3AH
second			DATA	3BH		;
msecond_H		DATA	3CH		;
msecond_L		DATA	3DH		;


;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		000BH				;1  Timer0 interrupt
		LJMP	F_Timer0_Interrupt

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

	MOV		display_index, #0
	MOV		R0, #LED8
	MOV		R2, #8
L_ClearLoop:
	MOV		@R0, #DIS_BLACK		;上电消隐
	INC		R0
	DJNZ	R2, L_ClearLoop
	
	CLR		TR0
	ORL		AUXR, #(1 SHL 7)	; Timer0_1T();
	ANL		TMOD, #NOT 04H		; Timer0_AsTimer();
	ANL		TMOD, #NOT 03H		; Timer0_16bitAutoReload();
	MOV		TH0, #Timer0_Reload / 256	;Timer0_Load(Timer0_Reload);
	MOV		TL0, #Timer0_Reload MOD 256
	SETB	ET0			; Timer0_InterruptEnable();
	SETB	TR0			; Timer0_Run();
	SETB	EA			; 打开总中断
	
	MOV		hour,   #12	; 初始化时间值
	MOV		minute, #0
	MOV		second, #0
	LCALL	F_DisplayRTC
	

;=================== 主循环 ==================================
L_Main_Loop:
	JNB		B_1ms,  L_Main_Loop		;1ms未到
	CLR		B_1ms
	
	INC		msecond_L		;msecond + 1
	MOV		A, msecond_L
	JNZ		L_Check1000ms
	INC		msecond_H
L_Check1000ms:	
	MOV		A, msecond_L	;msecond - 1000
	CLR		C
	SUBB	A, #LOW 1000
	MOV		A, msecond_H
	SUBB	A, #HIGH 1000
	JC		L_Main_Loop		;if(msecond < 1000), jmp
	
	MOV		msecond_L, #0	;if(msecond >= 1000)
	MOV		msecond_H, #0

	LCALL	F_RTC
	LCALL	F_DisplayRTC

	LJMP	L_Main_Loop

;**********************************************/



;========================================================================
; 函数: F_DisplayRTC
; 描述: 显示时钟子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_DisplayRTC:
	PUSH	B		;B入栈
	
	MOV		A, hour
	MOV		B, #10
	DIV		AB
	MOV		LED8, A
	MOV		LED8+1, B
	MOV		LED8+2, #DIS_;
	
	MOV		A, minute
	MOV		B, #10
	DIV		AB
	MOV		LED8+3, A;
	MOV		LED8+4, B;
	MOV		LED8+5, #DIS_;

	MOV		A, second
	MOV		B, #10
	DIV		AB
	MOV		LED8+6, A;
	MOV		LED8+7, B;
	
	POP		B		;B出栈
	RET

;========================================================================
; 函数: F_RTC
; 描述: RTC演示子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_RTC:
	INC		second		; second + 1
	MOV		A, second
	CLR		C
	SUBB	A,#60
	JC		L_QuitRTC	; second >= 60?
	MOV		second, #0;

	INC		minute		; minute + 1
	MOV		A, minute
	CLR		C
	SUBB	A,#60
	JC		L_QuitRTC	; minute >= 60?
	MOV		minute, #0

	INC		hour		; hour + 1
	MOV		A, hour
	CLR		C
	SUBB	A,#24
	JC		L_QuitRTC	; hour >= 24?
	MOV		hour, #0
L_QuitRTC:
	RET


; *********************** 显示相关程序 ****************************************
T_Display:						;标准字库
;	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
DB	03FH,006H,05BH,04FH,066H,06DH,07DH,007H,07FH,06FH,077H,07CH,039H,05EH,079H,071H
;  black  -    H    J    K	  L	   N	o    P	  U    t    G    Q    r    M    y
DB	000H,040H,076H,01EH,070H,038H,037H,05CH,073H,03EH,078H,03dH,067H,050H,037H,06EH
;    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
DB	0BFH,086H,0DBH,0CFH,0E6H,0EDH,0FDH,087H,0FFH,0EFH,046H

T_COM:
DB	001H,002H,004H,008H,010H,020H,040H,080H		;	位码


;========================================================================
; 函数: F_Send_595
; 描述: 向HC595发送一个字节子程序。
; 参数: ACC: 要发送的字节数据.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_Send_595:
	PUSH	02H		;R2入栈
	MOV		R2, #8
L_Send_595_Loop:
	RLC		A
	MOV		P_HC595_SER,C
	SETB	P_HC595_SRCLK
	CLR		P_HC595_SRCLK
	DJNZ	R2, L_Send_595_Loop
	POP		02H		;R2出栈
	RET

;========================================================================
; 函数: F_DisplayScan
; 描述: 显示扫描子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_DisplayScan:
	PUSH	DPH		;DPH入栈
	PUSH	DPL		;DPL入栈
	PUSH	00H		;R0 入栈
	
	MOV		DPTR, #T_COM
	MOV		A, display_index
	MOVC	A, @A+DPTR
	CPL		A
	LCALL	F_Send_595		;输出位码
	
	MOV		DPTR, #T_Display
	MOV		A, display_index
	ADD		A, #LED8
	MOV		R0, A
	MOV		A, @R0
	MOVC	A, @A+DPTR
	LCALL	F_Send_595		;输出段码

	SETB	P_HC595_RCLK
	CLR		P_HC595_RCLK	;	锁存输出数据
	INC		display_index
	MOV		A, display_index
	ANL		A, #0F8H			; if(display_index >= 8)
	JZ		L_QuitDisplayScan
	MOV		display_index, #0;	;8位结束回0
L_QuitDisplayScan:
	POP		00H		;R0 出栈
	POP		DPL		;DPL出栈
	POP		DPH		;DPH出栈
	RET


;*******************************************************************
;**************** 中断函数 ***************************************************

F_Timer0_Interrupt:	;Timer0 1ms中断函数
	PUSH	PSW		;PSW入栈
	PUSH	ACC		;ACC入栈

	LCALL	F_DisplayScan	; 1ms扫描显示一位
	SETB	B_1ms			; 1ms标志

	POP		ACC		;ACC出栈
	POP		PSW		;PSW出栈
	RETI
	


	END



