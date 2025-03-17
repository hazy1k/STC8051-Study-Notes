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

;用户可以修改宏来选择时钟频率.

;用户可以在"用户定义宏"中选择共阴或共阳. 推荐尽量使用共阴数码管.

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;本例程使用5V版本的IAP15F2K61S2或STC15F2KxxS2。用户可以在"用户定义宏"中按具体的MCU修改掉电保存的EEPROM地址。

;显示效果为: 上电后显示秒计数, 计数范围为0~10000，显示在右边的5个数码管.

;当掉电后，MCU进入低压中断，对秒计数进行保存。MCU上电时读出秒计数继续显示。

;用户可以在"用户定义宏"中选择滤波电容大还是小。
;大的电容(比如1000uF)，则掉电后保持的时间长，可以在低压中断中擦除后(需要20多ms时间)然后写入。
;小的电容，则掉电后保持的时间短, 则必须在主程序初始化时先擦除.

;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
;注意：下载时，下载界面"硬件选项"中下面的项要固定如下设置:

;不勾选	允许低压复位(禁止低压中断)

;		低压检测电压 4.64V

;不勾选	低压时禁止EEPROM操作.
;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


;******************************************

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ	EQU	22118	;22118KHZ

STACK_POIRTER	EQU		0D0H	; 堆栈开始地址

LargeCapacitor	EQU		0		; 0: 滤波电容比较小,  1: 滤波电容比较大

LED_TYPE		EQU		000H	; 定义LED类型, 000H -- 共阴, 0FFH -- 共阳

Timer0_Reload	EQU		(65536 - Fosc_KHZ)	; Timer 0 中断频率, 1000次/秒

EE_ADDRESS		EQU		08000H	;保存的地址

DIS_DOT			EQU		020H
DIS_BLACK		EQU		010H
DIS_			EQU		011H

;*******************************************************************
;*******************************************************************

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

AUXR      DATA 08EH
P4        DATA 0C0H
P5        DATA 0C8H
ISP_DATA  DATA 0C2H
ISP_ADDRH DATA 0C3H
ISP_ADDRL DATA 0C4H
ISP_CMD   DATA 0C5H
ISP_TRIG  DATA 0C6H
ISP_CONTR DATA 0C7H

ELVD 	BIT IE.6	; 低压监测中断允许位
PLVD 	BIT IP.6	; 低压中断 优先级设定位

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

Test_cnt_H		DATA	3BH		; 测试用的秒计数变量
Test_cnt_L		DATA	3CH

address_H		DATA	3DH
address_L		DATA	3EH
length			DATA	3FH

;*******************************************************************
;*******************************************************************
		ORG		0000H				;reset
		LJMP	F_Main

		ORG		000BH				;1  Timer0 interrupt
		LJMP	F_Timer0_Interrupt

		ORG		0033H				;6  Low Voltage Detect interrupt
		LJMP	F_LVD_Interrupt



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
	
	MOV		R2, #200	; delay 200ms
L_PowerUpDelay:
	JNB		B_1ms, $	; 等待1ms到
	CLR		B_1ms
	DJNZ	R2, L_PowerUpDelay

	MOV		address_H, #HIGH EE_ADDRESS
	MOV		address_L, #LOW  EE_ADDRESS
	MOV		length, #2		;读出2字节
	MOV		R0, #Test_cnt_H
	LCALL	F_EEPROM_read_n		;读N个字节	;address_H,address_L,R0,length
	MOV		A, Test_cnt_L
	CLR		C
	SUBB	A, #LOW 10000
	MOV		A, Test_cnt_H
	SUBB	A, #HIGH 10000
	JC		L_LessThan10000
	MOV		Test_cnt_H, #0
	MOV		Test_cnt_L, #0
L_LessThan10000:

IF	(LargeCapacitor == 0)	;滤波电容比较小，电容保持时间比较短，则先擦除
	LCALL	F_EEPROM_SectorErase	; 擦除一个扇区
ENDIF
	
	LCALL	F_Display		; 显示秒计数
	
	ANL		PCON, #NOT (1 SHL 5)	; 低压检测标志清0
	SETB	ELVD		; 低压监测中断允许
	SETB	PLVD		; 低压中断 优先级高
	

;=================== 主循环 ==================================
L_MainLoop:
	JNB		B_1ms,  L_MainLoop		;1ms未到
	CLR		B_1ms
	
	INC		msecond_L		;msecond + 1
	MOV		A, msecond_L
	JNZ		$+4
	INC		msecond_H
	
	MOV		A, msecond_L	;msecond - 1000
	CLR		C
	SUBB	A, #LOW 1000
	MOV		A, msecond_H
	SUBB	A, #HIGH 1000
	JC		L_MainLoop		;if(msecond < 1000), jmp
	
	MOV		msecond_L, #0	;if(msecond >= 1000)
	MOV		msecond_H, #0

	INC		Test_cnt_L		;Test_cnt + 1
	MOV		A, Test_cnt_L
	JNZ		$+4
	INC		Test_cnt_H

	MOV		A, Test_cnt_L	;if(Test_cnt > 10000)	Test_cnt = 0;	秒计数范围为0~10000
	CLR		C
	SUBB	A, #LOW 10000
	MOV		A, Test_cnt_H
	SUBB	A, #HIGH 10000
	JC		L_LessThan10000A
	MOV		Test_cnt_H, #0
	MOV		Test_cnt_L, #0
L_LessThan10000A:
	LCALL	F_Display		; 显示秒计数

	LJMP	L_MainLoop

