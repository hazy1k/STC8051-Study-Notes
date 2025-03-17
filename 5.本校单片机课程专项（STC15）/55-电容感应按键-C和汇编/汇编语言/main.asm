
;/*---------------------------------------------------------------------*/
;/* --- STC MCU Limited ------------------------------------------------*/
;/* --- Mobile: (86)13922805190 ----------------------------------------*/
;/* --- Fax: 86-755-82905966 -------------------------------------------*/
;/* --- Tel: 86-755-82948412 -------------------------------------------*/
;/* --- Web: www.STCMCU.com --------------------------------------------*/
;/* 如果要在文章中应用此代码,请在文章中注明使用了宏晶科技的资料及程序   */
;/*---------------------------------------------------------------------*/

;/*************	功能说明	**************

;测试使用STC15W408AS的ADC做的电容感应触摸键.

;假定测试芯片的工作频率为24MHz

;******************************************/

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ		EQU	24000	;定义主时钟 KHZ

STACK_POIRTER	EQU		0D0H	;堆栈开始地址

Timer0_Reload	EQU		(65536 - Fosc_KHZ/600)	;Timer 0 重装值， 对应300KHZ


;*******************************************************************
;*******************************************************************
        
P1ASF     DATA 0x9D;	//只写，模拟输入选择
ADC_CONTR DATA 0xBC;	//带AD系列
ADC_RES   DATA 0xBD;	//带AD系列
ADC_RESL  DATA 0xBE;	//带AD系列
AUXR      DATA 0x8E;
AUXR2     DATA 0x8F;

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


/*************	本地常量声明	**************/

TOUCH_CHANNEL	EQU	8	;ADC通道数

ADC_90T		EQU (3 SHL 5)	;ADC时间 90T
ADC_180T	EQU (2 SHL 5)	;ADC时间 180T
ADC_360T	EQU (1 SHL 5)	;ADC时间 360T
ADC_540T	EQU 0			;ADC时间 540T
ADC_FLAG	EQU (1 SHL 4)	;软件清0
ADC_START	EQU (1 SHL 3)	;自动清0

/*************	本地变量声明	**************/
P_LED7	BIT P2.7;
P_LED6	BIT P2.6;
P_LED5	BIT P2.5;
P_LED4	BIT P2.4;
P_LED3	BIT P2.3;
P_LED2	BIT P2.2;
P_LED1	BIT P2.1;
P_LED0	BIT P2.0;

adc				EQU	30H	;	当前ADC值   	30H~3FH, 两字节一个值
adc_prev		EQU	40H	;	上一个ADC值 	40H~4FH, 两字节一个值
TouchZero		EQU	50H	;	0点ADC值    	50H~5FH, 两字节一个值
TouchZeroCnt	EQU	60H	;	0点自动跟踪计数	60H~67H

cnt_250ms		DATA 68H	;



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
	
	MOV		R0, #1
L_ClearRamLoop:			;清除RAM
	MOV		@R0, #0
	INC		R0
	MOV		A, R0
	CJNE	A, #0FFH, L_ClearRamLoop
	
	MOV		SP, #STACK_POIRTER
	MOV		PSW, #0
	USING	0		;选择第0组R0~R7

;================= 用户初始化程序 ====================================
	MOV		R7, #50
	LCALL	F_delay_ms

	CLR		ET0				; 初始化Timer0输出一个300KHZ时钟
	CLR		TR0	;
	ORL		AUXR,  #080H	; Timer0 set as 1T mode
	ORL		AUXR2, #01H		; 允许输出时钟
	MOV		TMOD,  #0		; Timer0 set as Timer, 16 bits Auto Reload.
	MOV		TH0, #HIGH Timer0_Reload
	MOV		TL0, #LOW  Timer0_Reload;
	SETB	TR0

	LCALL	F_ADC_init
	MOV		R7, #50
	LCALL	F_delay_ms

	MOV		R0, #adc_prev	; 初始化上一个ADC值
