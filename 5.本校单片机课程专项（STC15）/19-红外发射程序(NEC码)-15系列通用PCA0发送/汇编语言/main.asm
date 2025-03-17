
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

;用户可以在宏定义中改变MCU主时钟频率. 范围 8MHZ ~ 33MHZ.

;红外接收程序。模拟市场上用量最大的NEC的编码。

;用户可以在宏定义中指定用户码.

;使用PCA2高速输出产生38KHZ载波, 1/3占空比, 每个38KHZ周期发射管发射9us,关闭16.3us.

;使用开发板上的16个IO扫描按键, MCU不睡眠, 连续扫描按键.

;当键按下, 第一帧为数据, 后面的帧为重复争,不带数据, 具体定义请自行参考NEC的编码资料.

;键释放后, 停止发送.

;******************************************/

;/****************************** 用户定义宏 ***********************************/

STACK_POIRTER	EQU		0D0H	;堆栈开始地址
Fosc_KHZ		EQU		22118	;22118KHZ

//Fosc = 22.1184MHZ, 当改变频率时, 请手工计算下面的 D_38K_DUTY 和 D_38K_ON 数值.
D_38K_DUTY	EQU	581							;=(Fosc(MHZ) * 26.3)	/* 	38KHZ周期时间	26.3us */
D_38K_ON	EQU	199							;=(Fosc(MHZ) * 9)		/* 发射管导通时间	9us */
D_38K_OFF	EQU	(D_38K_DUTY - D_38K_ON)		;=(Fosc(MHZ) * 17.3)	/* 发射管关闭时间	17.3us */

;*******************************************************************
;*******************************************************************


PCA_PWM0 DATA 0F2H	;	//PCA模块0 PWM寄存器。
PCA_PWM1 DATA 0F3H	;	//PCA模块1 PWM寄存器。
PCA_PWM2 DATA 0F4H	;	//PCA模块2 PWM寄存器。

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

;*******************************************************************
AUXR		DATA 08EH
P4			DATA 0C0H
P5			DATA 0C8H
ADC_CONTR	DATA 0BCH	;带AD系列
ADC_RES		DATA 0BDH	;带AD系列
ADC_RESL	DATA 0BEH	;带AD系列
P1ASF		DATA 09DH
PCON2		DATA 097H

P_SW1 		DATA	0A2H
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

;*************	IO口定义	**************/
P_HC595_SER   BIT P4.0	;	//pin 14	SER		data input
P_HC595_RCLK  BIT P5.4	;	//pin 12	RCLk	store (latch) clock
P_HC595_SRCLK BIT P4.3	;	//pin 11	SRCLK	Shift data clock

;*************	本地变量声明	**************/

IO_KeyState		DATA	30H	; IO行列键状态变量
IO_KeyState1	DATA	31H
IO_KeyHoldCnt	DATA	32H	; IO键按下计时
KeyCode			DATA	33H	; 给用户使用的键码, 1~16为ADC键， 17~32为IO键


/*************	红外发送相关变量	**************/
#define	User_code	0xFF00		//定义红外用户码

P_IR_TX   BIT P3.7	;定义红外发送端口

FLAG0	DATA	0x20
B_Space	BIT	FLAG0.0	;发送空闲(延时)标志

PCA_Timer0H	DATA	0x34	;PCA2软件定时器变量
PCA_Timer0L	DATA	0x35
tx_cntH		DATA	0x36	;发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
tx_cntL		DATA	0x37	
TxTime		DATA	0x38	;发送时间


;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		003BH				;7  PCA interrupt
		LJMP	F_PCA_Interrupt


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

	LCALL	F_PCA_Init		;初始化PCA2
	SETB	P_IR_TX
	
	SETB	EA				;打开总中断
	
	LCALL	F_DisableHC595	;禁止掉学习板上的HC595显示，省电

	MOV		KeyCode, #0

