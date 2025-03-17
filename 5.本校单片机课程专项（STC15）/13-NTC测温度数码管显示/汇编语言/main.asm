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

;读ADC和测温度.

;用STC的MCU的IO方式控制74HC595驱动8位数码管。

;用户可以修改宏来选择时钟频率.

;用户可以在"用户定义宏"中选择共阴或共阳. 推荐尽量使用共阴数码管.

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;左边4位数码管显示ADC2接的电压基准TL431的读数, 右边4位数码管显示温度值, 分辨率0.1度.

;NTC使用1%精度的MF52 10K@25度C.

;测温度时, 为了通用, 使用12位的ADC值, 使用对分查找表格来计算, 小数点后一位数是用线性插补来计算的.

;所以, 测温度的ADC3进行4次ADC连续采样, 变成12位的ADC来计算温度.


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

msecond_H		DATA	39H		;
msecond_L		DATA	3AH		;
min				DATA	3BH
max				DATA	3CH



;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		0003H				;0 INT0 interrupt
		RETI
		LJMP	F_INT0_Interrupt      

		ORG		000BH				;1  Timer0 interrupt
		LJMP	F_Timer0_Interrupt

		ORG		0013H				;2  INT1 interrupt
		LJMP	F_INT1_Interrupt      

		ORG		001BH				;3  Timer1 interrupt
		LJMP	F_Timer1_Interrupt

		ORG		0023H				;4  UART1 interrupt
		LJMP	F_UART1_Interrupt

		ORG		002BH				;5  ADC and SPI interrupt
		LJMP	F_ADC_Interrupt

		ORG		0033H				;6  Low Voltage Detect interrupt
		LJMP	F_LVD_Interrupt

		ORG		003BH				;7  PCA interrupt
		LJMP	F_PCA_Interrupt

		ORG		0043H				;8  UART2 interrupt
		LJMP	F_UART2_Interrupt

		ORG		004BH				;9  SPI interrupt
		LJMP	F_SPI_Interrupt

		ORG		0053H				;10  INT2 interrupt
		LJMP	F_INT2_Interrupt

		ORG		005BH				;11  INT3 interrupt
		LJMP	F_INT3_Interrupt

		ORG		0063H				;12  Timer2 interrupt
		LJMP	F_Timer2_Interrupt

		ORG		0083H				;16  INT4 interrupt
		LJMP	F_INT4_Interrupt



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
	

	LCALL	F_ADC_config	; ADC初始化

;=====================================================

;=====================================================
L_Main_Loop:
	JNB		B_1ms,  L_Main_Loop		;1ms未到
	CLR		B_1ms
	
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
	JC		L_Main_Loop		;if(msecond < 300), jmp
	
;================= 300ms到 ====================================
	MOV		msecond_L, #0	;if(msecond >= 1000)
	MOV		msecond_H, #0

	MOV		A, #2
	LCALL	F_Get_ADC10bitResult	;ACC - 通道0~7, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 1024 为错误
	MOV		A, R6
	ANL		A, #0FCH
	JNZ		L_Read_431_ADC_Err		; adc >= 1024, 错误

	LCALL	F_HEX2_DEC		;(R6 R7) HEX Change to DEC ---> (R3 R4 R5), use (R2~R7)
	MOV		A, R4
	SWAP	A
	ANL		A, #0x0F
	MOV		LED8,   A		;显示Vref的ADC值
	MOV		A, R4
	ANL		A, #0x0F
	MOV		LED8+1, A
	MOV		A, R5
	SWAP	A
	ANL		A, #0x0F
	MOV		LED8+2, A
	MOV		A, R5
	ANL		A, #0x0F
	MOV		LED8+3, A
	MOV		A, LED8
	JNZ		L_QuitRead_431_ADC
	MOV		LED8, #DIS_BLACK		;千位为0则消隐
	SJMP	L_QuitRead_431_ADC

L_Read_431_ADC_Err:		;错误, 显示 ----
	MOV		LED8,   #DIS_
	MOV		LED8+1, #DIS_
	MOV		LED8+2, #DIS_
	MOV		LED8+3, #DIS_
L_QuitRead_431_ADC:

	MOV		R2, #4
	MOV		R4, #0
	MOV		R5, #0