;**********************************************/


;========================================================================
; 函数: F_LVD_Interrupt
; 描述: 低压中断函数.
; 参数: non.
; 返回: non.
; 版本: V1.0, 2014-1-22
;========================================================================
F_LVD_Interrupt:
	PUSH	PSW
	PUSH	ACC
	PUSH	AR2

	CLR		P_HC595_SER		; 先关闭显示，省电
	MOV		R2, #16
L_LVD_ClearDisplay:
	SETB	P_HC595_SRCLK
	CLR		P_HC595_SRCLK
	DJNZ	R2, L_LVD_ClearDisplay
	SETB	P_HC595_RCLK
	CLR		P_HC595_RCLK		;锁存输出数据
	
	MOV		address_H, #HIGH EE_ADDRESS
	MOV		address_L, #LOW  EE_ADDRESS
IF	(LargeCapacitor == 1)	;滤波电容比较大，电容保持时间比较长(50ms以上)，则在中断里擦除
	LCALL	F_EEPROM_SectorErase	; 擦除一个扇区
ENDIF
	MOV		length, #2
	MOV		R0, #Test_cnt_H
	LCALL	F_EEPROM_write_n		;写N个字节	;address_H,address_L,R0,length

L_LVD_CheckLoop:		; 检测是否仍然低电压
	MOV		A, PCON
	ANL		PCON, #NOT (1 SHL 5)	; 低压检测标志清0
	JNB		ACC.5, L_QuitLVD_Init
	MOV		R2, #250
	DJNZ	R2, $			;延时一下
	SJMP	L_LVD_CheckLoop
L_QuitLVD_Init:

	POP		AR2
	POP		ACC
	POP		PSW
	RETI



;========================================================================
; 函数: F_HEX2_DEC
; 描述: 把双字节十六进制数转成十进制BCD数.
; 参数: (R6 R7): 要转换的双字节十六进制数.
; 返回: (R3 R4 R5) = BCD码.
; 版本: V1.0, 2013-10-22
;========================================================================
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

;========================================================================
; 函数: F_Display
; 描述: 显示计数函数.
; 参数: non.
; 返回: non.
; 版本: V1.0, 2014-1-22
;========================================================================
F_Display:
	MOV		R6, Test_cnt_H
	MOV		R7, Test_cnt_L
	LCALL	F_HEX2_DEC		;(R6 R7) HEX Change to DEC ---> (R3 R4 R5), use (R2~R7)
	MOV		LED8+3, R3	;万位
	MOV		A, R4
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
	
	MOV		R0, #LED8+3
L_Cut0_Loop:			;消高位0
	MOV		A,@R0
	JNZ		L_QuitCut0
	MOV		@R0, #DIS_BLACK
	INC		R0
	CJNE	R0, #LED8+7, L_Cut0_Loop
L_QuitCut0:	
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
	MOV		C, EA
	MOV		F0, C	;保存EA
L_EE_Read_Loop:
	CLR		EA		; 禁止中断
	MOV		ISP_TRIG, #0x5A			;ISP触发命令
	MOV		ISP_TRIG, #0xA5
	NOP
	MOV		C, F0
	MOV		EA, C		; 允许中断(如果允许)
	MOV		@R0, ISP_DATA

	MOV		A, ISP_ADDRL	;
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
	MOV		ISP_CMD, #3		;送扇区擦除命令
	MOV		C, EA
	MOV		F0, C	;保存EA
	CLR		EA		; 禁止中断
	MOV		ISP_TRIG, #0x5A			;ISP触发命令
	MOV		ISP_TRIG, #0xA5
	NOP
	MOV		C, F0
	MOV		EA, C		; 允许中断(如果允许)
	LCALL	F_DisableEEPROM
	RET

;========================================================================
; 函数: F_EEPROM_write_n
; 描述: 把缓冲的n个字节写入指定首地址的EEPROM.
; 参数: address_H,address_L:	写入EEPROM的首地址.
;       R0: 					写入源数据的缓冲的首地址.
;       length:      			写入的字节长度.
; 返回: non.
; 版本: V1.0, 2014-1-22
;========================================================================
F_EEPROM_write_n:
	PUSH	AR2
	MOV		R2, length
	MOV		ISP_ADDRH, address_H	; 送地址高字节（地址需要改变时才需重新送地址）
	MOV		ISP_ADDRL, address_L	; 送地址低字节
	MOV		ISP_CONTR, #(ISP_EN + ISP_WAIT_FREQUENCY)	; 设置等待时间，允许ISP/IAP操作，送一次就够
	MOV		ISP_CMD, #2		;送字节写命令，命令不需改变时，不需重新送命令
	MOV		C, EA
	MOV		F0, C	;保存EA
L_EE_W_Loop:
	MOV		ISP_DATA, @R0	; 送数据到ISP_DATA，只有数据改变时才需重新送
	CLR		EA		; 禁止中断
	MOV		ISP_TRIG, #0x5A			;ISP触发命令
	MOV		ISP_TRIG, #0xA5
	NOP
	MOV		C, F0
	MOV		EA, C		; 允许中断(如果允许)
	MOV		A, ISP_ADDRL	;
	ADD		A, #1
	MOV		ISP_ADDRL, A
	MOV		A, ISP_ADDRH
	ADDC	A, #0
	MOV		ISP_ADDRH, A
	INC		R0
	DJNZ	R2, L_EE_W_Loop

	LCALL	F_DisableEEPROM
	POP		AR2
	RET


	END