;=================== 主循环 ==================================
L_Main_Loop:
	MOV		A, #30
	LCALL	F_delay_ms		;延时30ms
	LCALL	F_IO_KeyScan	;扫描键盘

	MOV		A, KeyCode
	JZ		L_Main_Loop		;无键循环
	MOV		TxTime, #0		;
							;一帧数据最小长度 = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
							;一帧数据最大长度 = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
	MOV		tx_cntH, #HIGH 342		;对应9ms，同步头		9ms
	MOV		tx_cntL, #LOW  342
	LCALL	F_IR_TxPulse

	MOV		tx_cntH, #HIGH 171		;对应4.5ms，同步头间隔	4.5ms
	MOV		tx_cntL, #LOW  171
	LCALL	F_IR_TxSpace

	MOV		tx_cntH, #HIGH 21		;发送脉冲			0.5625ms
	MOV		tx_cntL, #LOW  21
	LCALL	F_IR_TxPulse

	MOV		A, #LOW  User_code	;发用户码低字节
	LCALL	F_IR_TxByte
	MOV		A, #HIGH User_code	;发用户码高字节
	LCALL	F_IR_TxByte
	MOV		A, KeyCode			;发数据
	LCALL	F_IR_TxByte
	MOV		A, KeyCode			;发数据反码
	CPL		A
	LCALL	F_IR_TxByte

	MOV		A, TxTime
	CLR		C
	SUBB	A, #56		;一帧按最大77ms发送, 不够的话,补偿时间		108ms
	JNC		L_ADJ_Time

	CLR		c
	MOV		A, #56
	SUBB	A, TxTime
	MOV		TxTime, A
	RRC		A
	RRC		A
	RRC		A
	ANL		A, #0x1F
	ADD		A, TxTime
	LCALL	F_delay_ms		;TxTime = 56 - TxTime;	TxTime = TxTime + TxTime / 8;	delay_ms(TxTime);
L_ADJ_Time:
	MOV		A, #31			;delay_ms(31)
	LCALL	F_delay_ms
		
L_WaitKeyRelease:
	MOV		A, IO_KeyState
	JZ		L_ClearKeyCode

							;键未释放, 发送重复帧(无数据)
	MOV		tx_cntH, #HIGH 342		;对应9ms，同步头		9ms
	MOV		tx_cntL, #LOW  342
	LCALL	F_IR_TxPulse

	MOV		tx_cntH, #HIGH 86		;对应2.25ms，同步头间隔	2.25ms
	MOV		tx_cntL, #LOW  86
	LCALL	F_IR_TxSpace

	MOV		tx_cntH, #HIGH 21		;发送脉冲			0.5625ms
	MOV		tx_cntL, #LOW  21
	LCALL	F_IR_TxPulse

	MOV		A, #96			;delay_ms(96)
	LCALL	F_delay_ms
	LCALL	F_IO_KeyScan	;扫描键盘
	SJMP	L_WaitKeyRelease

L_ClearKeyCode:
	MOV	KeyCode, #0

	LJMP	L_Main_Loop
;===================================================================


;========================================================================
; 函数: F_DisableHC595
; 描述: 禁止HC595显示.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
;========================================================================
F_DisableHC595:
	PUSH	AR7
	SETB	P_HC595_SER
	MOV		R7, #20
L_DisableHC595_Loop:
	SETB	P_HC595_SRCLK
	NOP
	CLR		P_HC595_SRCLK
	NOP
	DJNZ	R7, L_DisableHC595_Loop
	SETB	P_HC595_RCLK = 1;
	NOP
	CLR		P_HC595_RCLK = 0;							//锁存输出数据
	SETB	P_HC595_RCLK = 1;
	NOP
	CLR		P_HC595_RCLK = 0;							//锁存输出数据
	POP		AR7
	RET


;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
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


;========================================================================
; 函数: F_IO_KeyDelay
; 描述: 行列键扫描程序.
; 参数: none
; 返回: 读到按键, KeyCode为非0键码.
; 版本: V1.0, 2013-11-22
;========================================================================
;/*****************************************************
;	行列键扫描程序
;	使用XY查找4x4键的方法，只能单键，速度快
;
;   Y     P04      P05      P06      P07
;          |        |        |        |
;X         |        |        |        |
;P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
;          |        |        |        |
;P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
;          |        |        |        |
;P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
;          |        |        |        |
;P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
;          |        |        |        |
;******************************************************/


T_KeyTable:  DB 0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0

F_IO_KeyDelay:
	PUSH	03H		;R3入栈
	MOV		R3, #60
	DJNZ	R3, $	; (n * 4) T
	POP		03H		;R3出栈
	RET

