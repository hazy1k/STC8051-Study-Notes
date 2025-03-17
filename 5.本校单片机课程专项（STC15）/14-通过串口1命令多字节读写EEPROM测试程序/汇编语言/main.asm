

;---------------------------------------------------------------------
; --- STC MCU International Limited ----------------------------------
; --- STC 1T Series MCU Demo Programme -------------------------------
; --- Mobile: (86)13922805190 ----------------------------------------
; --- Fax: 86-0513-55012956,55012947,55012969 ------------------------
; --- Tel: 86-0513-55012928,55012929,55012966 ------------------------
; --- Web: www.GXWMCU.com --------------------------------------------
; --- QQ:  800003751 -------------------------------------------------
; 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   
;---------------------------------------------------------------------



;*************	本程序功能说明	**************

;通过串口对STC内部自带的EEPROM(FLASH)进行读写测试。

;对FLASH做扇区擦除、写入、读出的操作，命令指定地址。

;默认波特率:  115200,8,N,1. 
;默认主时钟:  22118400HZ.

;串口命令设置: (命令字母不区分大小写)
;	W 0x8000 1234567890  --> 对0x8000地址写入字符1234567890.
;	R 0x8000 10          --> 对0x8000地址读出10个字节数据. 

;注意：为了通用，程序不识别扇区是否有效，用户自己根据具体的型号来决定。

;日期: 2013-5-10

;******************************************/

;/****************************** 用户定义宏 ***********************************/
Fosc_KHZ	EQU	22118	;22118KHZ

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


ISP_EN			EQU		(1 SHL 7)
ISP_SWBS		EQU		(1 SHL 6)
ISP_SWRST		EQU		(1 SHL 5)
;ISP_WAIT_FREQUENCY	EQU	 7	 ;ISP_WAIT_1MHZ	
;ISP_WAIT_FREQUENCY	EQU	 6	 ;ISP_WAIT_2MHZ	
;ISP_WAIT_FREQUENCY	EQU	 5	 ;ISP_WAIT_3MHZ	
;ISP_WAIT_FREQUENCY	EQU	 4	 ;ISP_WAIT_6MHZ	
;ISP_WAIT_FREQUENCY	EQU	 3	 ;ISP_WAIT_12MHZ
;ISP_WAIT_FREQUENCY	EQU	 2	 ;ISP_WAIT_20MHZ
ISP_WAIT_FREQUENCY	EQU	 1	 ;ISP_WAIT_24MHZ
;ISP_WAIT_FREQUENCY	EQU	 0	 ;ISP_WAIT_30MHZ


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

INT_CLKO 	DATA 	0x8F
P_SW1 		DATA	0A2H
IE2   		DATA	0AFH
AUXR		DATA 	08EH
T2H			DATA 	0D6H
T2L			DATA 	0D7H
ISP_DATA  DATA 0C2H
ISP_ADDRH DATA 0C3H
ISP_ADDRL DATA 0C4H
ISP_CMD   DATA 0C5H
ISP_TRIG  DATA 0C6H
ISP_CONTR DATA 0C7H


RX1_Lenth	EQU		32		; 串口接收缓冲长度

B_TX1_Busy	BIT		20H.0	; 发送忙标志

TX1_Cnt		DATA	30H		; 发送计数
RX1_Cnt		DATA	31H		; 接收计数
address_H	DATA	32H
address_L	DATA	33H
length		DATA	34H
RX1_TimeOut	DATA	35H

RX1_Buffer	DATA	40H		; 40 ~ 5FH 接收缓冲
tmp			DATA	60H		; 60~7F

STACK_POIRTER	EQU		0D0H	;堆栈开始地址



;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		0023H				;4  UART1 interrupt
		LJMP	F_UART1_Interrupt


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
	MOV		A, #1
	LCALL	F_UART1_config	; 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	SETB	EA		; 允许全局中断
	
	MOV		DPTR, #TestString1	;Load string address to DPTR
	LCALL	F_SendString1		;Send string
	MOV		DPTR, #TestString1A	;Load string address to DPTR
	LCALL	F_SendString1		;Send string
	MOV		DPTR, #TestString1B	;Load string address to DPTR
	LCALL	F_SendString1		;Send string


