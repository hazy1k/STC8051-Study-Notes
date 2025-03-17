
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;*************	功能说明	**************

;使用STC15系列的Timer2做的模拟串口. P3.0接收, P3.1发送, 半双工.

;假定测试芯片的工作频率为22118400Hz. 时钟为5.5296MHZ ~ 35MHZ. 

;波特率高，则时钟也要选高, 优先使用 22.1184MHZ, 11.0592MHZ.

;测试方法: 上位机发送数据, MCU收到数据后原样返回.

;串口固定设置: 1位起始位, 8位数据位, 1位停止位,  波特率在范围如下.

;******************************************

;UART3_BitTime	EQU		9216	; 1200bps @ 11.0592MHz	UART3_BitTime = (MAIN_Fosc / Baudrate)
;UART3_BitTime	EQU		4608	; 2400bps @ 11.0592MHz
;UART3_BitTime	EQU		2304	; 4800bps @ 11.0592MHz
;UART3_BitTime	EQU		1152	; 9600bps @ 11.0592MHz
;UART3_BitTime	EQU		 576	;19200bps @ 11.0592MHz
;UART3_BitTime	EQU		 288	;38400bps @ 11.0592MHz
;UART3_BitTime	EQU		 192	;57600bps @ 11.0592MHz

;UART3_BitTime	EQU		15360	; 1200bps @ 18.432MHz
;UART3_BitTime	EQU		7680	; 2400bps @ 18.432MHz
;UART3_BitTime	EQU		3840	; 4800bps @ 18.432MHz
;UART3_BitTime	EQU		1920	; 9600bps @ 18.432MHz
;UART3_BitTime	EQU		960		;19200bps @ 18.432MHz
;UART3_BitTime	EQU		480		;38400bps @ 18.432MHz
;UART3_BitTime	EQU		320		;57600bps @ 18.432MHz

;UART3_BitTime	EQU		18432	; 1200bps @ 22.1184MHz
;UART3_BitTime	EQU		9216	; 2400bps @ 22.1184MHz
;UART3_BitTime	EQU		4608	; 4800bps @ 22.1184MHz
;UART3_BitTime	EQU		2304	; 9600bps @ 22.1184MHz
;UART3_BitTime	EQU		1152	;19200bps @ 22.1184MHz
;UART3_BitTime	EQU		576		;38400bps @ 22.1184MHz
;UART3_BitTime	EQU		384		;57600bps @ 22.1184MHz
UART3_BitTime	EQU		192		;115200bps @ 22.1184MHz

;UART3_BitTime	EQU		27648	; 1200bps @ 33.1776MHz
;UART3_BitTime	EQU		13824	; 2400bps @ 33.1776MHz
;UART3_BitTime	EQU		6912	; 4800bps @ 33.1776MHz
;UART3_BitTime	EQU		3456	; 9600bps @ 33.1776MHz
;UART3_BitTime	EQU		1728	;19200bps @ 33.1776MHz
;UART3_BitTime	EQU		864		;38400bps @ 33.1776MHz
;UART3_BitTime	EQU		576		;57600bps @ 33.1776MHz
;UART3_BitTime	EQU		288		;115200bps @ 33.1776MHz

IE2			DATA 	0AFH
AUXR		DATA 	08EH
INT_CLKO	DATA 	08FH
T2H			DATA 	0D6H
T2L			DATA 	0D7H

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

;===================== 模拟串口相关 ===========================
P_RX3 		BIT 	P3.0	; 定义模拟串口接收IO
P_TX3 		BIT 	P3.1	; 定义模拟串口发送IO

Rx3_Ring	BIT		20H.0	; 正在接收标志, 低层程序使用, 用户程序不可见
Tx3_Ting	BIT		20H.1	; 正在发送标志, 用户置1请求发送, 底层发送完成清0
RX3_End		BIT		20H.2	; 接收到一个字节, 用户查询 并清0
B_RxOk		BIT		20H.3	; 接收结束标志