F_IO_KeyScan:
	PUSH	06H		;R6入栈
	PUSH	07H		;R7入栈

	MOV		R6, IO_KeyState1	; 保存上一次状态

	MOV		P0, #0F0H		;X低，读Y
	LCALL	F_IO_KeyDelay		;delay about 250T
	MOV		A, P0
	ANL		A, #0F0H
	MOV		IO_KeyState1, A		; IO_KeyState1 = P0 & 0xf0

	MOV		P0, #0FH		;Y低，读X
	LCALL	F_IO_KeyDelay		;delay about 250T
	MOV		A, P0
	ANL		A, #0FH
	ORL		A, IO_KeyState1			; IO_KeyState1 |= (P0 & 0x0f)
	MOV		IO_KeyState1, A
	XRL		IO_KeyState1, #0FFH		; IO_KeyState1 ^= 0xff;	取反

	MOV		A, R6					;if(j == IO_KeyState1),	连续两次读相等
	CJNE	A, IO_KeyState1, L_QuitCheckIoKey	;不相等, jmp
	
	MOV		R6, IO_KeyState		;暂存IO_KeyState
	MOV		IO_KeyState, IO_KeyState1
	MOV		A, IO_KeyState
	JZ		L_NoIoKeyPress		; if(IO_KeyState != 0), 有键按下

	MOV		A, R6	
	JZ		L_CalculateIoKey	;if(R6 == 0)	F0 = 1;	第一次按下
	MOV		A, R6	
	CJNE	A, IO_KeyState,	L_QuitCheckIoKey	; if(j != IO_KeyState), jmp
	
	INC		IO_KeyHoldCnt	; if(++IO_KeyHoldCnt >= 20),	1秒后重键
	MOV		A, IO_KeyHoldCnt
	CJNE	A, #20, L_QuitCheckIoKey
	MOV		IO_KeyHoldCnt, #18;
L_CalculateIoKey:
	MOV		A, IO_KeyState
	SWAP	A		;R6 = T_KeyTable[IO_KeyState >> 4];
	ANL		A, #0x0F
	MOV		DPTR, #T_KeyTable
	MOVC	A, @A+DPTR
	MOV		R6, A
	
	JZ		L_QuitCheckIoKey	; if(R6 == 0)
	MOV		A, IO_KeyState
	ANL		A, #0x0F
	MOVC	A, @A+DPTR
	MOV		R7, A
	JZ		L_QuitCheckIoKey	; if(T_KeyTable[IO_KeyState& 0x0f] == 0)
	
	MOV		A, R6		;KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;	//计算键码，17~32
	ADD		A, ACC
	ADD		A, ACC
	MOV		R6, A
	MOV		A, R7
	ADD		A, R6
	ADD		A, #12
	MOV		KeyCode, A
	SJMP	L_QuitCheckIoKey
	
L_NoIoKeyPress:
	MOV		IO_KeyHoldCnt, #0

L_QuitCheckIoKey:
	MOV		P0, #0xFF
	POP		07H		;R7出栈
	POP		06H		;R6出栈
	RET

;========================================================================
; 函数: F_IR_TxPulse
; 描述: 发送脉冲函数.
; 参数: tx_cntH, tx_cntL: 要发送的38K周期数
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_IR_TxPulse:
	CLR		B_Space	;标志发脉冲
	MOV		CCAPM2, #(0x48 + 0x04 + 0x01)	; 工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	SETB	CR		;启动
	JB		CR, $	;等待结束
	SETB	P_IR_TX
	RET

;========================================================================
; 函数: F_IR_TxSpace
; 描述: 发送空闲函数.
; 参数: tx_cntH, tx_cntL: 要发送的38K周期数
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_IR_TxSpace:
	SETB	B_Space	;标志空闲
	MOV		CCAPM2, #(0x48 + 0x01)	; 工作模式 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	SETB	CR		;启动
	JB		CR, $	;等待结束
	SETB	P_IR_TX
	RET


;========================================================================
; 函数: F_IR_TxByte
; 描述: 发送一个字节函数.
; 参数: ACC: 要发送的字节
; 返回: none.
; 版本: V1.0, 2013-11-22
;========================================================================
F_IR_TxByte:
	PUSH	AR4
	PUSH	AR5

	MOV		R4, #8
	MOV		R5, A
L_IR_TxByteLoop:
	MOV		A, R5
	JNB		ACC.0, L_IR_TxByte_0
	MOV		tx_cntH, #HIGH 63		;发送数据1
	MOV		tx_cntL, #LOW  63
	LCALL	F_IR_TxSpace
	INC		TxTime			;TxTime += 2;	//数据1对应 1.6875 + 0.5625 ms
	INC		TxTime
	SJMP	L_IR_TxByte_Pause
