

/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/

/****************************
本示例在Keil开发环境下请选择Intel的8052芯片型号进行编译

本例程MCU的工作频率为22.1184MHz.

使用MCU自带的比较器进行ADC转换, 并通过串口输出结果. 用定时器0产生10us中断查询比较器的状态.

使用比较器做ADC, 原理图如下.
做ADC的原理是基于电荷平衡的计数式ADC.
电压从Vin输入, 通过100K+104滤波, 进入比较器的P5.5正输入端, 经过比较器的比较, 将结果输出到P1.2再通过100K+104滤波后
送比较器P5.4负输入端,跟输入电压平衡.
设置两个变量: 计数周期(量程)adc_duty 和 比较结果高电平的计数值 adc, adc严格比例于输入电压.
ADC的基准就是P1.2的高电平. 如果高电平准确,比较器的放大倍数足够大,则ADC结果会很准确.
当比较结果为高电平,则P1.2输出1, 并且adc+1.
当比较结果为低电平,则P1.2输出0, adc值不变.
每一次比较都判断计数周期是否完成,完成则adc里的值就是ADC结果.
电荷平衡计数式ADC的性能类似数字万用表用的双积分ADC, 当计数周期为20ms的倍数时,具有很强的抗工频干扰能力,很好的线性和精度.
原理可以参考ADD3501(3 1/2位数字万用表)或ADD3701(3 3/4位数字万用表), 也可以参考AD7740 VFC电路.

例: 比较一次的时间间隔为10us, 量程为10000, 则做1次ADC的时间为100ms. 比较器的响应时间越短, 则完成ADC就越快.

由于要求每次比较时间间隔都要相等,所以用C编程最好在定时器中断里进行, 定时器设置为自动重装, 高优先级中断, 其它中断均低优先级.
用汇编的话, 保证比较输出电平处理的时间要相等.

                                         100K
                            /| P5.5       ___
                    P1.2   /+|---------o-|___|- ------- Vin
                     .----<  | P5.4    |
                     |     \-|---.     |
                     |      \|   |     |
                     |           |     |
                     |    ___    |     |
                     '---|___|---o     |
                        100K     |     |
                                ---   ---
                                ---   ---
                            104  |     |  104
                                 |     |
                                ===   ===
                                GND   GND

******************************/


;*************	本地常量声明	**************/
AUXR        EQU     08EH        //辅助寄存器
T2H         DATA    0D6H        //定时器2高8位
T2L         DATA    0D7H        //定时器2低8位

P_SW1       EQU     0A2H        //外设功能切换寄存器1
S1_S0       EQU     40H         //P_SW1.6
S1_S1       EQU     80H         //P_SW1.7

P1M1		EQU 091H	;	//P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull		实际上1T的都一样
P1M0		EQU 092H	;	//					=10--->pure input,	11--->open drain
P5M1		EQU 0C9H	;	//	P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
P5M0		EQU 0CAH	;	//					=10--->pure input,	11--->open drain

CMPCR1 		EQU 0E6H
CMPCR2 		EQU 0E7H


;CMPCR1
CMPEN	EQU	080H	//1: 允许比较器, 0: 禁止,关闭比较器电源
CMPIF	EQU	040H;	//比较器中断标志, 包括上升沿或下降沿中断, 软件清0
PIE		EQU	020H;	//1: 比较结果由0变1, 产生上升沿中断
NIE		EQU	010H;	//1: 比较结果由1变0, 产生下降沿中断
PIS		EQU	008H;	//输入正极性选择, 0: 选择内部P5.5做正输入,           1: 由ADCIS[2:0]所选择的ADC输入端做正输入.
NIS		EQU	004H;	//输入负极性选择, 0: 选择内部BandGap电压BGv做负输入, 1: 选择外部P5.4做输入.
CMPOE	EQU	002H;	//1: 允许比较结果输出到P1.2, 0: 禁止.
CMPRES	EQU	001H;	//比较结果, 1: CMP+电平高于CMP-,  0: CMP+电平低于CMP-,  只读

;CMPCR2
INVCMPO	EQU	080H;	//1: 比较器输出取反,  0: 不取反
DISFLT	EQU	040H;	//1: 关闭0.1uF滤波,   0: 允许
LCDTY	EQU	000H;	//0~63, 比较结果变化延时周期数

TIM_16BitAutoReload			EQU	0
TIM_16Bit					EQU	1
TIM_8BitAutoReload			EQU	2
TIM_16BitAutoReloadNoMask	EQU	3


;*************	本地变量声明	**************/



ADC_SCALE	EQU	50000		;ADC满量程, 根据需要设置

P_ADC		BIT	P1.2;			//P1.2 比较器转IO输出端
adc_h		DATA	30H;		//ADC中间值, 用户层不可见
adc_l		DATA	31H;
adc_duty_h	DATA	32H;		//ADC计数周期, 用户层不可见
adc_duty_l	DATA	33H;
adc_value_h	DATA	34H;		//ADC值, 用户层使用
adc_value_l	DATA	35H