;=================== 主循环 ==================================
L_MainLoop:
	MOV		A, #1
	LCALL	F_delay_ms
	MOV		A, RX1_TimeOut		;超时计数
	JZ		L_MainLoop			;为0, 循环
	DJNZ	RX1_TimeOut, L_MainLoop	;-1,不为0循环

	MOV		A, RX1_Cnt
	CLR		C
	SUBB	A, #10
	JC		L_Jump_ProcessErr	; RX1_Cnt < 10 bytes, jump

	MOV		R0, #RX1_Buffer
L_CheckCharLoop:				; 小写转大写
	MOV		A, @R0
	CLR		C
	SUBB	A, #'a'
	JC		L_CheckLessThan_a	;if(RX1_Buffer[i] < 'a'), jump
	MOV		A, @R0
	CLR		C
	SUBB	A, #('z'+1)
	JNC		L_CheckLargeThan_z	;if(RX1_Buffer[i] > 'z'), jump
	MOV		A, @R0
	ADD		A, #('A' - 'a')		; 小写转大写
	MOV		@R0, A
L_CheckLessThan_a:
L_CheckLargeThan_z:
	INC		R0
	CJNE	R0, #(RX1_Buffer+8), L_CheckCharLoop
	SJMP	L_CalculateAddr
	
L_Jump_ProcessErr:
	LJMP	L_Rx1ProcessErr

L_CalculateAddr:	
	LCALL	F_GetAddress			; 计算地址
	JB		F0, L_Jump_ProcessErr	; 地址错误

	MOV		R0, #RX1_Buffer+8
	CJNE	@R0, #' ', L_Jump_ProcessErr	;检查命令是否正确	RX1_Buffer[8] = ' '

	MOV		R0, #RX1_Buffer
	CJNE	@R0, #'R', L_CMD_Not_R	;检查命令是读命令	RX1_Buffer[0] = 'R'
	LCALL	F_GetReadDataLength
	JB		F0, L_Jump_ProcessErr	;长度错误
	MOV		A, length
	JZ		L_Jump_ProcessErr	; 长度错误
	CLR		C
	SUBB	A, #RX1_Lenth+1
	JNC		L_Jump_ProcessErr	; 长度错误
	
	MOV		R0, #RX1_Buffer
	LCALL	F_EEPROM_read_n		;address_H,address_L,R0,length
	MOV		DPTR, #TestString2	;"读出"
	LCALL	F_SendString1
	MOV		A, length
	MOV		B, #100
	DIV		AB
	ADD		A, #'0'
	LCALL	F_SendByte		;发送长度 百位
	MOV		A, #10
	XCH		A, B
	DIV		AB
	ADD		A, #'0'
	LCALL	F_SendByte		;发送长度 十位
	XCH		A,B
	ADD		A, #'0'
	LCALL	F_SendByte		;发送长度 个位
	MOV		DPTR, #TestString3	;"个字节数据如下：\r\n"
	LCALL	F_SendString1
	MOV		R2, length
	MOV		R0, #RX1_Buffer
L_TxDataLoop:
	MOV		A, @R0
	LCALL	F_SendByte		;发送数据
	INC		R0
	DJNZ	R2, L_TxDataLoop
	MOV		A, #0DH
	LCALL	F_SendByte	;回车
	MOV		A, #0AH
	LCALL	F_SendByte	;换行
	LJMP	L_QuitRx1Process