L_IR_TxByte_0:
	MOV		tx_cntH, #HIGH 21		;发送数据0
	MOV		tx_cntL, #LOW  21
	LCALL	F_IR_TxSpace
	INC		TxTime			;数据0对应 0.5625 + 0.5625 ms
L_IR_TxByte_Pause:
	MOV		tx_cntH, #HIGH 21		;发送脉冲
	MOV		tx_cntL, #LOW  21
	LCALL	F_IR_TxPulse		;脉冲都是0.5625ms
	MOV		A, R5
	RR		A				;下一个位
	MOV		R5, A
	DJNZ	R4, L_IR_TxByteLoop
	POP		AR5
	POP		AR4
	
	RET

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
	MOV		CCON, #0x00;	//清除所有中断标志
	MOV		tx_cntH, #0
	MOV		tx_cntL, #2

	MOV		CCAPM0, #(0x48 + 1)	;工作模式 + 中断允许 0x00: PCA_Mode_Capture,  0x42: PCA_Mode_PWM,  0x48: PCA_Mode_SoftTimer
	MOV		PCA_Timer0H, #HIGH 100	;随便给一个小的初值
	MOV		PCA_Timer0L, #LOW  100
	MOV		CCAP0L, PCA_Timer0L	;将影射寄存器写入捕获寄存器，先写CCAPxL
	MOV		CCAP0H, PCA_Timer0H	;后写CCAPxH

	SETB	PPCA			;高优先级中断
	ANL		CMOD, #NOT 0xe0
	ORL		CMOD, #0x08		;选择时钟源, 0x00: 12T, 0x02: 2T, 0x04: Timer0溢出, 0x06: ECI, 0x08: 1T, 0x0A: 4T, 0x0C: 6T, 0x0E: 8T
	RET



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
	MOV		CCON, #0x40;	//清除所有中断标志,但不关CR
	JB		B_Space, L_PCA0_TxPause

					;发送脉冲，交替装载TH0值，溢出时自动重装
	CPL		P_IR_TX
	JNB		P_IR_TX, L_PCA0_LoadLow

	MOV		A, PCA_Timer0L
	ADD		A, #LOW D_38K_OFF	;装载高电平时间	17.3us
	MOV		PCA_Timer0L, A
	MOV		A, PCA_Timer0H
	ADDC	A, #HIGH D_38K_OFF
	MOV		PCA_Timer0H, A
	
	MOV		A, tx_cntL	;if(--tx_cnt == 0)	CR = 0;	//pulse has sent,	stop
	DEC		tx_cntL
	JNZ		$+4
	DEC		tx_cntH
	DEC		A
	ORL		A, tx_cntH
	JNZ		L_PCA0_Reload
	CLR		CR
	SJMP	L_PCA0_Reload

L_PCA0_LoadLow:
	MOV		A, PCA_Timer0L
	ADD		A, #LOW D_38K_ON	;PCA_Timer0 += D_38K_ON, 装载低电平时间	9us
	MOV		PCA_Timer0L, A
	MOV		A, PCA_Timer0H
	ADDC	A, #HIGH D_38K_ON
	MOV		PCA_Timer0H, A
	SJMP	L_PCA0_Reload
	
L_PCA0_TxPause:		;发送暂停时间
	MOV		A, PCA_Timer0L
	ADD		A, #LOW D_38K_DUTY	;PCA_Timer0 += D_38K_DUTY;	//装载周期时间	26.3us
	MOV		PCA_Timer0L, A
	MOV		A, PCA_Timer0H
	ADDC	A, #HIGH D_38K_DUTY
	MOV		PCA_Timer0H, A

	MOV		A, tx_cntL	;if(--tx_cnt == 0)	CR = 0;	//空闲时间
	DEC		tx_cntL
	JNZ		$+4
	DEC		tx_cntH
	DEC		A
	ORL		A, tx_cntH
	JNZ		L_PCA0_Reload
	CLR		CR

L_PCA0_Reload:
	MOV		CCAP0L, PCA_Timer0L	;将影射寄存器写入捕获寄存器，先写CCAP0L
	MOV		CCAP0H, PCA_Timer0H	;后写CCAP0H

	POP	ACC
	POP	PSW
	RETI

	END