L_Init_Loop1:
	MOV		@R0, #03H
	INC		R0
	MOV		@R0, #0FFH
	INC		R0
	MOV		A, R0
	CJNE	A, #(adc_prev + TOUCH_CHANNEL * 2), L_Init_Loop1

	MOV		R0, #TouchZero	; 初始化0点ADC值
L_Init_Loop2:
	MOV		@R0, #03H
	INC		R0
	MOV		@R0, #0FFH
	INC		R0
	MOV		A, R0
	CJNE	A, #(TouchZero + TOUCH_CHANNEL * 2), L_Init_Loop2

	MOV		R0, #TouchZeroCnt	; 初始化自动跟踪计数值
L_Init_Loop3:
	MOV		@R0, #0
	INC		R0
	MOV		A, R0
	CJNE	A, #(TouchZeroCnt + TOUCH_CHANNEL), L_Init_Loop3

	MOV		cnt_250ms, #5

;=================== 主循环 ==================================
L_MainLoop:
	MOV		R7, #50			;延时50ms
	LCALL	F_delay_ms
	LCALL	F_ShowLED		; 处理一次触摸键值
	DJNZ	cnt_250ms, L_MainLoop
	
	MOV		cnt_250ms, #5	;250ms处理一次0点自动跟踪
	LCALL	F_AutoZero		;自动跟踪零点

	SJMP	L_MainLoop

;=================== 主程序结束 ==================================



; /*************  ADC初始化函数 *****************/
F_ADC_init:
	MOV  	P1ASF,#0FFH		;8路ADC
	MOV  	ADC_CONTR,#080H	;允许ADC
	RET  	
; END OF ADC_init


; //========================================================================
; // 函数: F_Get_ADC10bitResult
; // 描述: 查询法读一次ADC结果.
; // 参数: R7: 选择要转换的ADC.
; // 返回: R6 R7 == 10位ADC结果.
; // 版本: V1.0, 2014-3-25
; //========================================================================

F_Get_ADC10bitResult:
	USING	0		;选择第0组R0~R7

	MOV  	ADC_RES, #0
	MOV  	ADC_RESL,#0
	MOV  	A, R7
	ORL  	A, #0E8H	;(0x80 OR ADC_90T OR ADC_START)	;触发ADC
	MOV  	ADC_CONTR, A
	NOP  	
	NOP  	
	NOP  	
	NOP  	

L_10bitADC_Loop1:
	MOV  	A, ADC_CONTR
	JNB  	ACC.4, L_10bitADC_Loop1		;等待ADC转换结束

	MOV  	ADC_CONTR,#080H		//清除标志
	MOV  	A,ADC_RES
	MOV  	B,#04H
	MUL  	AB
	MOV  	R7,A
	MOV  	R6,B
	MOV  	A, ADC_RESL
	ANL  	A, #03H
	ORL  	A,R7
	MOV  	R7,A
	RET
; END OF _Get_ADC10bitResult



; /********************* 自动0点跟踪函数 *************************/
F_AutoZero:			;250ms调用一次 这是使用相邻2个采样的差的绝对值之和来检测。
	USING	0		;选择第0组R0~R7

	CLR  	A
	MOV  	R5,A
L_AutoZero_Loop:
			;[R6 R7] = adc[i],  (j = adc[i])
	MOV  	A,R5
	ADD  	A,ACC
	ADD  	A,#LOW (adc)
	MOV  	R0,A
	MOV  	A,@R0
	MOV  	R6,A
	INC  	R0
	MOV  	A,@R0
	MOV  	R7,A

			; 计算差值 [R2 R3] = adc[i] - adc_prev[i],   (k = j - adc_prev[i];)	//减前一个读数
	MOV  	A,R5
	ADD  	A,ACC
	ADD  	A,#LOW (adc_prev+01H)
	MOV  	R0,A
	CLR  	C
	MOV  	A,R7
	SUBB 	A,@R0
	MOV  	R3,A
	MOV  	A,R6
	DEC  	R0
	SUBB 	A,@R0
	MOV  	R2,A

			; 求差值的绝对值 [R2 R3],  if(k & 0x8000)	F0 = 1,	k = 0 - k;	//释放	求出两次采样的差值
	CLR  	F0		;按下
	JNB  	ACC.7, L_AutoZero_1
	SETB 	F0
	CLR		C
	CLR		A
	SUBB	A, R3
	MOV		R3, A
	MOV  	A,R3
	CLR		A
	SUBB	A, R2
	MOV		R2, A

