
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

; 4串口全双工中断方式收发通讯程序。本例程使用22.1184MHZ时钟.

; 通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

;******************************************/

;/****************************** 用户定义宏 ***********************************/

UART1_Baudrate	EQU		(-48)	;115200bps @ 22.1184MHz		UART1_Baudrate = -(MAIN_Fosc / 4 / Baudrate)
UART2_Baudrate	EQU		(-96)	; 57600bps @ 22.1184MHz		UART2_Baudrate = -(MAIN_Fosc / 4 / Baudrate)
UART3_Baudrate	EQU		(-144)	; 38400bps @ 22.1184MHz		UART3_Baudrate = -(MAIN_Fosc / 4 / Baudrate)
UART4_Baudrate	EQU		(-288)	; 19200bps @ 22.1184MHz		UART4_Baudrate = -(MAIN_Fosc / 4 / Baudrate)


;*******************************************************************
;*******************************************************************
P_SW1 		DATA	0A2H
AUXR		DATA 	08EH
T2H			DATA 	0D6H
T2L			DATA 	0D7H
P_SW2		DATA	0BAH
S2CON 		DATA	09AH
S2BUF 		DATA	09BH
IE2   		DATA	0AFH

T4T3M 	DATA	0xD1;
T4H		DATA	0xD2;
T4L		DATA 	0xD3;
T3H		DATA 	0xD4;
T3L		DATA 	0xD5;
S3CON 	DATA	0xAC;
S3BUF 	DATA	0xAD;
S4CON	DATA	0x84;
S4BUF	DATA	0x85;
INT_CLKO DATA   0x8F;

P3M1 	DATA	0xB1;	//P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
P3M0	DATA	0xB2;	//					=10--->pure input,	11--->open drain
P4M1	DATA 	0xB3;	//P4M1.n,P4M0.n 	=00--->Standard,	01--->push-pull
P4M0	DATA 	0xB4;	//					=10--->pure input,	11--->open drain
P5M1	DATA 	0xC9;	//	P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
P5M0 	DATA	0xCA;	//					=10--->pure input,	11--->open drain
P6M1 	DATA	0xCB;	//	P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
P6M0	DATA	0xCC;	//					=10--->pure input,	11--->open drain
P7M1	DATA	0xE1;
P7M0	DATA	0xE2;
P1M1 	DATA	0x91;	//P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull		实际上1T的都一样
P1M0	DATA	0x92;	//					=10--->pure input,	11--->open drain
P0M1	DATA 	0x93;	//P0M1.n,P0M0.n 	=00--->Standard,	01--->push-pull
P0M0	DATA 	0x94;	//					=10--->pure input,	11--->open drain
P2M1	DATA 	0x95;	//P2M1.n,P2M0.n 	=00--->Standard,	01--->push-pull
P2M0 	DATA	0x96;	//					=10--->pure input,	11--->open drain


RX1_Length	EQU		128		; 串口接收缓冲长度
RX2_Length	EQU		128		; 串口接收缓冲长度
RX3_Length	EQU		128		; 串口接收缓冲长度
RX4_Length	EQU		128		; 串口接收缓冲长度

RX1_Buffer	EQU		0x0000	; 接收缓冲首地址
RX2_Buffer	EQU		(RX1_Buffer+RX1_Length)	; 接收缓冲首地址
RX3_Buffer	EQU		(RX2_Buffer+RX2_Length)	; 接收缓冲首地址
RX4_Buffer	EQU		(RX3_Buffer+RX3_Length)	; 接收缓冲首地址

B_TX1_Busy	BIT		20H.0	; 发送忙标志
B_TX2_Busy	BIT		20H.1	; 发送忙标志
B_TX3_Busy	BIT		20H.2	; 发送忙标志
B_TX4_Busy	BIT		20H.3	; 发送忙标志

RX1_write	DATA	30H		; 接收计数
RX2_write	DATA	31H		; 接收计数
RX3_write	DATA	32H		; 接收计数
RX4_write	DATA	33H		; 接收计数
TX1_read	DATA	34H
TX2_read	DATA	35H
TX3_read	DATA	36H
TX4_read	DATA	37H


STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;*******************************************************************
;*******************************************************************
		ORG		00H				;reset
		LJMP	F_Main

		ORG		23H				;4  UART1 interrupt
		LJMP	F_UART1_Interrupt

		ORG		43H				;8  UART2 interrupt
		LJMP	F_UART2_Interrupt

		ORG		8BH				;17  UART3 interrupt
		LJMP	F_UART3_Interrupt

		ORG		93H				;18  UART4 interrupt
		LJMP	F_UART4_Interrupt


