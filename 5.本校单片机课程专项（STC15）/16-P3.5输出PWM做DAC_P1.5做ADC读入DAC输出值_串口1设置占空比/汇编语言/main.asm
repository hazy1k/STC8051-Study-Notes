
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

;使用PCA0从P3.5输出8位的PWM, 输出的PWM经过RC滤波成直流电压送P1.5做ADC并用数码管显示出来.

;串口1配置为115200bps, 8,n, 1, 切换到P3.0 P3.1, 下载后就可以直接测试. 主时钟为22.1184MHZ, 通过串口1设置占空比.

;串口命令使用ASCII码的数字，比如： 10，就是设置占空比为10/256， 100： 就是设置占空比为100/256。

;可以设置的值为0~256, 0为连续低电平, 256为连续高电平.

;左边4位数码管显示PWM的占空比值，右边4位数码管显示ADC值。

;用STC的MCU的IO方式控制74HC595驱动8位数码管。

;用户可以修改宏来选择时钟频率.

;用户可以在"用户定义宏"中选择共阴或共阳. 推荐尽量使用共阴数码管.

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;******************************************/

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ	EQU	22118	;22118KHZ

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

LED_TYPE		EQU		000H	; 定义LED类型, 000H -- 共阴, 0FFH -- 共阳

Timer0_Reload	EQU		(65536 - Fosc_KHZ)	; Timer 0 中断频率, 1000次/秒

DIS_DOT			EQU		020H
DIS_BLACK		EQU		010H
DIS_			EQU		011H

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


ADC_P10			EQU		0x01	;IO引脚 Px.0
ADC_P11			EQU		0x02	;IO引脚 Px.1
ADC_P12			EQU		0x04	;IO引脚 Px.2
ADC_P13			EQU		0x08	;IO引脚 Px.3
ADC_P14			EQU		0x10	;IO引脚 Px.4
ADC_P15			EQU		0x20	;IO引脚 Px.5
ADC_P16			EQU		0x40	;IO引脚 Px.6
ADC_P17			EQU		0x80	;IO引脚 Px.7
ADC_P1_All		EQU		0xFF	;IO所有引脚

ADC_PowerOn		EQU		(1 SHL 7)
ADC_90T			EQU		(3 SHL 5)
ADC_180T		EQU		(2 SHL 5)
ADC_360T		EQU		(1 SHL 5)
ADC_540T		EQU		0
ADC_FLAG		EQU		(1 SHL 4)	;软件清0
ADC_START		EQU		(1 SHL 3)	;自动清0
ADC_CH0			EQU		0
ADC_CH1			EQU		1
ADC_CH2			EQU		2
ADC_CH3			EQU		3
ADC_CH4			EQU		4
ADC_CH5			EQU		5
ADC_CH6			EQU		6
ADC_CH7			EQU		7

ADC_RES_H2L8	EQU		(1 SHL 5)
ADC_RES_H8L2	EQU		NOT (1 SHL 5)

PCA_P12_P11_P10_P37		EQU		(0 SHL 4)
PCA_P34_P35_P36_P37		EQU		(1 SHL 4)
PCA_P24_P25_P26_P27		EQU		(2 SHL 4)
PCA_Mode_PWM			EQU		042H
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

;ENABLE					EQU		1

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

INT_CLKO 	DATA 	0x8F
P_SW1 		DATA	0A2H
IE2   		DATA	0AFH
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
B_1ms			BIT		20H.0	;	1ms标志

LED8			DATA	30H		;	显示缓冲 30H ~ 37H
display_index	DATA	38H		;	显示位索引

msecond_H		DATA	39H		;
msecond_L		DATA	3AH		;

RX1_Lenth		EQU		16		; 串口接收缓冲长度

B_TX1_Busy		BIT		20H.1	; 发送忙标志
B_RX1_OK		BIT		20H.2	; 接收结束标志
TX1_Cnt			DATA	3BH		; 发送计数
RX1_Cnt			DATA	3CH		; 接收计数
RX1_TimeOut		DATA	3DH		; 超时计数
RX1_Buffer		DATA	40H		;40 ~ 4FH 接收缓冲

;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		000BH				;1  Timer0 interrupt
		LJMP	F_Timer0_Interrupt

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
	

	LCALL	F_ADC_config	; ADC初始化
	LCALL	F_PCA_Init	;PCA初始化

	MOV		LED8+0, #1	;	//显示PWM默认值
	MOV		LED8+1, #2	;
	MOV		LED8+2, #8	;
	MOV		LED8+3, #DIS_BLACK	;	//这位不显示

	MOV		A, #1
	LCALL	F_UART1_config	; 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.

	MOV		DPTR, #StringText1	;Load string address to DPTR
	LCALL	F_SendString1		;Send string