L_NTC_ADC_Loop:
	MOV		A, #3
	LCALL	F_Get_ADC10bitResult	;ACC - 通道0~7, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 1024 为错误
	MOV		A, R7
	ADD		A, R5
	MOV		R5, A
	MOV		A, R6
	ADDC	A, R4
	MOV		R4, A
	DJNZ	R2, L_NTC_ADC_Loop	;连续做4次ADC, 加起来获得12位的ADC, 计算NTC温度需要12位的ADC.

	MOV		A, R4
	ANL		A, #0F0H
	JNZ		L_Read_NTC_ADC_Err		; (adc * 4) >= 4096, 错误

	MOV		AR6, R4
	MOV		AR7, R5
	LCALL	F_get_temperature	;计算温度值, (R6 R7)为12位ADC,  
								;返回温度为(R6 R7), 0对应-40.0度, 400对应0度, 625对应25.0度, 最大1600对应120.0度.
	MOV		A, R6
	ANL		A, #0xC0
	JNZ		L_Read_NTC_ADC_Err		; >= 16384, 错误
	
	MOV		A, R7
	CLR		C
	SUBB	A, #LOW 400
	MOV		A, R6
	SUBB	A, #HIGH 400
	JC		L_Temp_LowThan_0		; (R6 R7) < 400, jmp
	CLR		F0		; 温度 >= 0度
	MOV		R6, A	; (R6 R7) = (R6 R7) - 400
	MOV		A, R7
	CLR		C
	SUBB	A, #LOW 400
	MOV		R7, A
	SJMP	L_DisplayTemp

L_Temp_LowThan_0:
	SETB	F0			; 温度 < 0度
	MOV		A, #LOW 400	; (R6 R7) = 400 - (R6 R7)
	CLR		C
	SUBB	A, R7
	MOV		R7, A
	MOV		A, #HIGH 400
	SUBB	A, R6
	MOV		R6, A

L_DisplayTemp:	
	LCALL	F_HEX2_DEC		;(R6 R7) HEX Change to DEC ---> (R3 R4 R5), use (R2~R7)
	MOV		A, R4
	SWAP	A
	ANL		A, #0x0F
	MOV		LED8+4, A		;显示温度值
	MOV		A, R4
	ANL		A, #0x0F
	MOV		LED8+5, A
	MOV		A, R5
	SWAP	A
	ANL		A, #0x0F
	ADD		A, #DIS_DOT
	MOV		LED8+6, A
	MOV		A, R5
	ANL		A, #0x0F
	MOV		LED8+7, A
	MOV		A, LED8+4
	JNZ		L_LED8_4_Not_0
	MOV		LED8+4, #DIS_BLACK		;千位为0则消隐
L_LED8_4_Not_0:
	JNB		F0, L_QuitRead_NTC_ADC
	MOV		LED8+4, #DIS_	;负温度, 显示-
	SJMP	L_QuitRead_NTC_ADC

L_Read_NTC_ADC_Err:		;错误, 显示 ----
	MOV		LED8+4, #DIS_
	MOV		LED8+5, #DIS_
	MOV		LED8+6, #DIS_
	MOV		LED8+7, #DIS_
L_QuitRead_NTC_ADC:

L_Quit_Check_300ms:

;=====================================================


	LJMP	L_Main_Loop

;**********************************************/

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


F_ADC_config:
	MOV		P1ASF, #(ADC_P12 + ADC_P13)		; 设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	MOV		ADC_CONTR, #(ADC_PowerOn + ADC_90T)	;打开ADC, 设置速度
	ORL		PCON2, #ADC_RES_H2L8		;10位AD结果的高2位放ADC_RES的低2位，低8位在ADC_RESL。
;	SETB	EADC						;中断允许
;	SETB	PADC						;优先级设置
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
	
F_Timer1_Interrupt:
	RETI

F_Timer2_Interrupt:
	RETI

F_INT0_Interrupt:
	RETI
	
F_INT1_Interrupt:
	RETI

F_INT2_Interrupt:
	RETI

F_INT3_Interrupt:
	RETI

F_INT4_Interrupt:
	RETI

F_UART1_Interrupt:
	RETI

F_UART2_Interrupt:
	RETI

F_ADC_Interrupt:
	RETI

F_LVD_Interrupt:
	RETI

F_PCA_Interrupt:
	RETI

F_SPI_Interrupt:
	RETI