;******************** 主程序 **************************/
F_Main:
	CLR		A
	MOV		P0M1, A		; 设置为准双向口
	MOV		P0M0, A
	MOV		P1M1, A		; 设置为准双向口
	MOV		P1M0, A
	MOV		P2M1, A		; 设置为准双向口
	MOV		P2M0, A
	MOV		P3M1, A		; 设置为准双向口
	MOV		P3M0, A
	MOV		P4M1, A		; 设置为准双向口
	MOV		P4M0, A
	MOV		P5M1, A		; 设置为准双向口
	MOV		P5M0, A
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	MOV		A, #1
	LCALL	F_UART1_config
	MOV		A, #2
	LCALL	F_UART2_config
	MOV		A, #3
	LCALL	F_UART3_config
	MOV		A, #4
	LCALL	F_UART4_config

	SETB	EA		; 允许全局中断

;================= 串口发送提示信息 ====================================
	MOV		DPTR, #TestString1	;Load string address to DPTR
	LCALL	F_SendString1		;Send string

	MOV		DPTR, #TestString2	;Load string address to DPTR
	LCALL	F_SendString2		;Send string

	MOV		DPTR, #TestString3	;Load string address to DPTR
	LCALL	F_SendString3		;Send string

	MOV		DPTR, #TestString4	;Load string address to DPTR
	LCALL	F_SendString4		;Send string


;=================== 主循环 ==================================
L_MainLoop:

	MOV		A, TX1_read
	XRL		A, RX1_write
	JZ		L_QuitCheckRx1		;read与write相等, 则没有收到数据, 退出
	JB		B_TX1_Busy, L_QuitCheckRx1		; 发送忙， 退出
	
	SETB	B_TX1_Busy			; 标志发送忙
	MOV		DPTR, #RX1_Buffer
	MOV		A, TX1_read
	LCALL	F_GetRxData
	MOV		SBUF, A		; 发一个字节
	INC		TX1_read
	MOV		A, TX1_read
	CJNE	A, #RX1_Length, L_QuitCheckRx1
	MOV		TX1_read, #0		; 避免溢出处理
L_QuitCheckRx1:

	MOV		A, TX2_read
	XRL		A, RX2_write
	JZ		L_QuitCheckRx2		;read与write相等, 则没有收到数据, 退出
	JB		B_TX2_Busy, L_QuitCheckRx2		; 发送忙， 退出
	
	SETB	B_TX2_Busy			; 标志发送忙
	MOV		DPTR, #RX2_Buffer
	MOV		A, TX2_read
	LCALL	F_GetRxData
	MOV		S2BUF, A		; 发一个字节
	INC		TX2_read
	MOV		A, TX2_read
	CJNE	A, #RX2_Length, L_QuitCheckRx2
	MOV		TX2_read, #0		; 避免溢出处理
L_QuitCheckRx2:

	MOV		A, TX3_read
	XRL		A, RX3_write
	JZ		L_QuitCheckRx3		;read与write相等, 则没有收到数据, 退出
	JB		B_TX3_Busy, L_QuitCheckRx3		; 发送忙， 退出
	
	SETB	B_TX3_Busy			; 标志发送忙
	MOV		DPTR, #RX3_Buffer
	MOV		A, TX3_read
	LCALL	F_GetRxData
	MOV		S3BUF, A		; 发一个字节
	INC		TX3_read
	MOV		A, TX3_read
	CJNE	A, #RX3_Length, L_QuitCheckRx3
	MOV		TX3_read, #0		; 避免溢出处理
L_QuitCheckRx3:

	MOV		A, TX4_read
	XRL		A, RX4_write
	JZ		L_QuitCheckRx4		;read与write相等, 则没有收到数据, 退出
	JB		B_TX4_Busy, L_QuitCheckRx4		; 发送忙， 退出
	
	SETB	B_TX4_Busy			; 标志发送忙
	MOV		DPTR, #RX4_Buffer
	MOV		A, TX4_read
	LCALL	F_GetRxData
	MOV		S4BUF, A		; 发一个字节
	INC		TX4_read
	MOV		A, TX4_read
	CJNE	A, #RX4_Length, L_QuitCheckRx4
	MOV		TX4_read, #0		; 避免溢出处理
L_QuitCheckRx4:

	LJMP	L_MainLoop
;===================================================================

F_GetRxData:
	ADD		A, DPL
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #0
	MOV		DPH, A
	MOVX	A, @DPTR
	RET