Tx3_DAT		DATA	30H		; 发送移位变量, 用户不可见
Rx3_DAT		DATA	31H		; 接收移位变量, 用户不可见
Tx3_BitCnt	DATA	32H		; 发送数据的位计数器, 用户不可见
Rx3_BitCnt	DATA	33H		; 接收数据的位计数器, 用户不可见
Rx3_BUF		DATA	34H		; 接收到的字节, 用户读取
Tx3_BUF		DATA	35H		; 要发送的字节, 用户写入

;=================================================================
RxTimeOutH	DATA	36H		;
RxTimeOutL	DATA	37H		;

Tx3_read	DATA	38H		; 发送读指针
Rx3_write	DATA	39H		; 接收写指针

RX_Lenth	EQU		32		; 接收长度
buf3		EQU		40H		; 40H ~ 5FH	接收缓冲

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		0063H				;12  Timer2 interrupt
		LJMP	F_Timer2_Interrupt

		ORG		0083H				;16  INT4 interrupt
		LJMP	F_INT4_Interrupt

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
	LCALL	F_UART_Init		;UART初始化
	SETB	EA
	
		
;=================== 主循环 ==================================
L_MainLoop:
	JNB		RX3_End, L_QuitRx3	; 检测是否收到一个字节
	CLR		RX3_End				; 清除标志
	MOV		A, #buf3
	ADD		A, Rx3_write
	MOV		R0, A
	MOV		@R0, Rx3_BUF		; 写入缓冲
	MOV		RxTimeOutH, #HIGH 1000	; 装载超时时间
	MOV		RxTimeOutL, #LOW  1000	;
	INC		Rx3_write			; 指向下一个位置
	MOV		A, Rx3_write
	CLR		C
	SUBB	A, #RX_Lenth		; 溢出检测
	JC		L_QuitRx3
	MOV		Rx3_write, #0
L_QuitRx3:

	MOV		A, RxTimeOutL
	ORL		A, RxTimeOutH
	JZ		L_QuitTimeOut	; 超时时间是否非0?
	MOV		A, RxTimeOutL
	DEC		RxTimeOutL		; (超时时间  - 1) == 0?
	JNZ		$+4
	DEC		RxTimeOutH
	DEC		A
	ORL		A, RxTimeOutH
	JNZ		L_QuitTimeOut
	
	SETB	B_RxOk						; 超时, 标志接收完成
	ANL		AUXR, #NOT (1 SHL 4)		; Timer2 停止运行
	ANL		INT_CLKO, #NOT (1 SHL 6)	; 禁止INT4中断
	MOV		T2H, #HIGH (65536 - UART3_BitTime)	;数据位
	MOV		T2L, #LOW (65536 - UART3_BitTime)	;
	ORL		AUXR, #(1 SHL 4)			; Timer2 开始运行
L_QuitTimeOut:	

	JNB		B_RxOk, L_QuitTx3	; 检测是否接收OK?
	JB		Tx3_Ting, L_QuitTx3	; 检测是否发送空闲
	MOV		A, Tx3_read
	XRL		A, Rx3_write
	JZ		L_TxFinish			; 检测是否发送完毕

	MOV		A, #buf3
	ADD		A, Tx3_read
	MOV		R0, A
	MOV		Tx3_BUF, @R0		; 从缓冲读一个字符发送
	SETB	Tx3_Ting			; 设置发送标志
	INC		Tx3_read			; 指向下一个字符位置
	MOV		A, Tx3_read
	CLR		C
	SUBB	A, #RX_Lenth
	JC		L_QuitTx3			; 溢出检测
	MOV		Tx3_read, #0
	SJMP	L_QuitTx3

L_TxFinish:
	CLR		B_RxOk
	ANL		AUXR, #NOT (1 SHL 4)	; Timer2 停止运行
	ORL		INT_CLKO, #(1 SHL 6)	; 允许INT4中断

L_QuitTx3:

	LJMP	L_MainLoop

;=================== 主程序结束 ==================================