L_CMD_Not_R:
	MOV		R0, #RX1_Buffer
	CJNE	@R0, #'W', L_CMD_Not_W	;检查命令是写命令	RX1_Buffer[0] = 'W'
	MOV		A, RX1_Cnt
	CLR		C
	SUBB	A, #9
	MOV		length, A
	MOV		R0, #RX1_Buffer+9
	LCALL	F_EEPROM_SectorErase	; 擦除一个扇区
	LCALL	F_EEPROM_write_n		; 写N个字节并校验	;address_H,address_L,R0,length
	JB		F0, L_WriteError		; 写入错误, 转
	MOV		DPTR, #TestString4		; "已写入"
	LCALL	F_SendString1
	MOV		A, length
	MOV		B, #100
	DIV		AB
	ADD		A, #'0'
	LCALL	F_SendByte		;写入长度 百位
	MOV		A, #10
	XCH		A, B
	DIV		AB
	ADD		A, #'0'
	LCALL	F_SendByte		;写入长度 十位
	XCH		A,B
	ADD		A, #'0'
	LCALL	F_SendByte		;写入长度 个位
	MOV		DPTR, #TestString5		;"字节数据!\r\n"
	LCALL	F_SendString1
	SJMP	L_QuitRx1Process

L_WriteError:
	MOV		DPTR, #TestString6		;"写入错误!\r\n"
	LCALL	F_SendString1
	SJMP	L_QuitRx1Process

L_AddrNotEmpty:
	MOV		DPTR, #TestString7		;"要写入的地址为非空,不能写入,请先擦除!\r\n"
	LCALL	F_SendString1
	SJMP	L_QuitRx1Process

L_CMD_Not_W:
L_Rx1ProcessErr:
	MOV		DPTR, #TestString7		;"命令错误!\r\n"
	LCALL	F_SendString1

L_QuitRx1Process:
	MOV		RX1_Cnt, #0		;	//清除字节数

	LJMP	L_MainLoop
;===================================================================


;**********************************************

;========================================================================
; 函数: F_CheckData
; 描述: ASCII的数字'0'~'9','A'~'F'转成十六进制的数字 0~9, A~F.
; 参数: ACC: 要转换的数据。
; 返回: F0 = 0 正确,  F0 = 1 错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_CheckData:
	CLR		F0	;0--正确, 1--错误
	MOV		R7, A
	CLR		C
	SUBB	A, #'0'
	JC		L_CheckDataErr	; < '0'
	MOV		A, R7
	CLR		C
	SUBB	A, #'9'+1
	JNC		L_CheckDataChar	; > '9'
	MOV		A, R7
	CLR		C
	SUBB	A, #'0'
	RET
L_CheckDataChar:
	MOV		A,R7
	CLR		C
	SUBB	A, #'A'
	JC		L_CheckDataErr	; < 'A'
	MOV		A, R7
	CLR		C
	SUBB	A, #'F'+1
	JNC		L_CheckDataChar	; > 'F'
	MOV		A, R7
	CLR		C
	SUBB	A, #'A'-10
	RET

L_CheckDataErr:
	SETB	F0	;0--正确, 1--错误
	RET

;========================================================================
; 函数: F_GetAddress
; 描述: 获取要操作的EEPROM的首地址.
; 参数: non.
; 返回: F0 = 0 正确,  F0 = 1 错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_GetAddress:
	MOV		address_H, #0
	MOV		address_L, #0

	MOV		R0, #RX1_Buffer+2
	CJNE	@R0, #'0', L_AddrError	;检查地址是否0X开头
	INC		R0
	CJNE	@R0, #'X', L_AddrError

	MOV		R0, #RX1_Buffer+4
	MOV		A, @R0
	LCALL	F_CheckData
	JB		F0, L_AddrError
	SWAP	A
	MOV		address_H, A
	INC		R0
	MOV		A, @R0
	LCALL	F_CheckData
	JB		F0, L_AddrError
	ORL		A, address_H
	MOV		address_H, A

	INC		R0
	MOV		A, @R0
	LCALL	F_CheckData
	JB		F0, L_AddrError
	SWAP	A
	MOV		address_L, A

	INC		R0
	MOV		A, @R0
	LCALL	F_CheckData
	JB		F0, L_AddrError
	ORL		A, address_L
	MOV		address_L, A
	CLR		F0
	RET
L_AddrError:
	SETB	F0
	RET

