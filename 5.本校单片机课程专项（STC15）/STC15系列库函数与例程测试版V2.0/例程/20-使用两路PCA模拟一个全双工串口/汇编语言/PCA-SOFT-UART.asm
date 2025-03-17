
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;*************	功能说明	**************

;使用STC15系列的PCA0和PCA1做的模拟串口. PCA0接收(P2.5), PCA1发送(P2.6).

;假定测试芯片的工作频率为22118400Hz. 时钟为5.5296MHZ ~ 35MHZ. 

;波特率高，则时钟也要选高, 优先使用 22.1184MHZ, 11.0592MHZ.

;测试方法: 上位机发送数据,MCU收到数据后原样返回.

;串口固定设置: 1位起始位, 8位数据位, 1位停止位.

;******************************************

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

;UART3_BitTime	EQU		9216	; 1200bps @ 11.0592MHz	UART3_BitTime = (MAIN_Fosc / Baudrate)
;UART3_BitTime	EQU		4608	; 2400bps @ 11.0592MHz
;UART3_BitTime	EQU		2304	; 4800bps @ 11.0592MHz
;UART3_BitTime	EQU		1152	; 9600bps @ 11.0592MHz
;UART3_BitTime	EQU		 576	;19200bps @ 11.0592MHz
;UART3_BitTime	EQU		 288	;38400bps @ 11.0592MHz

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
UART3_BitTime	EQU		384		;57600bps @ 22.1184MHz

;UART3_BitTime	EQU		27648	; 1200bps @ 33.1776MHz
;UART3_BitTime	EQU		13824	; 2400bps @ 33.1776MHz
;UART3_BitTime	EQU		6912	; 4800bps @ 33.1776MHz
;UART3_BitTime	EQU		3456	; 9600bps @ 33.1776MHz
;UART3_BitTime	EQU		1728	;19200bps @ 33.1776MHz
;UART3_BitTime	EQU		864		;38400bps @ 33.1776MHz
;UART3_BitTime	EQU		576		;57600bps @ 33.1776MHz
;UART3_BitTime	EQU		288		;115200bps @ 33.1776MHz

PCA_P12_P11_P10_P37		EQU		(0 SHL 4)
PCA_P34_P35_P36_P37		EQU		(1 SHL 4)
PCA_P24_P25_P26_P27		EQU		(2 SHL 4)
PCA_Mode_Capture		EQU		0
PCA_Mode_SoftTimer		EQU		048H
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

;===================== 模拟串口相关 ===========================
P_RX3 		BIT 	P2.5	; 定义模拟串口接收IO
P_TX3 		BIT 	P2.6	; 定义模拟串口发送IO

Rx3_Ring	BIT		20H.0	; 正在接收标志, 低层程序使用, 用户程序不可见
Tx3_Ting	BIT		20H.1	; 正在发送标志, 用户置1请求发送, 底层发送完成清0
RX3_End		BIT		20H.2	; 接收到一个字节, 用户查询 并清0

Tx3_DAT		DATA	30H		; 发送移位变量, 用户不可见
Rx3_DAT		DATA	31H		; 接收移位变量, 用户不可见
Tx3_BitCnt	DATA	32H		; 发送数据的位计数器, 用户不可见
Rx3_BitCnt	DATA	33H		; 接收数据的位计数器, 用户不可见
Rx3_BUF		DATA	34H		; 接收到的字节, 用户读取
Tx3_BUF		DATA	35H		; 要发送的字节, 用户写入

;=================================================================
Tx3_read	DATA	36H		; 发送读指针
Rx3_write	DATA	37H		; 接收写指针

RX_Lenth	EQU		16		; 接收长度
buf3		EQU		40H		; 40H ~ 4FH	接收缓冲

;*******************************************************************
;*******************************************************************
		ORG		00H				;reset
		LJMP	F_Main

		ORG		3BH				;7  PCA interrupt
		LJMP	F_PCA_Interrupt

;******************** 主程序 **************************/
F_Main:
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	LCALL	F_PCA_Init	;PCA初始化
	SETB	EA
	
	MOV		Tx3_read,  #0
	MOV		Rx3_write, #0
	CLR		Tx3_Ting
	CLR		RX3_End
	CLR		Rx3_Ring
	MOV		Tx3_BitCnt, #0
		
;=================== 主循环 ==================================
L_MainLoop:
	JNB		RX3_End, L_QuitRx3	; 检测是否收到一个字节
	CLR		RX3_End				; 清除标志
	MOV		A, #buf3
	ADD		A, Rx3_write
	MOV		R0, A
	MOV		@R0, Rx3_BUF		; 写入缓冲
	INC		Rx3_write			; 指向下一个位置
	MOV		A, Rx3_write
	CLR		C
	SUBB	A, #RX_Lenth		; 溢出检测
	JC		L_QuitRx3
	MOV		Rx3_write, #0
