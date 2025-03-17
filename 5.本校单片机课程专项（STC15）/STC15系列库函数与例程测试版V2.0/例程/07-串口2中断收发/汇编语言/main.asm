
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

;串口2全双工中断方式收发通讯程序。本例程使用22.1184MHZ时钟，如要改变，请修改下面的"定义主时钟"的值并重新编译。

;串口设置为：115200,8,n,1.

;通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

;******************************************/

;/****************************** 用户定义宏 ***********************************/

;UART2_Baudrate	EQU		(-4608)	;   600bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-2304)	;  1200bps @ 11.0592MHz	UART2_Baudrate = (MAIN_Fosc / Baudrate)
;UART2_Baudrate	EQU		(-1152)	;  2400bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-576)	;  4800bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-288)	;  9600bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-144)	; 19200bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-72)	; 38400bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-48)	; 57600bps @ 11.0592MHz
;UART2_Baudrate	EQU		(-24)	;115200bps @ 11.0592MHz

;UART2_Baudrate	EQU		(-7680)	;   600bps @ 18.432MHz
;UART2_Baudrate	EQU		(-3840)	;  1200bps @ 18.432MHz
;UART2_Baudrate	EQU		(-1920)	;  2400bps @ 18.432MHz
;UART2_Baudrate	EQU		(-960)	;  4800bps @ 18.432MHz
;UART2_Baudrate	EQU		(-480)	;  9600bps @ 18.432MHz
;UART2_Baudrate	EQU		(-240)	; 19200bps @ 18.432MHz
;UART2_Baudrate	EQU		(-120)	; 38400bps @ 18.432MHz
;UART2_Baudrate	EQU		(-80)	; 57600bps @ 18.432MHz
;UART2_Baudrate	EQU		(-40)	;115200bps @ 18.432MHz

;UART2_Baudrate	EQU		(-9216)	;   600bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-4608)	;  1200bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-2304)	;  2400bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-1152)	;  4800bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-576)	;  9600bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-288)	; 19200bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-144)	; 38400bps @ 22.1184MHz
;UART2_Baudrate	EQU		(-96)	; 57600bps @ 22.1184MHz
UART2_Baudrate	EQU		(-48)	;115200bps @ 22.1184MHz

;UART2_Baudrate	EQU		(-6912)	; 1200bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-3456)	; 2400bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-1728)	; 4800bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-864)	; 9600bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-432)	;19200bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-216)	;38400bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-144)	;57600bps @ 33.1776MHz
;UART2_Baudrate	EQU		(-72)	;115200bps @ 33.1776MHz


;*******************************************************************
;*******************************************************************
P_SW2		DATA	0BAH
AUXR		DATA 	08EH
T2H			DATA 	0D6H
T2L			DATA 	0D7H
S2CON 		DATA	09AH
S2BUF 		DATA	09BH
IE2   		DATA	0AFH

RX2_Lenth	EQU		32		; 串口接收缓冲长度

B_TX2_Busy	BIT		20H.0	; 发送忙标志
TX2_Cnt		DATA	30H		; 发送计数
RX2_Cnt		DATA	31H		; 接收计数
RX2_Buffer	DATA	40H		;40 ~ 5FH 接收缓冲

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;*******************************************************************
;*******************************************************************
		ORG		00H				;reset
		LJMP	F_Main

		ORG		43H				;8  UART2 interrupt
		LJMP	F_UART2_Interrupt


;******************** 主程序 **************************/
F_Main:
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	CLR		B_TX2_Busy
	MOV		RX2_Cnt, #0;
	MOV		TX2_Cnt, #0;

	ANL		S2CON, #NOT (1 SHL 7)	;8位数据
	ANL		P_SW2, #NOT 01H			; UART2 使用P1.0 P1.1口	默认
;	ORL		P_SW2, #1				; UART2 使用P4.6 P4.7口

	ANL		AUXR, #NOT (1 SHL 4)	; Timer stop,  默认		波特率使用Timer2产生
	ANL		AUXR, #NOT (1 SHL 3)	; Timer2 set As Timer 默认
	ORL		AUXR, #(1 SHL 2)		; Timer2 set as 1T mode
	MOV		T2H, #HIGH UART2_Baudrate
	MOV		T2L, #LOW  UART2_Baudrate
	ORL		S2CON, #(1 SHL 4)		; 允许接收
	ORL		IE2, #1					; 允许中断
	ORL		AUXR, #(1 SHL 4)		; Timer run enable

	SETB	EA		; 允许全局中断
	
;=================== 主循环 ==================================
L_MainLoop:
	MOV		A, TX2_Cnt
	CJNE	A, RX2_Cnt, L_ReturnData
	SJMP	L_QuitCheckRx2
L_ReturnData:					; 收到过数据
	JB		B_TX2_Busy, L_QuitCheckRx2		; 发送忙， 退出
	SETB	B_TX2_Busy			; 标志发送忙
	MOV		A, #RX2_Buffer
	ADD		A, TX2_Cnt
	MOV		R0, A
	MOV		S2BUF, @R0		; 发一个字节
	INC		TX2_Cnt
	MOV		A, TX2_Cnt
	CJNE	A, #RX2_Lenth, L_QuitCheckRx2
	MOV		TX2_Cnt, #0		; 避免溢出处理
L_QuitCheckRx2:

	LJMP	L_MainLoop


;********************* UART2中断函数************************
F_UART2_Interrupt:
	PUSH	PSW
	PUSH	ACC
	
	MOV		A, S2CON
	JNB		ACC.0, L_QuitUartRx
	ANL		S2CON, #NOT 1;
	MOV		A, #RX2_Buffer
	ADD		A, RX2_Cnt
	MOV		R0, A
	MOV		@R0, S2BUF	;保存一个字节
	INC		RX2_Cnt
	MOV		A, RX2_Cnt
	CJNE	A, #RX2_Lenth, L_QuitUartRx
	MOV		RX2_Cnt, #0		; 避免溢出处理
L_QuitUartRx:

	MOV		A, S2CON
	JNB		ACC.1, L_QuitUartTx
	ANL		S2CON, #NOT 2;
	CLR		B_TX2_Busy		; 清除发送忙标志
L_QuitUartTx:

	POP		ACC
	POP		PSW
	RETI

	END