;========================================================================
; 函数: F_CheckNumber
; 描述: 获取要读出数据的字节数.
; 参数: non.
; 返回: F0 = 0 正确,  F0 = 1 错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_CheckNumber:
	CLR		F0	;0--正确, 1--错误
	MOV		R7, A
	CLR		C
	SUBB	A, #'0'
	JC		L_CheckNumberErr	; < '0'
	MOV		A, R7
	CLR		C
	SUBB	A, #'9'+1
	JNC		L_CheckNumberErr	; > '9'
	MOV		A, R7
	CLR		C
	SUBB	A, #'0'
	RET
L_CheckNumberErr:
	SETB	F0	;0--正确, 1--错误
	RET

F_GetReadDataLength:
	CLR		F0	;0--正确, 1--错误
	
	MOV		length, #0
	MOV		R2, #9
	MOV		R0, #RX1_Buffer+9
L_GetReadDataLengthLoop:	
	MOV		A, length
	MOV		B, #10
	MUL		AB
	MOV		length, A
	MOV		A, @R0
	LCALL	F_CheckNumber
	JB		F0, L_GetReadDataLengthErr
	ADD		A, length
	MOV		length, A
	INC		R0
	INC		R2
	MOV		A, R2
	CJNE	A, RX1_Cnt, L_GetReadDataLengthLoop
	RET
L_GetReadDataLengthErr:
	RET


TestString1:
    DB  "STC15F2K60S2系列单片机EEPROM测试程序!",0DH,0AH,0
TestString1A:
	DB	"W 0x8000 1234567890  --> 对0x8000地址写入字符1234567890.",0DH,0AH,0
TestString1B:
	DB	"R 0x8000 10          --> 对0x8000地址读出10个字节数据.",0DH,0AH,0
TestString2:
	DB	"读出",0
TestString3:
	DB	"个字节数据如下:",0DH,0AH,0
TestString4:
	DB	"已写入",0
TestString5:
	DB	"字节数据!",0DH,0AH,0
TestString6:
	DB	"写入错误!",0DH,0AH,0
TestString7:
	DB	"命令错误!",0DH,0AH,0

F_SendByte:
	SETB	B_TX1_Busy		;标志发送忙
	MOV		SBUF, A			;发送一个字节
	JB		B_TX1_Busy, $	;等待发送完成
    RET

;========================================================================
; 函数: F_SendString1
; 描述: 串口1发送字符串函数。
; 参数: DPTR: 字符串首地址.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_SendString1:
	CLR		A
	MOVC	A, @A+DPTR		;Get current char
	JZ		L_SendEnd1		;Check the end of the string
	LCALL	F_SendByte		;发送一个字节
	INC		DPTR 			;increment string ptr
	SJMP	F_SendString1		;Check next
L_SendEnd1:
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
F_SetTimer2Baudraye:	; 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
	ANL		AUXR, #NOT (1 SHL 4)	; Timer stop	波特率使用Timer2产生
	ANL		AUXR, #NOT (1 SHL 3)	; Timer2 set As Timer
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
	CJNE	A, #2, L_Uart1NotUseTimer2
	ORL		AUXR, #0x01		; S1 BRT Use Timer2;
	MOV		DPTR, #UART1_Baudrate
	LCALL	F_SetTimer2Baudraye
	SJMP	L_SetupUart1

L_Uart1NotUseTimer2:
	CLR		TR1					; Timer Stop	波特率使用Timer1产生
	ANL		AUXR, #NOT 0x01		; S1 BRT Use Timer1;
	ORL		AUXR, #(1 SHL 6)	; Timer1 set as 1T mode
	ANL		TMOD, #NOT (1 SHL 6); Timer1 set As Timer
	ANL		TMOD, #NOT 0x30		; Timer1_16bitAutoReload;
	MOV		TH1, #HIGH UART1_Baudrate
	MOV		TL1, #LOW  UART1_Baudrate
	CLR		ET1					; 禁止中断
	ANL		INT_CLKO, #NOT 0x02	; 不输出时钟
	SETB	TR1

L_SetupUart1:
	SETB	REN		; 允许接收
	SETB	ES		; 允许中断

	ANL		SCON, #0x3f
	ORL		SCON, #0x40		; UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