;=================== 主循环 ==================================
L_Main_Loop:
	JNB		B_1ms,  L_Main_Loop		;1ms未到
	CLR		B_1ms
	
	MOV		A, RX1_TimeOut
	JNZ		L_CheckRx1TimeOut	;串口空闲超时计数非0, 
L_QuitCheckRxTimeOut:
	LJMP	L_QuitProcessUart1	;串口空闲超时计数为0, 退出
L_CheckRx1TimeOut:
	DJNZ	RX1_TimeOut, L_QuitCheckRxTimeOut	;串口空闲超时计数-1非0退出

	SETB	B_RX1_OK			;串口空闲超时, 接收结束, 做标志
	MOV		A, RX1_Cnt
	JNZ		L_RxCntNotZero		;接收到字符数非空, 则处理
	LJMP	L_QuitProcessUart1	;接收到字符数空, 则退出

L_RxCntNotZero:
	ANL		A, #NOT 3	;限制为3位数字
	JZ		L_NumberLengthOk
	MOV		DPTR, #StringError3	;Load string address to DPTR	"错误! 输入字符过多! 输入占空比为0~256!"
	LCALL	F_SendString1		;Send string
	LJMP	L_QuitCalculatePWM	;退出
	
L_NumberLengthOk:
	MOV		R2, #0
	MOV		R3, #0
	MOV		R0, #RX1_Buffer
	MOV		R1, RX1_Cnt

L_GetUartPwmLoop:
	MOV		A, @R0
	CLR		C
	SUBB	A, #'0'				;计算是否小于ASCII数字0
	JNC		L_RxdataLargeThan0
	MOV		DPTR, #StringError1	;Load string address to DPTR	"错误! 接收到小于0的数字字符! 占空比为0~256!"
	LCALL	F_SendString1		;Send string
	LJMP	L_QuitCalculatePWM
	
L_RxdataLargeThan0:
	MOV		A, @R0
	CLR		C
	SUBB	A, #03AH			;计算是否大于ASCII数字9
	JC		L_RxdataLessThan03A
	MOV		DPTR, #StringError4	;Load string address to DPTR	"错误! 接收到大于9的数字字符! 占空比为0~256!"
	LCALL	F_SendString1		;Send string
	LJMP	L_QuitCalculatePWM

L_RxdataLessThan03A:
	MOV		AR4, R2
	MOV		AR5, R3
	MOV		R7, #10
	LCALL	F_MUL16x8	;(R4,R5)* R7 -->(R5,R6,R7)
	MOV		AR2, R6
	MOV		AR3, R7
	MOV		A, @R0		;计算 [R2 R3] = [R2 R3] * 10 + RX1_Buffer[i] - '0';
	CLR		C
	SUBB	A, #'0'
	ADD		A, R3
	MOV		R3, A
	CLR		A
	ADDC	A, R2
	MOV		R2, A
	INC		R0
	DJNZ	R1, L_GetUartPwmLoop

	MOV		A, R3
	CLR		C
	SUBB	A, #LOW 257		;if(j >= 257)
	MOV		A, R2
	SUBB	A, #HIGH 257
	JC		L_SetPWM_Right
	MOV		DPTR, #StringError2	;Load string address to DPTR	错误! 输入占空比过大, 请不要大于256!
	LCALL	F_SendString1		;Send string
	LJMP	L_QuitCalculatePWM

L_SetPWM_Right:

	LCALL	F_UpdatePwm		;更新占空比
	MOV		A, R2
	JZ		L_PWM_LessTan256
	MOV		LED8,   #2		;占空比为256,显示256
	MOV		LED8+1, #5
	MOV		LED8+2, #6
	SJMP	L_SetPWM_OK
L_PWM_LessTan256:
	MOV		A, R3
	MOV		B, #100		;显示0~255占空比
	DIV		AB
	MOV		LED8,   A
	MOV		A, B
	MOV		B, #10
	DIV		AB
	MOV		LED8+1, A
	MOV		LED8+2, B
	MOV		A, LED8
	JNZ		L_SetPWM_OK
	MOV		LED8, #DIS_BLACK	;百位消0

L_SetPWM_OK:
	MOV		DPTR, #StringText2	;Load string address to DPTR	"已更新占空比!"
	LCALL	F_SendString1		;Send string
L_QuitCalculatePWM:
	MOV		RX1_Cnt, #0
	CLR		B_RX1_OK
