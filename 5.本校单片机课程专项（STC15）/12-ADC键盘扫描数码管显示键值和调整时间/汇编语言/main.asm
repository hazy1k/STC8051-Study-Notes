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

;左边4位LED显示时间(小时,分钟), 右边最后两位显示按键值.

;ADC按键键码为1~16.

;按键只支持单键按下, 不支持多键同时按下, 那样将会有不可预知的结果.

;键按下超过1秒后,将以10键/秒的速度提供重键输出. 用户只需要检测KeyCode是否非0来判断键是否按下.

;调整时间键:
;键码1: 小时+.
;键码2: 小时-.
;键码3: 分钟+.
;键码4: 分钟-.


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

AUXR      DATA 08EH
P4        DATA 0C0H
P5        DATA 0C8H
ADC_CONTR DATA 0BCH	;带AD系列
ADC_RES   DATA 0BDH	;带AD系列
ADC_RESL  DATA 0BEH	;带AD系列
P1ASF     DATA 09DH
PCON2     DATA 097H

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
Flag0			DATA	20H
B_1ms			BIT		Flag0.0	;	1ms标志

LED8			DATA	30H		;	显示缓冲 30H ~ 37H
display_index	DATA	38H		;	显示位索引

hour			DATA	39H		;RTC变量
minute			DATA	3AH
second			DATA	3BH		;
msecond_H		DATA	3CH		;
msecond_L		DATA	3DH		;

ADC_KeyState	DATA	3EH	; ADC键状态变量
ADC_KeyState1	DATA	3FH
ADC_KeyState2	DATA	40H
ADC_KeyState3	DATA	41H
ADC_KeyHoldCnt	DATA	42H	; ADC键按下计时

KeyCode			DATA	43H	; 给用户使用的键码, 1~16为ADC键， 17~32为IO键

cnt10ms			DATA	44H;
cnt50ms			DATA	45H;



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
	MOV		PSW, #0		;选择第0组R0~R7
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
	
	MOV		cnt10ms, #10

	CLR		A
	MOV		ADC_KeyState, A
	MOV		ADC_KeyState1, A
	MOV		ADC_KeyState2, A
	MOV		ADC_KeyState3, A	; 键状态
	MOV		ADC_KeyHoldCnt, A	; 键按下计时
	MOV		KeyCode, A		; 给用户使用的键码, 1~16有效

	MOV		P1ASF, #010H		; 设置P1.4为ADC
	MOV		ADC_CONTR, #0E0H	;打开ADC, 设置速度90T
	ORL		PCON2, #020H		;10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。

;=====================================================

;=====================================================
L_Main_Loop:
	JNB		B_1ms,  L_Main_Loop		;1ms未到
	CLR		B_1ms
	
;=================== 检测1000ms是否到 ==================================
	INC		msecond_L		;msecond + 1
	MOV		A, msecond_L
	JNZ		L_Check1000ms
	INC		msecond_H
L_Check1000ms:
	CLR		C
	MOV		A, msecond_L	;msecond - 1000
	SUBB	A, #LOW 1000
	MOV		A, msecond_H
	SUBB	A, #HIGH 1000
	JC		L_Quit_Check_1000ms		;if(msecond < 1000), jmp
	
;================= 1000ms到， 处理模拟的RTC ====================================
	MOV		msecond_L, #0	;if(msecond >= 1000)
	MOV		msecond_H, #0

	LCALL	F_RTC
	LCALL	F_DisplayRTC
L_Quit_Check_1000ms:

	MOV		A, msecond_L			;检测if(msecond == 500)
	CJNE	A, #LOW 500, L_Quit_Display_Dot
	MOV		A, msecond_H
	CJNE	A, #HIGH 500, L_Quit_Display_Dot
	LCALL	F_DisplayRTC	;小时后的小数点做秒闪
L_Quit_Display_Dot:
;=====================================================

;==================== 10ms读一次ADC =================================
L_Read_ADC_Key:
	DJNZ	cnt10ms, L_Quit_Read_ADC_Key	; (cnt10ms - 1) != 0, jmp
	MOV		cnt10ms, #10	;
	
	MOV		A, #4
	LCALL	F_Get_ADC10bitResult	;ACC - 通道0~7, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 1024 为错误
	MOV		A, R6
	ANL		A, #0FCH
	JNZ		L_Quit_Read_ADC_Key		; adc >= 1024, 错误
	LCALL	F_CalculateAdcKey		; 计算按键, (R6 R7) == adc, 返回值在R4 R5.

L_Quit_Read_ADC_Key:
;=====================================================