L_AutoZero_1:
	CLR  	C		;计算 [R2 R3] - #20,  if(k >= 20)	//变化比较大
	MOV  	A,R3
	SUBB 	A,#20
	MOV  	A,R2
	SUBB 	A,#00H
	JC   	L_AutoZero_2	;[R2 R3] , 20, 转

	MOV  	A,#LOW (TouchZeroCnt)	;如果变化比较大，则清0计数器	TouchZeroCnt[i] = 0;
	ADD  	A,R5
	MOV  	R0,A
	MOV  	@R0, #0

			; 	if(F0)	TouchZero[i] = j;		//如果是释放，并且变化比较大，则直接替代
	JNB  	F0,L_AutoZero_3
	MOV  	A,R5
	ADD  	A,ACC
	ADD  	A,#LOW (TouchZero)
	MOV  	R0,A
	MOV  	@R0,AR6
	INC  	R0
	MOV  	@R0,AR7
	SJMP 	L_AutoZero_3

L_AutoZero_2:		;变化比较小，则蠕动，自动0点跟踪
					; 	if(++TouchZeroCnt[i] >= 20)		//连续检测到小变化20次/4 = 5秒.
	MOV  	A,#LOW (TouchZeroCnt)
	ADD  	A,R5
	MOV  	R0,A
	INC  	@R0
	MOV  	A,@R0
	CLR  	C
	SUBB 	A,#20
	JC   	L_AutoZero_3	;if(TouchZeroCnt[i] < 20), 转

	MOV  	@R0, #0		;TouchZeroCnt[i] = 0;

	MOV  	A,R5		;TouchZero[i] = adc_prev[i];	//变化缓慢的值作为0点
	ADD  	A,ACC
	ADD  	A,#LOW (adc_prev)
	MOV  	R0,A
	MOV  	A,@R0
	MOV  	R2,A
	INC  	R0
	MOV  	A,@R0
	MOV  	R3,A
	MOV  	A,R5
	ADD  	A,ACC
	ADD  	A,#LOW (TouchZero)
	MOV  	R0,A
	MOV  	@R0,AR2
	INC  	R0
	MOV  	@R0,AR3

L_AutoZero_3:
			; 	保存采样值	adc_prev[i] = j;
	MOV  	A,R5
	ADD  	A,ACC
	ADD  	A,#LOW (adc_prev)
	MOV  	R0,A
	MOV  	@R0,AR6
	INC  	R0
	MOV  	@R0,AR7

	INC  	R5
	MOV  	A,R5
	XRL  	A,#08H
	JZ   	$ + 5H
	LJMP 	L_AutoZero_Loop
	RET  	
; END OF AutoZero



; /********************* 获取触摸信息函数 50ms调用1次 *************************/