L_QuitProcessUart1:

;=================== 检测300ms是否到 ==================================
	INC		msecond_L		;msecond + 1
	MOV		A, msecond_L
	JNZ		$+4
	INC		msecond_H
	
	CLR		C
	MOV		A, msecond_L	;msecond - 300
	SUBB	A, #LOW 300
	MOV		A, msecond_H
	SUBB	A, #HIGH 300
	JNC		L_300msIsGood	;if(msecond < 300), jmp
	LJMP	L_Main_Loop		;if(msecond == 300), jmp
L_300msIsGood:

;================= 300ms到 ====================================
	MOV		msecond_L, #0	;if(msecond >= 1000)
	MOV		msecond_H, #0

	MOV		A, #ADC_CH5
	LCALL	F_Get_ADC10bitResult	; 读外部电压ADC, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 1024 为错误

	LCALL	F_HEX2_DEC		;(R6 R7) HEX Change to DEC ---> (R3 R4 R5), use (R2~R7)
	
	MOV		A, R4			;显示ADC值
	SWAP	A
	ANL		A, #0x0F
	MOV		LED8+4, A
	MOV		A, R4
	ANL		A, #0x0F
	MOV		LED8+5, A
	MOV		A, R5
	SWAP	A
	ANL		A, #0x0F
	MOV		LED8+6, A
	MOV		A, R5
	ANL		A, #0x0F
	MOV		LED8+7, A

	MOV		A, LED8+4			;显示消无效0
	JNZ		L_QuitProcessADC
	MOV		LED8+4, #DIS_BLACK
	MOV		A, LED8+5
	JNZ		L_QuitProcessADC
	MOV		LED8+5, #DIS_BLACK
	MOV		A, LED8+6
	JNZ		L_QuitProcessADC
	MOV		LED8+6, #DIS_BLACK
L_QuitProcessADC:

L_Quit_Check_300ms:


	LJMP	L_Main_Loop
;===================================================================

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
	ORL		A, #PCA_P34_P35_P36_P37		;切换IO口
	MOV		AUXR1, A
	MOV		A, CMOD
	ANL		A, #NOT(7 SHL 1)
	ORL		A, #PCA_Clock_1T		;选择时钟源
	MOV		CMOD, A

	MOV		CCAPM0, #PCA_Mode_PWM	; PWM模式
	MOV		CCAP0L, #128			; 将影射寄存器写入捕获寄存器，先写CCAP0L
	MOV		CCAP0H, #128			; 后写CCAP0H

;	SETB	PPCA		; 高优先级中断
	SETB	CR			; 运行PCA定时器
	RET

;======================================================================

;========================================================================
; 函数: F_UpdatePwm
; 描述: 更新PWM值. 
; 参数: [R2 R3]: pwm值, 这个值是输出低电平的时间.
; 返回: none.
; 版本: V1.0, 2012-11-22
;========================================================================
F_UpdatePwm:
	MOV		A, R2
	ORL		A, R3
	JNZ		L_UpdatePWM_Not0
	ORL		PCA_PWM0, #3		; PWM0一直输出0		PWM0_OUT_0();	输出连续低电平
	RET
	
L_UpdatePWM_Not0:
	MOV		A, #0
	CLR		C
	SUBB	A, R3
	MOV		CCAP0H, A
	ANL		PCA_PWM0, #NOT 3	; PWM0正常输出(默认)	PWM0_NORMAL();
	RET


;***************************************************************************
F_MUL16x8:               ;(R4,R5)* R7 -->(R5,R6,R7)
        MOV   A,R7      ;1T		1
		MOV   B,R5      ;2T		3
		MUL   AB        ;4T  R3*R7	4
		MOV   R6,B      ;1T		4
		XCH   A,R7      ;2T		3
		
		MOV   B,R4      ;1T		3
		MUL   AB        ;4T  R3*R6	4
		ADD   A,R6      ;1T		2
		MOV   R6,A      ;1T		3
		CLR   A         ;1T		1
		ADDC  A,B       ;1T		3
		MOV   R5,A      ;1T		2
		RET             ;4T		10

;//========================================================================
;// 函数: F_HEX2_DEC
;// 描述: 把双字节十六进制数转成十进制BCD数.
;// 参数: (R6 R7): 要转换的双字节十六进制数.
;// 返回: (R3 R4 R5) = BCD码.
;// 版本: V1.0, 2013-10-22
;//========================================================================
F_HEX2_DEC:        	;(R6 R7) HEX Change to DEC ---> (R3 R4 R5), use (R2~R7)
		MOV  	R2,#16
		MOV  	R3,#0
		MOV  	R4,#0
		MOV  	R5,#0