;=====================================================
L_KeyProcess:
	MOV		A, KeyCode
	JZ		L_Quit_KeyProcess
							;有键按下
	MOV		A, KeyCode
	MOV		B, #10
	DIV		AB
	MOV		LED8+6, A		; 显示键码
	MOV		LED8+7, B

	MOV		A, KeyCode
	CJNE	A, #1, L_Quit_K1
	INC		hour	; hour + 1
	MOV		A, hour
	CLR		C
	SUBB	A, #24
	JC		L_K1_Display
	MOV		hour, #0
L_K1_Display:
	LCALL	F_DisplayRTC
L_Quit_K1:

	MOV		A, KeyCode
	CJNE	A, #2, L_Quit_K2
	DEC		hour	; hour - 1
	MOV		A, hour
	CJNE	A, #255, L_K2_Display
	MOV		hour, #23
L_K2_Display:
	LCALL	F_DisplayRTC
L_Quit_K2:

	MOV		A, KeyCode
	CJNE	A, #3, L_Quit_K3
	MOV		second, #0		;调整分钟时清除秒
	INC		minute	; minute + 1
	MOV		A, minute
	CLR		C
	SUBB	A, #60
	JC		L_K3_Display
	MOV		minute, #0
L_K3_Display:
	LCALL	F_DisplayRTC
L_Quit_K3:

	MOV		A, KeyCode
	CJNE	A, #4, L_Quit_K4
	MOV		second, #0		;调整分钟时清除秒
	DEC		minute	; minute - 1
	MOV		A, minute
	CJNE	A, #255, L_K4_Display
	MOV		minute, #59
L_K4_Display:
	LCALL	F_DisplayRTC
L_Quit_K4:

	MOV		KeyCode, #0
L_Quit_KeyProcess:

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
	
	MOV		A, minute
	MOV		B, #10
	DIV		AB
	MOV		LED8+2, A;
	MOV		LED8+3, B;

	MOV		A, msecond_L	;msecond - 500
	CLR		C
	SUBB	A, #LOW 500
	MOV		A, msecond_H
	SUBB	A, #HIGH 500
	JC		L_QuitDisplayRTC	;if(msecond < 500), jmp
	ORL		LED8+1, #DIS_DOT	; 小时后的小数点做秒闪
L_QuitDisplayRTC:
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



;========================================================================
; 函数: F_Get_ADC10bitResult
; 描述: 查询法读一次ADC结果.
; 参数: ACC: 选择要转换的ADC.
; 返回: (R6 R7) = 10位ADC结果.
; 版本: V1.0, 2013-10-22
;========================================================================
F_Get_ADC10bitResult:	;ACC - 通道0~7, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 1024 为错误
	MOV		R7, A			//channel
	MOV		ADC_RES,  #0;
	MOV		ADC_RESL, #0;

	MOV		A, ADC_CONTR		;ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	ANL		A, #0xE0
	ORL		A, #08H
	ORL		A, R7
	MOV		ADC_CONTR, A
	NOP
	NOP
	NOP
	NOP

L_WaitAdcLoop:
	MOV		A, ADC_CONTR
	JNB		ACC.4, L_WaitAdcLoop

	ANL		ADC_CONTR, #NOT 010H	;清除完成标志

	MOV		A, ADC_RES		;10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
	ANL		A, #3
	MOV		R6, A
	MOV		R7, ADC_RESL

L_QuitAdc:
	RET