;	MF52E 10K at 25, B = 3950, ADC = 12 bits
temp_table:
	DW	140		;-40	0
	DW	149		;-39	1
	DW	159		;-38	2
	DW	168		;-37	3
	DW	178		;-36	4
	DW	188		;-35	5
	DW	199		;-34	6
	DW	210		;-33	7
	DW	222		;-32	8
	DW	233		;-31	9
	DW	246		;-30	10
	DW	259		;-29	11
	DW	272		;-28	12
	DW	286		;-27	13
	DW	301		;-26	14
	DW	317		;-25	15
	DW	333		;-24	16
	DW	349		;-23	17
	DW	367		;-22	18
	DW	385		;-21	19
	DW	403		;-20	20
	DW	423		;-19	21
	DW	443		;-18	22
	DW	464		;-17	23
	DW	486		;-16	24
	DW	509		;-15	25
	DW	533		;-14	26
	DW	558		;-13	27
	DW	583		;-12	28
	DW	610		;-11	29
	DW	638		;-10	30
	DW	667		;-9	31
	DW	696		;-8	32
	DW	727		;-7	33
	DW	758		;-6	34
	DW	791		;-5	35
	DW	824		;-4	36
	DW	858		;-3	37
	DW	893		;-2	38
	DW	929		;-1	39
	DW	965		;0	40
	DW	1003	;1	41
	DW	1041	;2	42
	DW	1080	;3	43
	DW	1119	;4	44
	DW	1160	;5	45
	DW	1201	;6	46
	DW	1243	;7	47
	DW	1285	;8	48
	DW	1328	;9	49
	DW	1371	;10	50
	DW	1414	;11	51
	DW	1459	;12	52
	DW	1503	;13	53
	DW	1548	;14	54
	DW	1593	;15	55
	DW	1638	;16	56
	DW	1684	;17	57
	DW	1730	;18	58
	DW	1775	;19	59
	DW	1821	;20	60
	DW	1867	;21	61
	DW	1912	;22	62
	DW	1958	;23	63
	DW	2003	;24	64
	DW	2048	;25	65
	DW	2093	;26	66
	DW	2137	;27	67
	DW	2182	;28	68
	DW	2225	;29	69
	DW	2269	;30	70
	DW	2312	;31	71
	DW	2354	;32	72
	DW	2397	;33	73
	DW	2438	;34	74
	DW	2479	;35	75
	DW	2519	;36	76
	DW	2559	;37	77
	DW	2598	;38	78
	DW	2637	;39	79
	DW	2675	;40	80
	DW	2712	;41	81
	DW	2748	;42	82
	DW	2784	;43	83
	DW	2819	;44	84
	DW	2853	;45	85
	DW	2887	;46	86
	DW	2920	;47	87
	DW	2952	;48	88
	DW	2984	;49	89
	DW	3014	;50	90
	DW	3044	;51	91
	DW	3073	;52	92
	DW	3102	;53	93
	DW	3130	;54	94
	DW	3157	;55	95
	DW	3183	;56	96
	DW	3209	;57	97
	DW	3234	;58	98
	DW	3259	;59	99
	DW	3283	;60	100
	DW	3306	;61	101
	DW	3328	;62	102
	DW	3351	;63	103
	DW	3372	;64	104
	DW	3393	;65	105
	DW	3413	;66	106
	DW	3432	;67	107
	DW	3452	;68	108
	DW	3470	;69	109
	DW	3488	;70	110
	DW	3506	;71	111
	DW	3523	;72	112
	DW	3539	;73	113
	DW	3555	;74	114
	DW	3571	;75	115
	DW	3586	;76	116
	DW	3601	;77	117
	DW	3615	;78	118
	DW	3628	;79	119
	DW	3642	;80	120
	DW	3655	;81	121
	DW	3667	;82	122
	DW	3679	;83	123
	DW	3691	;84	124
	DW	3702	;85	125
	DW	3714	;86	126
	DW	3724	;87	127
	DW	3735	;88	128
	DW	3745	;89	129
	DW	3754	;90	130
	DW	3764	;91	131
	DW	3773	;92	132
	DW	3782	;93	133
	DW	3791	;94	134
	DW	3799	;95	135
	DW	3807	;96	136
	DW	3815	;97	137
	DW	3822	;98	138
	DW	3830	;99	139
	DW	3837	;100	140
	DW	3844	;101	141
	DW	3850	;102	142
	DW	3857	;103	143
	DW	3863	;104	144
	DW	3869	;105	145
	DW	3875	;106	146
	DW	3881	;107	147
	DW	3887	;108	148
	DW	3892	;109	149
	DW	3897	;110	150
	DW	3902	;111	151
	DW	3907	;112	152
	DW	3912	;113	153
	DW	3917	;114	154
	DW	3921	;115	155
	DW	3926	;116	156
	DW	3930	;117	157
	DW	3934	;118	158
	DW	3938	;119	159
	DW	3942	;120	160