FALG		DATA	20H
adc_ok		BIT		FALG.0;		//ADC结束标志, 为1则adc_value的值可用. 此标志给用户层查询,并且清0



		ORG		00H				;reset
		LJMP	F_MAIN_FUNC

		ORG		03H				;INT0 interrupt
	;	LJMP	F_INT0_interrupt      
		RETI

		ORG		0BH				;Timer0 interrupt
		LJMP	F_Timer0_interrupt
		RETI

		ORG		13H				;INT1 interrupt
	;	LJMP	F_INT1_interrupt      

		ORG		1BH				;Timer1 interrupt
	;	LJMP	F_Timer1_interrupt
		RETI

		ORG		00023H
	;	LJMP	F_UART0_RCV
		RETI


;******************** 主程序 **************************/
F_MAIN_FUNC:

	MOV		SP,#80H

;变量初始化
	MOV  	adc_duty_l,#LOW  ADC_SCALE	;周期计数赋初值
	MOV  	adc_duty_h,#HIGH ADC_SCALE
	MOV  	adc_l,#0					;清除ADC值
	MOV  	adc_h,#0
	MOV		adc_value_h,#0				;ADC值, 用户层使用
	MOV		adc_value_l,#0
	MOV		FALG,#0



;IO口初始化
	ANL		P1M1,#NOT 00000100B	;P1.2 push-pull
	ORL		P1M0,# 00000100B
	ORL		P5M1,# 00110000B		;P5.4 P5.5 pure input
	ANL		P5M0,#NOT 00110000B

;比较器初始化
	MOV		CMPCR1,#0
	MOV		CMPCR2,#20;			//比较结果变化延时周期数, 0~63
	ORL		CMPCR1,#CMPEN;		//允许比较器		ENABLE,DISABLE
;	ORL		CMPCR1,#PIE;		//允许上升沿中断	ENABLE,DISABLE
;	ORL		CMPCR1,#NIE;		//允许下降沿中断	ENABLE,DISABLE
;	ORL		CMPCR1,#PIS;		//输入正极性选择, 0: 选择内部P5.5做正输入,           1: 由ADCIS[2:0]所选择的ADC输入端做正输入.
	ORL		CMPCR1,#NIS;		//输入负极性选择, 0: 选择内部BandGap电压BGv做负输入, 1: 选择外部P5.4做输入
;	ORL		CMPCR1,#CMPOE;		//允许比较结果输出到P1.2,   ENABLE,DISABLE
;	ORL		CMPCR2,#INVCMPO;	//比较器输出取反, 	ENABLE,DISABLE
	ORL		CMPCR2,#DISFLT;		//内部0.1uF滤波,  	ENABLE,DISABLE

;定时器0 初始化
	MOV		A,TMOD
	ANL		A,#NOT 00FH;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
	ORL		A,TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	MOV		TMOD,A
	ORL		AUXR,#080H;	//1T
	SETB	ET0;	//允许中断
	SETB	PT0;	//高优先级中断

	MOV		TH0,#HIGH (-221);		//重装值	100KHZ, 10us,	65536 - (MAIN_Fosc)/100000
	MOV		TL0,#LOW  (-221);
	SETB	TR0;	//开始运行

;串口1初始化
    MOV   A,P_SW1
    ANL   A,#03FH               //S1_S0=0 S1_S1=0
    MOV   P_SW1,A               //(P3.0/RxD, P3.1/TxD)
    
//  MOV   A,P_SW1
//  ANL   A,#03FH               //S1_S0=1 S1_S1=0
//  ORL   A,#S1_S0              //(P3.6/RxD_2, P3.7/TxD_2)
//  MOV   P_SW1,A
//  
//  MOV   A,P_SW1
//  ANL   A,#03FH               //S1_S0=0 S1_S1=1
//  ORL   A,#S1_S1              //(P1.6/RxD_3, P1.7/TxD_3)
//  MOV   P_SW1,A

    MOV   SCON,#50H             //8位可变波特率

    MOV		T2L,#LOW  (-576)		//设置波特率重装值(65536-22118400/4/9600)
    MOV		T2H,#HIGH (-576)
    MOV		A,AUXR
	ANL		A,#NOT 01DH
	ORL		AUXR,#14H				//T2为1T模式, 并启动定时器2
    ORL		AUXR,#01H				//选择定时器2为串口1的波特率发生器
//  SETB	ES					//使能串口中断

	SETB	EA;		//中断全局允许

	MOV		DPTR,#TestString1	//发送测试字符串
	LCALL	F_SendSring