;/***************** ADC键盘计算键码 *****************************
;电路和软件算法设计: Coody
;本ADC键盘方案在很多实际产品设计中, 验证了其稳定可靠, 即使按键使用导电膜,都很可靠.
;16个键,理论上各个键对应的ADC值为 (1024 / 16) * k = 64 * k, k = 1 ~ 16, 特别的, k=16时,对应的ADC值是1023.
;但是实际会有偏差,则判断时限制这个偏差, ADC_OFFSET为+-偏差, 则ADC值在 (64*k-ADC_OFFSET) 与 (64*k+ADC_OFFSET)之间为键有效.
;间隔一定的时间,就采样一次ADC,比如10ms.
;为了避免偶然的ADC值误判, 或者避免ADC在上升或下降时误判, 使用连续3次ADC值均在偏差范围内时, ADC值才认为有效.
;以上算法, 能保证读键非常可靠.
;**********************************************/
ADC_OFFSET	EQU	16		;必须小于30
F_CalculateAdcKey:		//R6 R7为输入的ADC值 (u16 adc),	10ms call
	PUSH	03H		;R3入栈
	PUSH	04H		;R4入栈
	PUSH	05H		;R5入栈
	PUSH	DPH		;DPH入栈
	PUSH	DPL		;DPL入栈
	
	CLR		C			;计算(adc - (64-ADC_OFFSET)
	MOV		A, R7
	SUBB	A, #(64 - ADC_OFFSET)
	MOV		A, R6
	SUBB	A, #0
	JNC		L_ADC_MoreThanOffset
	MOV		ADC_KeyState, #0	;	if(adc < (64-ADC_OFFSET)), 键状态归0
	MOV		ADC_KeyHoldCnt, #0	;
	
L_ADC_MoreThanOffset:

	MOV		R4, #0		;j = 64 - ADC_OFFSET;
	MOV		R5, #(64 - ADC_OFFSET)
	MOV		DPH, #0		;k = 64 + ADC_OFFSET;
	MOV		DPL, #(64 + ADC_OFFSET)
	MOV		R3, #1
L_CheckAdcKeyLoop1:
	CLR		C		; adc - j
	MOV		A, R7
	SUBB	A, R5
	MOV		A, R6
	SUBB	A, R4
	JC		L_CheckNextAdcKey		; adc < j

	CLR		C		; adc - k
	MOV		A, R7
	SUBB	A, DPL
	MOV		A, R6
	SUBB	A, DPH
	JNC		L_CheckNextAdcKey		; adc > k
	SJMP	L_CheckAdcKeyEnd	;if((adc >= j) && (adc <= k)),  判断是否在偏差范围内

L_CheckNextAdcKey:
	MOV		A, R5		; j = j + 64
	ADD		A, #64
	MOV		R5, A
	MOV		A, R4
	ADDC	A, #0
	MOV		R4, A

	MOV		A, DPL		; k = k + 64
	ADD		A, #64
	MOV		DPL, A
	MOV		A, DPH
	ADDC	A, #0
	MOV		DPH, A
	
	INC		R3
	MOV		A, R3
	CJNE	A, #17, L_CheckAdcKeyLoop1

L_CheckAdcKeyEnd:
	MOV		ADC_KeyState3, ADC_KeyState2
	MOV		ADC_KeyState2, ADC_KeyState1
	MOV		A, R3
	CLR		C
	SUBB	A, #17
	JC		L_AdcKeyIsOk
	MOV		ADC_KeyState1, #0	;if(i > 16)	KeyState1 = 0;	//键无效
	LJMP	L_QuitCheckAdcKey

L_AdcKeyIsOk:		;键有效
	MOV		ADC_KeyState1, R3
	MOV		A, ADC_KeyState3
	CJNE	A, ADC_KeyState2, L_QuitCheckAdcKey		;if (KeyState3 != KeyState2)
	CJNE	A, ADC_KeyState1, L_QuitCheckAdcKey 	;if (KeyState3 != KeyState1)
	JZ		L_QuitCheckAdcKey 					;if (KeyState3 == 0)

	MOV		A, ADC_KeyState
	JNZ		L_NotFirstCheck	;不是第一次检测到, jmp
	MOV		KeyCode,  R3	; 保存键码
	MOV		ADC_KeyState, R3	; 保存键状态
	MOV		ADC_KeyHoldCnt, #0	; 清除键按着的时间计数
L_NotFirstCheck:

	MOV		A, ADC_KeyState
	XRL		A, R3
	JNZ		L_NotCheckAdcKeySame
				;if(KeyState == i)	//连续检测到同一键按着
	INC		ADC_KeyHoldCnt
	MOV		A, ADC_KeyHoldCnt
	CJNE	A, #100, L_QuitCheckAdcKey	;if(++KeyHoldCnt >= 100)	//按下1秒后,以10次每秒的速度Repeat Key
	MOV		ADC_KeyHoldCnt, #90		;按下1秒后,以10次每秒的速度Repeat Key
	MOV		KeyCode, R3			; 保存键码
	SJMP	L_QuitCheckAdcKey
L_NotCheckAdcKeySame:
	MOV		ADC_KeyHoldCnt, #0	;	按下时间计数归0

L_QuitCheckAdcKey:
	POP		DPL		;DPL出栈
	POP		DPH		;DPH出栈
	POP		05H		;R5出栈
	POP		04H		;R4出栈
	POP		03H		;R3出栈
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


;//========================================================================
;// 函数: F_Send_595
;// 描述: 向HC595发送一个字节子程序。
;// 参数: ACC: 要发送的字节数据.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
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

;//========================================================================
;// 函数: F_DisplayScan
;// 描述: 显示扫描子程序。
;// 参数: none.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
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