;	SETB	PS		; 高优先级中断
	SETB	REN		; 允许接收
	SETB	ES		; 允许中断

	ANL		P_SW1, #0x3f
	ORL		P_SW1, #0x80		; UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (必须使用内部时钟)
;	ORL		PCON2, #(1 SHL 4)	; 内部短路RXD与TXD, 做中继, ENABLE,DISABLE

	CLR		B_TX1_Busy
	MOV		RX1_Cnt, #0;
	MOV		TX1_Cnt, #0;
	RET

;========================================================================
; 函数: F_UART1_Interrupt
; 描述: UART2中断函数。
; 参数: nine.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART1_Interrupt:
	PUSH	PSW
	PUSH	ACC
	PUSH	AR0
	
	JNB		RI, L_QuitUartRx
	CLR		RI
	MOV		RX1_TimeOut, #5
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

	POP		AR0
	POP		ACC
	POP		PSW
	RETI


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
	PUSH	AR2		;入栈R2
	PUSH	AR3		;入栈R3
	PUSH	AR4		;入栈R4

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

	POP		AR4		;出栈R2
	POP		AR3		;出栈R3
	POP		AR2		;出栈R4
	RET


;========================================================================
; 函数: F_DisableEEPROM
; 描述: 禁止访问ISP/IAP.
; 参数: non.
; 返回: non.
; 版本: V1.0, 2012-10-22
;========================================================================
F_DisableEEPROM:
	MOV		ISP_CONTR, #0		; 禁止ISP/IAP操作
	MOV		ISP_CMD,  #0		; 去除ISP/IAP命令
	MOV		ISP_TRIG, #0		; 防止ISP/IAP命令误触发
	MOV		ISP_ADDRH, #0FFH	; 清0地址高字节
	MOV		ISP_ADDRL, #0FFH	; 清0地址低字节，指向非EEPROM区，防止误操作
	RET

;========================================================================
; 函数: F_EEPROM_Trig
; 描述: 触发EEPROM操作.
; 参数: none.
; 返回: none.
; 版本: V1.0, 2014-6-30
;========================================================================
F_EEPROM_Trig:
	MOV		C, EA
	MOV		F0, C			;保存全局中断
	CLR		EA				;禁止中断, 避免触发命令无效
	MOV		ISP_TRIG, #0x5A	;先送5AH，再送A5H到ISP/IAP触发寄存器，每次都需要如此
							;送完A5H后，ISP/IAP命令立即被触发启动.
							;CPU等待IAP完成后，才会继续执行程序.
	MOV		ISP_TRIG, #0xA5
	NOP
	NOP
	MOV		C, F0
	MOV		EA, C		;恢复全局中断
	RET

;========================================================================
; 函数: F_EEPROM_read_n
; 描述: 从指定EEPROM首地址读出n个字节放指定的缓冲.
; 参数: address_H,address_L:  读出EEPROM的首地址.
;       R0:                   读出数据放缓冲的首地址.
;       length:               读出的字节长度.
; 返回: non.
; 版本: V1.0, 2012-10-22
;========================================================================

F_EEPROM_read_n:
	PUSH	AR2

	MOV		R2, length
	MOV		ISP_ADDRH, address_H	; 送地址高字节（地址需要改变时才需重新送地址）
	MOV		ISP_ADDRL, address_L	; 送地址低字节
	MOV		ISP_CONTR, #(ISP_EN + ISP_WAIT_FREQUENCY)	; 设置等待时间，允许ISP/IAP操作，送一次就够
	MOV		ISP_CMD, #1				; ISP读出命令送字节读命令，命令不需改变时，不需重新送命令
L_EE_Read_Loop:
	LCALL	F_EEPROM_Trig			; 触发EEPROM操作
	MOV		@R0, ISP_DATA

	MOV		A, ISP_ADDRL			; 地址 +1
	ADD		A, #1
	MOV		ISP_ADDRL, A
	MOV		A, ISP_ADDRH
	ADDC	A, #0
	MOV		ISP_ADDRH, A
	INC		R0
	DJNZ	R2, L_EE_Read_Loop

	LCALL	F_DisableEEPROM
	POP		AR2
	RET