F_check_adc:		;判断键按下或释放,有回差控制
	USING	0		;选择第0组R0~R7

	MOV  	R4, AR7
			; 	adc[index] = 1023 - Get_ADC10bitResult(index);		//获取ADC值, 转成按下键, ADC值增加
	LCALL	F_Get_ADC10bitResult	;返回的ADC值在 [R6 R7]
	CLR  	C
	MOV  	A,#0FFH		;1023 - [R6 R7]
	SUBB 	A,R7
	MOV  	R7,A
	MOV  	A,#03H
	SUBB 	A,R6
	MOV  	R6,A
	
	MOV  	A,R4		;保存 adc[index]
	ADD  	A,ACC
	ADD  	A,#LOW (adc)
	MOV  	R0,A
	MOV  	@R0,AR6
	INC  	R0
	MOV  	@R0,AR7

			; 	if(adc[index] < TouchZero[index])	return	0;	//比0点还小的值，则认为是键释放
	MOV  	A,R4
	ADD  	A,ACC
	ADD  	A,#LOW (TouchZero+01H)
	MOV  	R1,A
	MOV  	A,R4
	ADD  	A,ACC
	ADD  	A,#LOW (adc)
	MOV  	R0,A
	MOV  	A,@R0
	MOV  	R6,A
	INC  	R0
	MOV  	A,@R0
	CLR  	C
	SUBB 	A,@R1		;计算 adc[index] - TouchZero[index]
	MOV  	A,R6
	DEC  	R1
	SUBB 	A,@R1
	JNC  	L_check_adc_1	;if(adc[index] >= TouchZero[index]), 转
	MOV  	R7,#00H			;if(adc[index] < TouchZero[index]), 比0点还小的值，则认为是键释放, 返回0
	RET  	

L_check_adc_1:
			; 计算差值 	[R6 R7] = delta = adc[index] - TouchZero[index];
	MOV  	A,R4
	ADD  	A,ACC
	ADD  	A,#LOW (TouchZero+01H)
	MOV  	R1,A
	MOV  	A,R4
	ADD  	A,ACC
	ADD  	A,#LOW (adc+01H)
	MOV  	R0,A
	CLR  	C
	MOV  	A,@R0
	SUBB 	A,@R1
	MOV  	R7,A
	DEC  	R0
	MOV  	A,@R0
	DEC  	R1
	SUBB 	A,@R1
	MOV  	R6,A

			;---- Variable 'delta' assigned to Register 'R6/R7' ----
	CLR  	C
	MOV  	A,R7
	SUBB 	A,#40
	MOV  	A,R6
	SUBB 	A,#00H
	JC   	L_check_adc_2	;if(delta < 40), 转
	MOV  	R7,#1			;if(delta >= 40)	return 1;	//键按下  返回1
	RET  	

L_check_adc_2:
	SETB 	C
	MOV  	A,R7
	SUBB 	A,#20
	MOV  	A,R6
	SUBB 	A,#00H
	JNC  	L_check_adc_3
	MOV  	R7,#0	;if(delta <= 20)	return 0;	//键释放  返回0
	RET  	
L_check_adc_3:
	MOV  	R7,#2	;if((delta > 20) && (delta < 40))	保持原状态  返回2
	RET  	
; END OF _check_adc


/********************* 键处理 50ms调用1次 *************************/

F_ShowLED:
	USING	0		;选择第0组R0~R7

	MOV  	R7, #0
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck0
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED0, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck0:

	MOV  	R7, #1
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck1
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED1, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck1:

	MOV  	R7, #2
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck2
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED2, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck2:

	MOV  	R7, #3
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck3
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED3, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck3:

	MOV  	R7, #4
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck4
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED4, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck4:

	MOV  	R7, #5
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck5
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED5, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck5:

	MOV  	R7, #6
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck6
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED6, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck6:

	MOV  	R7, #7
	LCALL	F_check_adc
	MOV  	A,R7
	ANL		A, #0FEH
	JNZ  	L_QuitCheck7
	MOV		A, R7
	MOV		C, ACC.0
	CPL		C
	MOV		P_LED7, C	;if(i == 0) 指示灯灭, if(i == 1) 指示灯亮
L_QuitCheck7:

	RET  	
; END OF ShowLED


;//========================================================================
;// 函数: F_delay_ms
;// 描述: 延时子程序。
;// 参数: R7: 延时ms数.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
F_delay_ms:
	PUSH	AR3		;入栈R3
	PUSH	AR4		;入栈R4

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
	
	DJNZ	R7, L_delay_ms_1

	POP		AR4		;出栈R2
	POP		AR3		;出栈R3
	RET


;*******************************************************************
;**************** 中断函数 ***************************************************

F_Timer0_Interrupt:
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

	END
	