; 获取数据表中双字节数据的首地址
; 输入: ACC
; 输出: DPTR.   DPTR = #temp_table + ACC * 2

F_GetFirstAddress:	;DPTR = #temp_table + ACC * 2
	MOV		DPTR, #temp_table
	PUSH	01H		; R1入栈
	MOV		R1, A
	ADD		A, DPL
	MOV		DPL, A
	CLR		A
	ADDC	A, DPH
	MOV		DPH, A
	
	MOV		A, R1
	ADD		A, DPL
	MOV		DPL, A
	CLR		A
	ADDC	A, DPH
	MOV		DPH, A
	POP		01H		;R1 出栈
	RET


;/********************  计算温度 ***********************************************/
;// 计算结果: 0对应-40.0度, 400对应0度, 625对应25.0度, 最大1600对应120.0度. 
;// 为了通用, ADC输入为12bit的ADC值.
;// 电路和软件算法设计: Coody
;/**********************************************/

D_SCALE		EQU		10		//结果放大倍数, 放大10倍就是保留一位小数
F_get_temperature:    ;(R6 R7)为12位ADC,  返回温度为(R6 R7), 0对应-40.0度, 400对应0度, 625对应25.0度, 最大1600对应120.0度.

	CLR		C
	MOV		A, #LOW 4096	;adc = 4096 - adc;	//Rt接地时要做这个计算
	SUBB	A, R7
	MOV		R7, A
	MOV		A, #HIGH 4096
	SUBB	A, R6
	MOV		R6, A
	
	
	MOV		DPTR, #temp_table
	MOV		A, #1
	MOVC	A, @A+DPTR
	SETB	C
	SUBB	A, R7				; table[0] - adc - 1
	CLR		A
	MOVC	A, @A+DPTR
	SUBB	A, R6
	JC		L_MoreThanTable_0	; adc >= table[0], jmp
	MOV		R6, #0xFF			; if(adc < table[0])	return (0xfffe)
	MOV		R7, #0xFE
	RET
L_MoreThanTable_0:
	MOV		A, #160
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2

	MOV		A, #1
	MOVC	A, @A+DPTR
	CLR		C
	SUBB	A, R7				; table[160] - adc
	CLR		A
	MOVC	A, @A+DPTR
	SUBB	A, R6
	JNC		L_LessThanTable_160	; adc <= table[160], jmp
	MOV		R6, #0xFF			;if(adc > table[160])		return (0xffff)
	MOV		R7, #0xFF
	RET
L_LessThanTable_160:
	
	MOV		min, #0		; -40度
	MOV		max, #160	; 120度

	MOV		R0, #5
L_NTC_CheckTableLoop1:	;对分查表
	MOV		A, min
	CLR		C
	RRC		A
	PUSH	ACC
	MOV		A, max
	CLR		C
	RRC		A
	MOV		R1, A
	POP		ACC
	ADD		A, R1
	MOV		R1, A	;R1 = min / 2 + max / 2
	
	MOV		A, R1
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2
	MOV		A, #1				; table[R1] - adc 
	MOVC	A, @A+DPTR
	CLR		C
	SUBB	A, R7
	CLR		A
	MOVC	A, @A+DPTR
	SUBB	A, R6
	JC		L_MoreThanTable_R1	; adc > table[R1], jmp
	MOV		max, R1				; if(adc <= table[R1])	max = R1
	SJMP	L_NTC_CheckLoopEnd
L_MoreThanTable_R1:
	MOV		min, R1				; if(adc > table[R1])	min = R1
L_NTC_CheckLoopEnd:
	DJNZ	R0, L_NTC_CheckTableLoop1

L_CheckEQU_Min:		;检测是够刚好 adc == table[min]
	MOV		A, min
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2
	MOV		A, #1
	MOVC	A, @A+DPTR
	CJNE	A, 07H, L_QuitCheckEQU_Min		; 07H == R7
	CLR		A
	MOVC	A, @A+DPTR
	CJNE	A, 06H, L_QuitCheckEQU_Min		; 06H == R6
	MOV		A, min
	MOV		B, #D_SCALE
	MUL		AB				;if(adc == table[min])	(R6 R7) = min * D_SCALE
	MOV		R6, B
	MOV		R7, A
	RET