;========================================================================
; 函数: F_EEPROM_SectorErase
; 描述: 把指定地址的EEPROM扇区擦除.
; 参数: address_H,address_L:  要擦除的扇区EEPROM的地址.
; 返回: non.
; 版本: V1.0, 2013-5-10
;========================================================================
F_EEPROM_SectorErase:
											;只有扇区擦除，没有字节擦除，512字节/扇区。
											;扇区中任意一个字节地址都是扇区地址。
	MOV		ISP_ADDRH, address_H	; 送地址高字节（地址需要改变时才需重新送地址）
	MOV		ISP_ADDRL, address_L	; 送地址低字节
	MOV		ISP_CONTR, #(ISP_EN + ISP_WAIT_FREQUENCY)	; 设置等待时间，允许ISP/IAP操作，送一次就够
	MOV		ISP_CMD, #3				;送扇区擦除命令
	LCALL	F_EEPROM_Trig			; 触发EEPROM操作
	LCALL	F_DisableEEPROM			; 禁止EEPROM操作
	RET

;========================================================================
; 函数: F_EEPROM_write_n
; 描述: 把缓冲的n个字节写入指定首地址的EEPROM, 并且校验.
; 参数: address_H,address_L:	写入EEPROM的首地址.
;       R0: 					写入源数据的缓冲的首地址.
;       length:      			写入的字节长度.
; 返回: F0 == 0, 写入正确,  F0 == 1, 写入错误.
; 版本: V1.0, 2014-1-22
;========================================================================
F_EEPROM_write_n:
	PUSH	AR2
	PUSH	AR0
	MOV		R2, length
	MOV		ISP_ADDRH, address_H	; 送地址高字节（地址需要改变时才需重新送地址）
	MOV		ISP_ADDRL, address_L	; 送地址低字节
	MOV		ISP_CONTR, #(ISP_EN + ISP_WAIT_FREQUENCY)	; 设置等待时间，允许ISP/IAP操作，送一次就够
	MOV		ISP_CMD, #2				;送字节写命令，命令不需改变时，不需重新送命令
L_EE_W_Loop:
	MOV		ISP_DATA, @R0			; 送数据到ISP_DATA，只有数据改变时才需重新送
	LCALL	F_EEPROM_Trig			; 触发EEPROM操作
	MOV		A, ISP_ADDRL			;地址 +1
	ADD		A, #1
	MOV		ISP_ADDRL, A
	MOV		A, ISP_ADDRH
	ADDC	A, #0
	MOV		ISP_ADDRH, A
	INC		R0
	DJNZ	R2, L_EE_W_Loop

	POP		AR0
	MOV		R2, length				; 写入后比较
	MOV		ISP_ADDRH, address_H	; 送地址高字节（地址需要改变时才需重新送地址）
	MOV		ISP_ADDRL, address_L	; 送地址低字节
	MOV		ISP_CONTR, #(ISP_EN + ISP_WAIT_FREQUENCY)	; 设置等待时间，允许ISP/IAP操作，送一次就够
	MOV		ISP_CMD, #1		; ISP读出命令送字节读命令，命令不需改变时，不需重新送命令
L_EE_Compare_Loop:
	LCALL	F_EEPROM_Trig			; 触发EEPROM操作
	MOV		A, ISP_DATA
	XRL		A, @R0
	JNZ		L_EE_CompareErr

	MOV		A, ISP_ADDRL	;
	ADD		A, #1
	MOV		ISP_ADDRL, A
	MOV		A, ISP_ADDRH
	ADDC	A, #0
	MOV		ISP_ADDRH, A
	INC		R0
	DJNZ	R2, L_EE_Compare_Loop

	LCALL	F_DisableEEPROM
	CLR		F0
	POP		AR2
	RET

L_EE_CompareErr:
	LCALL	F_DisableEEPROM
	SETB	F0
	POP		AR2
	RET


	END

	