TestString1:    DB  "STC15F4K60S4 USART1 Test Prgramme!",0DH,0AH,0
TestString2:    DB  "STC15F4K60S4 USART2 Test Prgramme!",0DH,0AH,0
TestString3:    DB  "STC15F4K60S4 USART3 Test Prgramme!",0DH,0AH,0
TestString4:    DB  "STC15F4K60S4 USART4 Test Prgramme!",0DH,0AH,0


;----------------------------
;Send a string to UART
;Input: DPTR (address of string)
;Output:None
;----------------------------
F_SendString1:
	CLR		A
	MOVC	A, @A+DPTR		;Get current char
	JZ		L_SendEnd1		;Check the end of the string
	SETB	B_TX1_Busy		;标志发送忙
	MOV		SBUF, A			;发送一个字节
	JB		B_TX1_Busy, $	;等待发送完成
	INC		DPTR 			;increment string ptr
	SJMP	F_SendString1		;Check next
L_SendEnd1:
    RET

F_SendString2:
	CLR		A
	MOVC	A, @A+DPTR		;Get current char
	JZ		L_SendEnd2		;Check the end of the string
	SETB	B_TX2_Busy		;标志发送忙
	MOV		S2BUF, A		;发送一个字节
	JB		B_TX2_Busy, $	;等待发送完成
	INC		DPTR 			;increment string ptr
	SJMP	F_SendString2	;Check next
L_SendEnd2:
    RET

F_SendString3:
	CLR		A
	MOVC	A, @A+DPTR		;Get current char
	JZ		L_SendEnd3		;Check the end of the string
	SETB	B_TX3_Busy		;标志发送忙
	MOV		S3BUF, A		;发送一个字节
	JB		B_TX3_Busy, $	;等待发送完成
	INC		DPTR 			;increment string ptr
	SJMP	F_SendString3	;Check next
L_SendEnd3:
    RET

F_SendString4:
	CLR		A
	MOVC	A, @A+DPTR		;Get current char
	JZ		L_SendEnd4		;Check the end of the string
	SETB	B_TX4_Busy		;标志发送忙
	MOV		S4BUF, A		;发送一个字节
	JB		B_TX4_Busy, $	;等待发送完成
	INC		DPTR 			;increment string ptr
	SJMP	F_SendString4	;Check next
L_SendEnd4:
    RET


;========================================================================
; 函数: F_SetTimer2Baudraye
; 描述: 设置Timer2做波特率发生器。
; 参数: DPTR: Timer2的重装值.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_SetTimer2Baudrate:
	ANL		AUXR, #NOT (1 SHL 4)	; Timer stop
	ANL		AUXR, #NOT (1 SHL 3)	; Timer2 set As Timer 默认
	ORL		AUXR, #(1 SHL 2)		; Timer2 set as 1T mode
	MOV		T2H, DPH
	MOV		T2L, DPL
	ANL		IE2, #NOT (1 SHL 2)		; 禁止中断
	ORL		AUXR, #(1 SHL 4)		; Timer run enable
	RET

;========================================================================
; 函数: F_UART1_config
; 描述: UART1初始化函数。
; 参数: ACC: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART1_config:
	CJNE	A, #2, L_UART1_NotUseT2
	ORL		AUXR, #001H		; S1 BRT Use Timer2;	波特率使用Timer2产生
	MOV		DPTR, #UART1_Baudrate
	LCALL	F_SetTimer2Baudrate
	SJMP	L_SetUART1

L_UART1_NotUseT2:
	CLR		TR1						; Timer Stop	波特率使用Timer1产生
	ANL		AUXR, #NOT 01H			; S1 BRT Use Timer1;
	ORL		AUXR, #(1 SHL 6)		; Timer1 set as 1T mode
	ANL		TMOD, #NOT (1 SHL 6)	; Timer1 set As Timer
	ANL		TMOD, #NOT 0x30			; Timer1_16bitAutoReload;
	MOV		TH1, #HIGH UART1_Baudrate
	MOV		TL1, #LOW  UART1_Baudrate
	CLR		ET1						; 禁止中断
	ANL		INT_CLKO, #NOT 0x02		; 不输出时钟
	SETB	TR1

L_SetUART1:
	MOV		A, SCON
	ANL		A, #03FH
	ORL		A, #(1 SHL 6)	; 8位数据, 1位起始位, 1位停止位, 无校验
	MOV		SCON, A
;	SETB	PS		; 高优先级中断
	SETB	REN		; 允许接收
	SETB	ES		; 允许中断
	ANL		P_SW1, #NOT 0C0H	; UART1 使用P30 P31口	默认