L_QuitRx3:

	JB		Tx3_Ting, L_QuitTx3	;检测是否发送空闲
	MOV		A, Tx3_read
	XRL		A, Rx3_write
	JZ		L_QuitTx3			;检测是否收到过字符

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
L_QuitTx3:

	LJMP	L_MainLoop

;=================== 主程序结束 ==================================

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

	MOV		CCAPM0, #(PCA_Mode_Capture OR PCA_Fall_Active OR ENABLE)	; 16位下降沿捕捉中断模式

	MOV		CCAPM1, #(PCA_Mode_SoftTimer OR ENABLE)	; 16位软件定时器, 中断模式
	MOV		CCAP1L, #LOW UART3_BitTime				; 将影射寄存器写入捕获寄存器，先写CCAP0L
	MOV		CCAP1H, #HIGH UART3_BitTime				; 后写CCAP0H

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

	JNB		Rx3_Ring, L_Rx3_Start	; 已收到起始位
	DJNZ	Rx3_BitCnt, L_RxBit		; 接收完一帧数据
	
	CLR		Rx3_Ring			; 停止接收
	MOV		Rx3_BUF, Rx3_DAT	; 存储数据到缓冲区
	SETB	RX3_End				;
	MOV		CCAPM0, #(PCA_Mode_Capture OR PCA_Fall_Active OR ENABLE)	; 16位下降沿捕捉中断模式
	SJMP	L_QuitPCA0
	
L_RxBit:
	MOV		A, Rx3_DAT			; 把接收的单b数据 暂存到 RxShiftReg(接收缓冲)
	MOV		C, P_RX3
	RRC		A
	MOV		Rx3_DAT, A
	MOV		A, CCAP0L			;
	ADD		A, #LOW UART3_BitTime	; 数据位时间
	MOV		CCAP0L, A			; 将影射寄存器写入捕获寄存器，先写CCAP0L
	MOV		A, CCAP0H			; 数据位时间
	ADDC	A, #HIGH UART3_BitTime	;
	MOV		CCAP0H, A			; 后写CCAP0H
	SJMP	L_QuitPCA0

L_Rx3_Start:
	MOV		CCAPM0, #(PCA_Mode_SoftTimer OR ENABLE)	; 16位软件定时中断模式
	MOV		A, CCAP0L			; 数据位时间
	ADD		A, #LOW (UART3_BitTime / 2 + UART3_BitTime)	;
	MOV		CCAP0L, A			; 将影射寄存器写入捕获寄存器，先写CCAP0L
	MOV		A, CCAP0H			; 数据位时间
	ADDC	A, #HIGH (UART3_BitTime / 2 + UART3_BitTime)	;
	MOV		CCAP0H, A			; 后写CCAP0H
	SETB	Rx3_Ring			; 标志已收到起始位
	MOV		Rx3_BitCnt, #9		; 初始化接收的数据位数(8个数据位+1个停止位)
L_QuitPCA0:

	;=============== PCA模块1中断 ===================
	JNB		CCF1, L_QuitPCA1	; PCA模块1中断, 16位软件定时中断模式
	CLR		CCF1				; 清PCA模块1中断标志
	MOV		A, CCAP1L			;
	ADD		A, #LOW UART3_BitTime	; 数据位时间
	MOV		CCAP1L, A			; 将影射寄存器写入捕获寄存器，先写CCAP0L
	MOV		A, CCAP1H			; 
	ADDC	A, #HIGH UART3_BitTime	; 数据位时间
	MOV		CCAP1H, A			; 后写CCAP0H

	JNB		Tx3_Ting, L_QuitPCA1	; 不发送, 退出
	MOV		A, Tx3_BitCnt
	JNZ		L_TxData			; 发送计数器为0 表明单字节发送还没开始
	CLR		P_TX3				; 发送开始位
	MOV		Tx3_DAT, Tx3_BUF	; 把缓冲的数据放到发送的buff
	MOV		Tx3_BitCnt, #9		; 发送数据位数 (8数据位+1停止位)
	SJMP	L_QuitPCA1
L_TxData:						; 发送计数器为非0 正在发送数据
	DJNZ	Tx3_BitCnt, L_TxBit	; 发送计数器减为0 表明单字节发送结束
	SETB	P_TX3				; 送停止位数据
	CLR		Tx3_Ting			; 发送停止
	SJMP	L_QuitPCA1
L_TxBit:
	MOV		A, Tx3_DAT			; 把最低位送到 CY(益处标志位)
	RRC		A
	MOV		P_TX3, C			; 发送一个bit数据
	MOV		Tx3_DAT, A
L_QuitPCA1:

	POP		ACC
	POP		PSW

	RETI
	

	END



