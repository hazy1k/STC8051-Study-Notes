
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

;串口1全双工中断方式收发通讯程序。本例程使用22.1184MHZ时钟，如要改变，请修改下面的"定义主时钟"的值并重新编译。

;串口设置为：115200,8,n,1.

;通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

;******************************************/

;/****************************** 用户定义宏 ***********************************/

;UART1_Baudrate	EQU		(-4608)	;   600bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-2304)	;  1200bps @ 11.0592MHz	UART1_Baudrate = (MAIN_Fosc / Baudrate)
;UART1_Baudrate	EQU		(-1152)	;  2400bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-576)	;  4800bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-288)	;  9600bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-144)	; 19200bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-72)	; 38400bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-48)	; 57600bps @ 11.0592MHz
;UART1_Baudrate	EQU		(-24)	;115200bps @ 11.0592MHz

;UART1_Baudrate	EQU		(-7680)	;   600bps @ 18.432MHz
;UART1_Baudrate	EQU		(-3840)	;  1200bps @ 18.432MHz
;UART1_Baudrate	EQU		(-1920)	;  2400bps @ 18.432MHz
;UART1_Baudrate	EQU		(-960)	;  4800bps @ 18.432MHz
;UART1_Baudrate	EQU		(-480)	;  9600bps @ 18.432MHz
;UART1_Baudrate	EQU		(-240)	; 19200bps @ 18.432MHz
;UART1_Baudrate	EQU		(-120)	; 38400bps @ 18.432MHz
;UART1_Baudrate	EQU		(-80)	; 57600bps @ 18.432MHz
;UART1_Baudrate	EQU		(-40)	;115200bps @ 18.432MHz

;UART1_Baudrate	EQU		(-9216)	;   600bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-4608)	;  1200bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-2304)	;  2400bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-1152)	;  4800bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-576)	;  9600bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-288)	; 19200bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-144)	; 38400bps @ 22.1184MHz
;UART1_Baudrate	EQU		(-96)	; 57600bps @ 22.1184MHz
UART1_Baudrate	EQU		(-48)	;115200bps @ 22.1184MHz

;UART1_Baudrate	EQU		(-6912)	; 1200bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-3456)	; 2400bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-1728)	; 4800bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-864)	; 9600bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-432)	;19200bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-216)	;38400bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-144)	;57600bps @ 33.1776MHz
;UART1_Baudrate	EQU		(-72)	;115200bps @ 33.1776MHz


;*******************************************************************
;*******************************************************************
P_SW1 		DATA	0A2H
AUXR		DATA 	08EH
T2H			DATA 	0D6H
T2L			DATA 	0D7H


RX1_Lenth	EQU		32		; 串口接收缓冲长度

B_TX1_Busy	BIT		20H.0	; 发送忙标志
TX1_Cnt		DATA	30H		; 发送计数
RX1_Cnt		DATA	31H		; 接收计数
RX1_Buffer	DATA	40H		;40 ~ 5FH 接收缓冲

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;*******************************************************************
;*******************************************************************
		ORG		00H				;reset
		LJMP	F_Main

		ORG		23H				;4  UART1 interrupt
		LJMP	F_UART1_Interrupt


;******************** 主程序 **************************/
F_Main:
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	CLR		B_TX1_Busy
	MOV		RX1_Cnt, #0;
	MOV		TX1_Cnt, #0;

	
	MOV		A, SCON
	ANL		A, #03FH
	ORL		A, #040H	; 8位数据
	MOV		SCON, A

	ANL		P_SW1, #NOT 0C0H	; UART1 使用P30 P31口	默认
;	ORL		P_SW1, #040H		; UART1 使用P36 P37口
;	ORL		P_SW1, #080H		; UART1 使用P16 P17口

;	CLR		TR1					; Timer Stop	波特率使用Timer1产生
;	ANL		AUXR, #NOT 01H		; S1 BRT Use Timer1;
;	ORL		AUXR, #(1 SHL 6)	; Timer1 set as 1T mode
;	MOV		TH1, #HIGH UART1_Baudrate
;	MOV		TL1, #LOW  UART1_Baudrate
;	SETB	TR1

	ANL		AUXR, #NOT (1 SHL 4)	; Timer stop	波特率使用Timer2产生
	ORL		AUXR, #001H				; S1 BRT Use Timer2;
	ORL		AUXR, #(1 SHL 2)		; Timer2 set as 1T mode
	MOV		T2H, #HIGH UART1_Baudrate
	MOV		T2L, #LOW  UART1_Baudrate
	ORL		AUXR, #(1 SHL 4)		; Timer run enable

	SETB	REN		; 允许接收
	SETB	ES		; 允许中断

	SETB	EA		; 允许全局中断
	
;=================== 主循环 ==================================
L_MainLoop:
	MOV		A, TX1_Cnt
	CJNE	A, RX1_Cnt, L_ReturnData
	SJMP	L_QuitCheckRx1
L_ReturnData:					; 收到过数据
	JB		B_TX1_Busy, L_QuitCheckRx1		; 发送忙， 退出
	SETB	B_TX1_Busy			; 标志发送忙
	MOV		A, #RX1_Buffer
	ADD		A, TX1_Cnt
	MOV		R0, A
	MOV		SBUF, @R0		; 发一个字节
	INC		TX1_Cnt
	MOV		A, TX1_Cnt
	CJNE	A, #RX1_Lenth, L_QuitCheckRx1
	MOV		TX1_Cnt, #0		; 避免溢出处理
L_QuitCheckRx1:
	LJMP	L_MainLoop


;********************* UART1中断函数************************
F_UART1_Interrupt:
	PUSH	PSW
	PUSH	ACC
	
	JNB		RI, L_QuitUartRx
	CLR		RI
	MOV		A, #RX1_Buffer
	ADD		A, RX1_Cnt
	MOV		R0, A
	MOV		@R0, SBUF	;保存一个字节
	INC		RX1_Cnt
	MOV		A, RX1_Cnt
	CJNE	A, #RX1_Lenth, L_QuitUartRx
	MOV		RX1_Cnt, #0		; 避免溢出处理
L_QuitUartRx:

	JNB		TI, L_QuitUartTx
	CLR		TI
	CLR		B_TX1_Busy		; 清除发送忙标志
L_QuitUartTx:

	POP		ACC
	POP		PSW
	RETI

	END