;========================================================================
; 函数: F_UART_Init
; 描述: UART初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_UART_Init:
	MOV		Tx3_read,  #0
	MOV		Rx3_write, #0
	CLR		Tx3_Ting
	CLR		RX3_End
	CLR		Rx3_Ring
	MOV		Tx3_BitCnt, #0

	MOV		RxTimeOutH, #0
	MOV		RxTimeOutL, #0
	CLR		B_RxOk

	ANL		AUXR, #NOT(1 SHL 4)		; Timer2 停止运行
	MOV		T2H, #HIGH (65536 - UART3_BitTime)	; 数据位
	MOV		T2L, #LOW (65536 - UART3_BitTime)	; 数据位
	ORL		INT_CLKO, #(1 SHL 6)	; 允许INT4中断
	ORL		IE2, #(1 SHL 2)			; 允许Timer2中断
	ORL		AUXR, #(1 SHL 2)		; 1T模式
	RET

;======================================================================

;========================================================================
; 函数: void	F_Timer2_Interrupt
; 描述: Timer2中断处理程序.
; 参数: None
; 返回: none.
; 版本: V1.0, 2012-11-22
;========================================================================
F_Timer2_Interrupt:
	PUSH	PSW
	PUSH	ACC

	JNB		Rx3_Ring, L_QuitRx	; 已收到起始位
	DJNZ	Rx3_BitCnt, L_RxBit	; 接收完一帧数据
	
	CLR		Rx3_Ring			; 停止接收
	MOV		Rx3_BUF, Rx3_DAT	; 存储数据到缓冲区
	SETB	RX3_End				;
	ANL		AUXR, #NOT (1 SHL 4); Timer2 停止运行
	ORL		INT_CLKO, #(1 SHL 6); 允许INT4中断
	SJMP	L_QuitRx
	
L_RxBit:
	MOV		A, Rx3_DAT			; 把接收的单b数据 暂存到 RxShiftReg(接收缓冲)
	MOV		C, P_RX3
	RRC		A
	MOV		Rx3_DAT, A
L_QuitRx:

	JNB		Tx3_Ting, L_QuitTx	; 不发送, 退出
	MOV		A, Tx3_BitCnt
	JNZ		L_TxData			; 发送计数器为0 表明单字节发送还没开始
	CLR		P_TX3				; 发送开始位
	MOV		Tx3_DAT, Tx3_BUF	; 把缓冲的数据放到发送的buff
	MOV		Tx3_BitCnt, #9		; 发送数据位数 (8数据位+1停止位)
	SJMP	L_QuitTx
L_TxData:						; 发送计数器为非0 正在发送数据
	DJNZ	Tx3_BitCnt, L_TxBit	; 发送计数器减为0 表明单字节发送结束
	SETB	P_TX3				; 送停止位数据
	CLR		Tx3_Ting			; 发送停止
	SJMP	L_QuitTx
L_TxBit:
	MOV		A, Tx3_DAT			; 把最低位送到 CY(益处标志位)
	RRC		A
	MOV		P_TX3, C			; 发送一个bit数据
	MOV		Tx3_DAT, A
L_QuitTx:

	POP		ACC
	POP		PSW

	RETI
	

;===================== INT4中断函数 ==================================
F_INT4_Interrupt:
	PUSH	PSW
	PUSH	ACC

	ANL		AUXR, #NOT(1 SHL 4)	;Timer2 停止运行
	MOV		T2H, #HIGH (65536 - (UART3_BitTime / 2 + UART3_BitTime))	; 起始位 + 半个数据位
	MOV		T2L, #LOW (65536 - (UART3_BitTime / 2 + UART3_BitTime))		; 起始位 + 半个数据位
	ORL		AUXR, #(1 SHL 4)	;Timer2 开始运行
	SETB	Rx3_Ring			; 标志已收到起始位
	MOV		Rx3_BitCnt, #9		; 初始化接收的数据位数(8个数据位+1个停止位)
	
	ANL		INT_CLKO,  #NOT(1 SHL 6);	//禁止INT4中断
	MOV		T2H, #HIGH (65536 - UART3_BitTime)	; 数据位
	MOV		T2L, #LOW (65536 - UART3_BitTime)	; 数据位

	POP		ACC
	POP		PSW
	RETI

	END