L_MainLoop:
	JNB		adc_ok,$			;等待转换结束
	CLR		adc_ok;				//清除ADC已结束标志
	MOV		DPTR,#StringADC		//发送测试字符串
	LCALL	F_SendSring

	MOV		R3,adc_value_h
	MOV		R4,adc_value_l
	LCALL	F_HEX2_DEC		;二进制转十进制, (R3---R4) HEX Change to DEC ---> (R5---R7), use (R2~R7),	344T
	
	MOV		A,R5			;BCD码展开并转成ASCII码
	ADD		A,#'0'
	MOV		R3,A
	MOV		A,R6
	SWAP	A
	ANL		A,#0FH
	ADD		A,#'0'
	MOV		R4,A
	MOV		A,R6
	ANL		A,#0FH
	ADD		A,#'0'
	MOV		R5,A
	MOV		A,R7
	SWAP	A
	ANL		A,#0FH
	ADD		A,#'0'
	MOV		R6,A
	MOV		A,R7
	ANL		A,#0FH
	ADD		A,#'0'
	MOV		R7,A

	MOV		A,R3	;消无效0
	CJNE	A,#'0',L_Cut_0_end
	MOV		R3,#' '
	MOV		A,R4
	CJNE	A,#'0',L_Cut_0_end
	MOV		R4,#' '
	MOV		A,R5
	CJNE	A,#'0',L_Cut_0_end
	MOV		R5,#' '
	MOV		A,R6
	CJNE	A,#'0',L_Cut_0_end
	MOV		R6,#' '
L_Cut_0_end:
	MOV		A,R3			//发串口
	LCALL	F_TxSendByte
	MOV		A,R4
	LCALL	F_TxSendByte
	MOV		A,R5
	LCALL	F_TxSendByte
	MOV		A,R6
	LCALL	F_TxSendByte
	MOV		A,R7
	LCALL	F_TxSendByte
	MOV		A,#' '
	LCALL	F_TxSendByte
	MOV		A,#' '
	LCALL	F_TxSendByte
	
	MOV		A,#0x0d			;换行
	LCALL	F_TxSendByte
	MOV		A,#0x0a
	LCALL	F_TxSendByte

	LJMP	L_MainLoop		;循环




TestString1:
    DB  0dh,0ah,"使用比较器做ADC例子",0dh,0ah,0
StringADC:
    DB  "ADC = ",0

;********************* 串口发送一个字节 ************************/
; 描述: 串口发送一个字节。9600，N，8，1
; 参数: ACC: 要发送的数据字节.
; 返回: none.
F_TxSendByte:	;ACC--要发送的字节,
	CLR		TI
	MOV		SBUF,A
	JNB		TI,$
	CLR		TI
	RET

;********************* 发送字符串 ****************************
F_SendSring:
	CLR		A
	MOVC	A,@A+DPTR			//读取字符
	JZ		L_QuitSendString	//检测字符串结束标志
	INC		DPTR				//字符串地址+1
	LCALL	F_TxSendByte		//发送当前字符
	SJMP	F_SendSring
L_QuitSendString:
    RET

;***************************************************************************
F_HEX2_DEC:        	;(R3---R4) HEX Change to DEC ---> (R5---R7), use (R2~R7),	344T
       
		MOV  	R2,#16
		MOV  	R5,#0
		MOV  	R6,#0
		MOV  	R7,#0

L_HEX2_DEC:
		CLR  	C
		MOV  	A,R4
		RLC  	A
		MOV  	R4,A
		MOV  	A,R3
		RLC  	A
		MOV  	R3,A

		MOV  	A,R7
		ADDC 	A,R7
		DA   	A
		MOV  	R7,A

		MOV  	A,R6
		ADDC 	A,R6
		DA   	A
		MOV  	R6,A

		MOV  	A,R5
		ADDC 	A,R5
		DA   	A
		MOV  	R5,A

		DJNZ 	R2,L_HEX2_DEC
		RET


;********************* Timer0中断函数************************/
F_Timer0_interrupt:
	PUSH	PSW
	PUSH	ACC
	
	MOV		A,CMPCR1
	JNB		ACC.0,L_CMP_Out0

	SETB	P_ADC		;比较器输出高电平,	则P_ADC输出高电平, 给负输入端做反馈. 
	INC		adc_l
	MOV		A,adc_l		;ADC计数+1
	JNZ		L_ADC_CheckDuty
	INC		adc_h
	SJMP	L_ADC_CheckDuty

L_CMP_Out0:				;比较器输出低电平
	CLR		P_ADC		;P_ADC输出低电平, 给负输入端做反馈. 

L_ADC_CheckDuty:
	MOV  	A,adc_duty_l	;ADC周期-1, 到0则ADC结束
	DEC  	adc_duty_l
	JNZ  	L_ADC_CheckDuty1
	DEC  	adc_duty_h
L_ADC_CheckDuty1:
	DEC  	A
	ORL  	A,adc_duty_h
	JNZ  	L_ADC_CheckDuty2

	MOV  	adc_duty_l,#LOW  ADC_SCALE	;周期计数赋初值
	MOV  	adc_duty_h,#HIGH ADC_SCALE
	MOV  	adc_value_l,adc_l	;保存ADC值
	MOV  	adc_value_h,adc_h
	MOV  	adc_l,#0			;清除ADC值
	MOV  	adc_h,#0
	SETB 	adc_ok				;标志ADC已结束

L_ADC_CheckDuty2:
	POP  	ACC
	POP		PSW
	RETI 	

	END
	

	