L_QuitCheckEQU_Min:

L_CheckEQU_Max:		;检测是够刚好 adc == table[max]
	MOV		A, max
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2
	MOV		A, #1
	MOVC	A, @A+DPTR
	CJNE	A, AR7, L_QuitCheckEQU_Max		; AR7 == R7
	CLR		A
	MOVC	A, @A+DPTR
	CJNE	A, AR6, L_QuitCheckEQU_Max		; AR6 == R6
	MOV		A, max
	MOV		B, #D_SCALE
	MUL		AB				;if(adc == table[max])	(R6 R7) = max * D_SCALE
	MOV		R6, B
	MOV		R7, A
	RET
L_QuitCheckEQU_Max:


L_Calculate_Loop:		; table[min] < adc < table[max], 计算小数部分, 线性插补
	MOV		A, min
	SETB	C
	SUBB	A, max			; min - max - 1
	JC		L_Calculate_1	; while(min <= max)
	RET
L_Calculate_1:
	INC		min		; min++
	MOV		A, min
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2
	MOV		A, #1
	MOVC	A, @A+DPTR
	CJNE	A, AR7, L_AdcNotEquTableMin		; AR7 == R7
	CLR		A
	MOVC	A, @A+DPTR
	CJNE	A, AR6, L_AdcNotEquTableMin		; AR6 == R6
	MOV		A, min
	MOV		B, #D_SCALE
	MUL		AB				;if(adc == table[min])	(R6 R7) = min * D_SCALE
	MOV		R6, B
	MOV		R7, A
	RET

L_AdcNotEquTableMin:	; adc != table[min]
	MOV		A, #1
	SETB	C
	MOVC	A, @A+DPTR
	SUBB	A, R7
	CLR		A
	MOVC	A, @A+DPTR
	SUBB	A, R6
	JC		L_Calculate_Loop
							;	if(adc < table[min])
	DEC		min		;min--
	MOV		A, min
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2
	CLR		A
	MOVC	A, @A+DPTR
	MOV		R0, A		;(R0 R1) = table[min]
	MOV		A, #1
	MOVC	A, @A+DPTR
	MOV		R1, A

	MOV		A, min
	INC		A
	LCALL	F_GetFirstAddress	; DPTR = #temp_table + ACC * 2
	MOV		A, #1
	MOVC	A, @A+DPTR
	CLR		C
	SUBB	A, R1		; (R4 R5) = table[min+1] - table[min]
	MOV		R5, A
	CLR		A
	MOVC	A, @A+DPTR
	SUBB	A, R0
	MOV		R4, A
	
	MOV		A, R7
	CLR		C
	SUBB	A, R1	; (R0 R1) = adc - table[min]
	MOV		R1, A
	MOV		A, R6
	SUBB	A, R0
	MOV		R0, A
	
	MOV		A, R1
	MOV		B, #D_SCALE
	MUL		AB			;(R0 R1) = (adc - table[min]) * D_SCALE
	MOV		R1, A
	MOV		R0, B
	
	MOV		AR6, R0
	MOV		AR7, R1
	LCALL	F_DIV16_8	;(R6,R7)/R5 = (R6,R7), rest in R5,	use R0,R5 R6 R7,B,
						;(R6 R7) = ((adc - table[min]) * D_SCALE) / (table[min+1] - table[min])
	MOV		A, min
	MOV		B, #D_SCALE
	MUL		AB				;(B ACC) = min * D_SCALE
	
	ADD		A, R7
	MOV		R7, A
	MOV		A, B
	ADDC	A, #0		;(R6 R7) = (min * D_SCALE) + R7
	MOV		R6, A

	RET


F_DIV16_8:		;(R6,R7)/R5 = (R6,R7), rest in R5,	use R0,R5 R6 R7,B,
L_0032:
		MOV		A,R5
		MOV		R0,A
		MOV		B,A
		MOV		A,R6
		DIV		AB
		JB		OV,L_0057
		MOV		R6,A
		MOV		R5,B
		MOV		B,#8
L_0041:
		MOV		A,R7
		ADD		A,R7
		MOV		R7,A
		MOV		A,R5
		RLC		A
		MOV		R5,A
		JC		L_0050
		SUBB	A,R0
		JNC		L_0052
		DJNZ	B,L_0041
		RET
L_0050:
		CLR		C
		SUBB	A,R0
L_0052:
		MOV		R5,A
		INC		R7
		DJNZ	B,L_0041
L_0057:
		RET

		END