L_HEX2_DEC:
		CLR  	C	
		MOV  	A,R7
		RLC  	A	
		MOV  	R7,A
		MOV  	A,R6
		RLC  	A	
		MOV  	R6,A

		MOV  	A,R5
		ADDC 	A,R5
		DA   	A	
		MOV  	R5,A

		MOV  	A,R4
		ADDC 	A,R4
		DA   	A	
		MOV  	R4,A

		MOV  	A,R3
		ADDC 	A,R3
		DA   	A	
		MOV  	R3,A

		DJNZ 	R2,L_HEX2_DEC
		RET
;**********************************************/

F_ADC_config:
	MOV		P1ASF, #ADC_P15			; 设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	MOV		ADC_CONTR, #(ADC_PowerOn + ADC_90T)	;打开ADC, 设置速度
	ORL		PCON2, #ADC_RES_H2L8		;10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
	ORL		P1M1, #ADC_P15			; 把ADC口设置为高阻输入
	ANL		P1M0, #NOT ADC_P15
;	SETB	EADC					;中断允许
;	SETB	PADC					;优先级设置
	RET

;//========================================================================
;// 函数: F_Get_ADC10bitResult
;// 描述: 查询法读一次ADC结果.
;// 参数: ACC: 选择要转换的ADC.
;// 返回: (R6 R7) = 10位ADC结果.
;// 版本: V1.0, 2013-10-22
;//========================================================================
F_Get_ADC10bitResult:	;ACC - 通道0~7, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 1024 为错误
	MOV		R7, A			//channel
	MOV		ADC_RES,  #0;
	MOV		ADC_RESL, #0;

	MOV		A, ADC_CONTR		;ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 
	ANL		A, #0xE0
	ORL		A, #ADC_START
	ORL		A, R7
	MOV		ADC_CONTR, A
	NOP
	NOP
	NOP
	NOP

	MOV		R3, #100
L_WaitAdcLoop:
	MOV		A, ADC_CONTR
	JNB		ACC.4, L_CheckAdcTimeOut

	ANL		ADC_CONTR, #NOT ADC_FLAG	;清除完成标志
	MOV		A, ADC_RES		;10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
	ANL		A, #3
	MOV		R6, A
	MOV		R7, ADC_RESL
	SJMP	L_QuitAdc
	
L_CheckAdcTimeOut:
	DJNZ	R3, L_WaitAdcLoop
	MOV		R6, #HIGH 1024		;超时错误,返回1024,调用的程序判断
	MOV		R7, #LOW  1024
L_QuitAdc:
	RET

;====================================================================================
StringText1:
    DB  "PWM和ADC测试程序, 输入占空比为 0~256!",0DH,0AH,0
StringText2:
	DB	"已更新占空比!",0DH,0AH,0
StringError1:
	DB	"错误! 接收到小于0的数字字符! 占空比为0~256!",0DH,0AH,0
StringError2:
	DB	"错误! 输入占空比过大, 请不要大于256!",0DH,0AH,0
StringError3:
	DB	"错误! 输入字符过多! 输入占空比为0~256!",0DH,0AH,0
StringError4:
	DB	"错误! 接收到大于9的数字字符! 占空比为0~256!",0DH,0AH,0


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
	SETB	B_TX1_Busy		;标志发送忙
	MOV		SBUF, A			;发送一个字节
	JB		B_TX1_Busy, $	;等待发送完成
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
	JB		B_RX1_OK, L_QuitUartRx
	MOV		A, RX1_Cnt
	CJNE	A, #RX1_Lenth, L_RxCntNotOver
	MOV		RX1_Cnt, #0		; 避免溢出处理
L_RxCntNotOver:
	MOV		A, #RX1_Buffer
	ADD		A, RX1_Cnt
	MOV		R0, A
	MOV		@R0, SBUF	;保存一个字节
	INC		RX1_Cnt
	MOV		RX1_TimeOut, #5
L_QuitUartRx:

	JNB		TI, L_QuitUartTx
	CLR		TI
	CLR		B_TX1_Busy		; 清除发送忙标志
L_QuitUartTx:

	POP		AR0
	POP		ACC
	POP		PSW
	RETI


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
	XRL		A, #NOT LED_TYPE
	LCALL	F_Send_595		;输出位码
	
	MOV		DPTR, #T_Display
	MOV		A, display_index
	ADD		A, #LED8
	MOV		R0, A
	MOV		A, @R0
	MOVC	A, @A+DPTR
	XRL		A, #LED_TYPE
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