;	ORL		P_SW1, #040H		; UART1 使用P36 P37口
;	ORL		P_SW1, #080H		; UART1 使用P16 P17口	(必须使用内部时钟)

	CLR		A
	MOV		TX1_read, A;
	MOV		RX1_write, A;
	CLR		B_TX1_busy
	RET

;====================================================

;========================================================================
; 函数: F_UART2_config
; 描述: UART2初始化函数。
; 参数: ACC: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART2_config:
	CJNE	A, #2, L_UART2_NotUseT2
	MOV		DPTR, #UART2_Baudrate
	LCALL	F_SetTimer2Baudrate

	ANL		S2CON, #NOT (1 SHL 7)	; 8位数据, 1位起始位, 1位停止位, 无校验
	ORL		S2CON, #(1 SHL 4)		; 允许接收
	ORL		IE2, #1					; 允许中断
	ANL		P_SW2, #NOT 1	; 切换到 P1.0 P1.1
;	ORL		P_SW2, #1		; 切换到 P4.6 P4.7

	CLR		A
	MOV		TX2_read, A;
	MOV		RX2_write, A;
	CLR		B_TX2_busy
L_UART2_NotUseT2:
	RET
	
;========================================================================
; 函数: F_UART3_config
; 描述: UART3初始化函数。
; 参数: ACC: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer3做波特率.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART3_config:
	CJNE	A, #2, L_UART3_NotUseT2
	ANL		S3CON, #NOT (1 SHL 6)	; BRT select Timer2		波特率使用Timer2产生
	MOV		DPTR, #UART3_Baudrate
	LCALL	F_SetTimer2Baudrate
	SJMP	L_SetUART3

L_UART3_NotUseT2:
	ORL		S3CON, # (1 SHL 6);	//BRT select Timer3		波特率使用Timer3产生
	ANL		T4T3M, #0xF0;		//停止计数, 清除控制位
	ANL		IE2,   #NOT (1 SHL 5);	//禁止中断
	ORL		T4T3M, #(1 SHL 1);	//1T
	ANL		T4T3M, #NOT (1 SHL 2);	//定时
	ANL		T4T3M, #NOT  1;		//不输出时钟
	MOV		T3H,   #HIGH UART3_Baudrate
	MOV		T3L,   #LOW  UART3_Baudrate
	ORL		T4T3M, #(1 SHL 3);	//开始运行

L_SetUART3:	
	ANL		S3CON, #NOT  (1<<5);	//禁止多机通讯方式
	ANL		S3CON, #NOT  (1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
	ORL		IE2,   # (1 SHL 3);	//允许中断
	ORL		S3CON, # (1 SHL 4);	//允许接收
	ANL		P_SW2, #NOT  2;		//切换到 P0.0 P0.1
;	ORL		P_SW2, # 2;			//切换到 P5.0 P5.1

	CLR		A
	MOV		TX3_read, A;
	MOV		RX3_write, A;
	CLR		B_TX3_busy
	RET

;========================================================================
; 函数: F_UART4_config
; 描述: UART4初始化函数。
; 参数: ACC: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART4_config:
	CJNE	A, #2, L_UART4_NotUseT2
	ANL		S4CON, #NOT (1 SHL 6)	; BRT select Timer2		波特率使用Timer2产生
	MOV		DPTR, #UART4_Baudrate
	LCALL	F_SetTimer2Baudrate
	SJMP	L_SetUART4

L_UART4_NotUseT2:
	ORL		S4CON, #(1 SHL 6);	//BRT select Timer3		波特率使用Timer3产生
	ANL		T4T3M, #0x0F;		//停止计数, 清除控制位
	ANL		IE2,   #NOT (1 SHL 6);	//禁止中断
	ORL		T4T3M, #(1 SHL 5);	//1T
	ANL		T4T3M, #NOT (1 SHL 6);	//定时
	ANL		T4T3M, #NOT (1 SHL 4);		//不输出时钟
	MOV		T4H,   #HIGH UART4_Baudrate
	MOV		T4L,   #LOW  UART4_Baudrate
	ORL		T4T3M, #(1 SHL 7);	//开始运行
L_SetUART4:	
	ANL		S4CON, #NOT  (1<<5);	//禁止多机通讯方式
	ANL		S4CON, #NOT  (1<<7);	// 8位数据, 1位起始位, 1位停止位, 无校验
	ORL		IE2,   # (1 SHL 4);	//允许中断
	ORL		S4CON, # (1 SHL 4);	//允许接收
	ANL		P_SW2, #NOT  4;		//切换到 P0.2 P0.3
