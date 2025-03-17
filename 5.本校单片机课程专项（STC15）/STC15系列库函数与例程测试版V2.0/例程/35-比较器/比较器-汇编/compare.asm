
;/*------------------------------------------------------------------*/
;/* --- STC MCU International Limited -------------------------------*/
;/* --- STC 1T Series MCU RC Demo -----------------------------------*/
;/* --- Mobile: (86)13922805190 -------------------------------------*/
;/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
;/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
;/* --- Web: www.GXWMCU.com -----------------------------------------*/
;/* --- QQ:  800003751 ----------------------------------------------*/
;/* If you want to use the program or the program referenced in the  */
;/* article, please specify in which data and procedures from STC    */
;/*------------------------------------------------------------------*/



P1M1	DATA	091H	; P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull		实际上1T的都一样
P1M0	DATA	092H	; 					=10--->pure input,	11--->open drain

CMPCR1	DATA	0E6H

CMPEN	EQU	080H	; 1: 允许比较器, 0: 禁止,关闭比较器电源
CMPIF	EQU	040H	; 比较器中断标志, 包括上升沿或下降沿中断, 软件清0
PIE		EQU	020H	; 1: 比较结果由0变1, 产生上升沿中断
NIE		EQU	010H	; 1: 比较结果由1变0, 产生下降沿中断
PIS		EQU	008H	; 输入正极性选择, 0: 选择内部P5.5做正输入,           1: 由ADCIS[2:0]所选择的ADC输入端做正输入.
NIS		EQU	004H	; 输入负极性选择, 0: 选择内部BandGap电压BGv做负输入, 1: 选择外部P5.4做输入.
CMPOE	EQU	002H	; 1: 允许比较结果输出到P1.2, 0: 禁止.
CMPRES	EQU	001H	; 比较结果, 1: CMP+电平高于CMP-,  0: CMP+电平低于CMP-,  只读

CMPCR2	DATA	0E7H
INVCMPO	EQU	080H	; 1: 比较器输出取反,  0: 不取反
DISFLT	EQU	040H	; 1: 关闭0.1uF滤波,   0: 允许
LCDTY	EQU	000H	;	0~63, 比较结果变化延时周期数


	ORG	000H
	LJMP	MAIN
	
	ORG	00ABH
	LJMP	CMP_ISR
	
	
	ORG	100H
MAIN:
	MOV		P1M1,#0
	MOV		P1M0,#0x04	;P1.2 push-pull output
	
	MOV		CMPCR1,#0	;
	MOV		CMPCR2,#0	; 比较结果变化延时周期数, 0~63
;	ORL		CMPCR1,#PIE	; 1: 比较结果由0变1, 产生上升沿中断
;	ORL		CMPCR1,#NIE	; 1: 比较结果由1变0, 产生下降沿中断
;	ORL		CMPCR1,#PIS	; 输入正极性选择, 0: 选择外部P5.5做正输入, 	1: 由ADCIS[2:0]所选择的ADC输入端做正输入.
	ORL		CMPCR1,#NIS	; 输入负极性选择, 0: 选择内部BandGap电压BGv做负输入,1: 选择外部P5.4做输入.
	ORL		CMPCR1,#CMPOE	; 1: 允许比较结果输出到P1.2, 0: 禁止
	ORL		CMPCR2,#INVCMPO	; 1: 比较器输出取反,  0: 不取反
;	ORL		CMPCR2,#DISFLT	; 1: 关闭0.1uF滤波,   0: 允许
	ORL		CMPCR1,#CMPEN	; 1: 允许比较器, 0: 禁止,关闭比较器电源

	SETB	EA
L_Loop:
CPL   P1.3	
	SJMP	L_Loop
	SJMP	$
	


;================== 比较器中断函数 =======================
CMP_ISR:
	PUSH	PSW
	PUSH	ACC
	
	ANL		CMPCR1,#NOT CMPIF	;清除标志

	CPL		P1.3		;有中断,取反P1.3,示波器观察

	MOV		A,CMPCR1
	JNB		ACC.0,L_CmpOutLow
	CPL		P1.4		;上升沿中断, 取反P1.4
	SJMP	L_QuitCmpISR

L_CmpOutLow:
	CPL		P1.5		;下降沿中断, 取反P1.5

L_QuitCmpISR:
	POP		ACC
	POP		PSW
	RETI
;==================	比较器中断函数 end ======================

	END
	
	
	
	