;	ORL		P_SW2, # 4;			//切换到 P5.2 P5.3

	CLR		A
	MOV		TX4_read, A;
	MOV		RX4_write, A;
	CLR		B_TX4_busy
	RET

F_GetWriteAddress:
	ADD		A, DPL
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #0
	MOV		DPH, A
	RET


;********************* UART1中断函数************************
F_UART1_Interrupt:
	PUSH	PSW
	PUSH	ACC
	PUSH	DPH
	PUSH	DPL
	
	JNB		RI, L_QuitUartRx1
	CLR		RI
	MOV		DPTR, #RX1_Buffer
	MOV		A, RX1_write
	LCALL	F_GetWriteAddress
	MOV		A, SBUF
	MOVX	@DPTR, A	;保存一个字节
	INC		RX1_write
	MOV		A, RX1_write
	CJNE	A, #RX1_Length, L_QuitUartRx1
	MOV		RX1_write, #0		; 避免溢出处理
L_QuitUartRx1:

	JNB		TI, L_QuitUartTx1
	CLR		TI
	CLR		B_TX1_Busy		; 清除发送忙标志
L_QuitUartTx1:

	POP		DPL
	POP		DPH
	POP		ACC
	POP		PSW
	RETI


;********************* UART2中断函数************************
F_UART2_Interrupt:
	PUSH	PSW
	PUSH	ACC
	PUSH	DPH
	PUSH	DPL
	
	MOV		A, S2CON
	JNB		ACC.0, L_QuitUartRx2
	ANL		S2CON, #NOT 1;
	MOV		DPTR, #RX2_Buffer
	MOV		A, RX2_write
	LCALL	F_GetWriteAddress
	MOV		A, S2BUF
	MOVX	@DPTR, A	;保存一个字节
	INC		RX2_write
	MOV		A, RX2_write
	CJNE	A, #RX2_Length, L_QuitUartRx2
	MOV		RX2_write, #0		; 避免溢出处理
L_QuitUartRx2:

	MOV		A, S2CON
	JNB		ACC.1, L_QuitUartTx2
	ANL		S2CON, #NOT 2;
	CLR		B_TX2_Busy		; 清除发送忙标志
L_QuitUartTx2:

	POP		DPL
	POP		DPH
	POP		ACC
	POP		PSW
	RETI

;********************* UART3中断函数************************
F_UART3_Interrupt:
	PUSH	PSW
	PUSH	ACC
	PUSH	DPH
	PUSH	DPL
	
	MOV		A, S3CON
	JNB		ACC.0, L_QuitUartRx3
	ANL		S3CON, #NOT 1;
	MOV		DPTR, #RX3_Buffer
	MOV		A, RX3_write
	LCALL	F_GetWriteAddress
	MOV		A, S3BUF
	MOVX	@DPTR, A	;保存一个字节
	INC		RX3_write
	MOV		A, RX3_write
	CJNE	A, #RX3_Length, L_QuitUartRx3
	MOV		RX3_write, #0		; 避免溢出处理
L_QuitUartRx3:

	MOV		A, S3CON
	JNB		ACC.1, L_QuitUartTx3
	ANL		S3CON, #NOT 2;
	CLR		B_TX3_Busy		; 清除发送忙标志
L_QuitUartTx3:

	POP		DPL
	POP		DPH
	POP		ACC
	POP		PSW
	RETI


;********************* UART4中断函数************************
F_UART4_Interrupt:
	PUSH	PSW
	PUSH	ACC
	PUSH	DPH
	PUSH	DPL
	
	MOV		A, S4CON
	JNB		ACC.0, L_QuitUartRx4
	ANL		S4CON, #NOT 1;
	MOV		DPTR, #RX4_Buffer
	MOV		A, RX4_write
	LCALL	F_GetWriteAddress
	MOV		A, S4BUF
	MOVX	@DPTR, A	;保存一个字节
	INC		RX4_write
	MOV		A, RX4_write
	CJNE	A, #RX4_Length, L_QuitUartRx4
	MOV		RX4_write, #0		; 避免溢出处理
L_QuitUartRx4:

	MOV		A, S4CON
	JNB		ACC.1, L_QuitUartTx4
	ANL		S4CON, #NOT 2;
	CLR		B_TX4_Busy		; 清除发送忙标志
L_QuitUartTx4:

	POP		DPL
	POP		DPH
	POP		ACC
	POP		PSW
	RETI



	END




