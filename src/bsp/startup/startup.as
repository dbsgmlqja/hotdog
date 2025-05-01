;/***************************************************************/
;/*	Initialize routine for MN10300 Series						*/
;/* Filename	: startup.asm									*/
;/* Contents	: Initialize and Interrupt setup sample			*/
;/* Functions	: startup										*/
;/*		  system setup routine									*/
;/* Division	: Panasonic Corporation							*/
;/* Date	: 2013.04											*/
;/***************************************************************/

; control registers
IVAR0		equ	0x00008000
IVAR1		equ	0x00008004
IVAR2		equ	0x00008008
IVAR3		equ	0x0000800C
IVAR4		equ	0x00008010
IVAR5		equ	0x00008014
IVAR6		equ	0x00008018
IVBR		equ	0x0000801C

CPUM		equ	0x00008040
PRTCPUM		equ	0x00008048
CHWAIT		equ	0x3B100002
CHCTR		equ	0x3B100000

NMICR		equ	0x00008900
IAGR		equ	0x00008A00

PCNT		equ	0x0000A808
PRTCKCNT	equ	0x0000A820
CKCTR		equ	0x00008280
PRTCKGEN	equ	0x00008290

WDCTR 		equ	0x00008202
PRTWDT		equ	0x00008210

WD2CTR		equ	0x0000A850
WD2MDSEL	equ	0x0000A854
PRTWDT2		equ	0x0000A828
;
IRQ_DISABLE	equ	0x0700			;IRQ disable (set mask level 7)

; Section
.text		SECTION "ax",PROGBITS,PUBLIC		;text area
.rodata		SECTION "a",PROGBITS,PUBLIC			;constant area
	align	4
.MN.data	SECTION "aw",NOBITS,PUBLIC			;initialized data area
	align	4
.MN.romdata	SECTION "a",PROGBITS,PUBLIC			;ROM data area
	align	4
.MN.romdataend	SECTION "a",NOBITS,PUBLIC		;end of ROM data area
	align	4
.bss		SECTION "aw",NOBITS,PUBLIC			;non-initialized external data area
	align	4
.MN.bssend	SECTION "aw",NOBITS,PUBLIC			;end of non-initialized data area
	align	4

;****************************RAMEX additional code************************************************ 
_RAMEX_END	SECTION "ax",PROGBITS,PUBLIC	; 28k Byte end address
	align	4
	global	__MN_load_top_ramex		;
	global	__MN_load_end_ramex		;
	global	__MN_top_ramex 	   		;
;*************************************************************************************************

.text           SECTION
 
		global	_main, _0main, _irq_vct_tbl
		global	_enable_irq, _0enable_irq
		global	_disable_irq, _0disable_irq
		global	_set_imask, _0set_imask
		global	_int_WDT


RAM_SIZE	equ	0x2000			;RAM size 16kbyte(MN103HF38X/28X)
STACK_SIZE	equ	0x0400			;stack size 1kbyte ??
PLL_SEL		equ	0x0009			; PLL multiply select 	: 0x0000= 3 multiply, 0x0001= 3.75multiply, 0x0002= 5 multiply, 0x0003= 6multiply,
									;	0x0004= 6 multiply, 0x0005= 7.5multiply, 0x0006= 7.5 multiply, 0x0007= 10multiply,
									;	0x0008= 10 multiply, 0x0009= 12multiply, 0x000A= 12 multiply, 0x000B= 15multiply,
									;	0x000C= 15 multiply, 0x000D= 20multiply, 0x000E= 24 multiply, 0x000F= 30multiply
PLL_ON		equ	0x0010			; PLL ON
PLL_OUT		equ	0x0020			; PLL OUT

IOCLK_SEL	equ	0x0020			; IOCLK frequency select: 0x0010= 1/8, 0x0020= 1/4, 0x0030= 1/2

WAIT_200US	equ	200			; Plase select 0.0002 * (oscillating frequency /2) / 5.
						; (WARNING :It might actually take time more than 200us by the alignment.)	??


.bss		SECTION "aw", NOBITS,PUBLIC
		ALIGN 4

stack		ds	STACK_SIZE
stack_end

.text		SECTION "ax", PROGBITS,PUBLIC
	
	ALIGN 1
_reset								;branch here after reset (set 0x40000000 when linking!)
		jmp	_start

		offset	0x00000008			;branch when NMI occurs (set 0x40000008 when linking!)

;*--- NMI program ---*'ther],(sp)	;back up of d0,d1,a0,a1,mdr,lir,lar
		movhu	(NMICR),d0			;NMI detection
		and	0x0006,d0				;NMI?
		beq	endnmi
		mov	_int_WDT,a0
		calls	(a0)
endnmi		mov	0x0000ffff,d0		;clear ID
		movhu	d0,(NMICR)      	;clear NMI factor flag
		movhu	(NMICR),d0			;read as dummy
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti							;end of NMI
		
		offset	0x00000050			; flash option(set 0x40000050 when linking!)	

		dc	0x02					; flash option(0x02:enable set WD2MDSEL, except 0x02:disable set WD2MDSEL)

;*------------------------------------------------------*
;* interrupt handler ( C program ) 						*
;*	start interrupt routine 							*
;*------------------------------------------------------*
; level 0
irq_lev0						;branch here when interruption at level 0. set this address to (IVAR0).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

; level 1
irq_lev1						;branch here when interruption at level 1. set this address to (IVAR1).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

; level 2
irq_lev2						;branch here when interruption at level 2. set this address to (IVAR2).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

; level 3
irq_lev3						;branch here when interruption at level 3. set this address to (IVAR3).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

; level 4
irq_lev4						;branch here when interruption at level 4. set this address to (IVAR4).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

; level 5
irq_lev5						;branch here when interruption at level 5. set this address to (IVAR5).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

; level 6
irq_lev6						;branch here when interruption at level 6. set this address to (IVAR6).
		movm	[other],(sp)		;back up d0,d1,a0,a1,mdr,lir,lar
		udf15	d0,d0				;MDRQ Save	mdrq->d0	(udf15 = getx) 	;extention high speed operation
		mov	d0,(sp)					;	d0 -> stack                     ;extention high speed operation
		add	-4,sp					;	sp - 4 -> sp                    ;extention high speed operation

		movhu	(IAGR),d0			;read group number
		and	0x00fc,d0
		mov	_irq_vct_tbl,a0
		mov	(d0,a0),a0

		calls	(a0)				;call interrupt hundler of each group

		add	4,sp					;MDRQ 		sp + 4 -> sp            	;extention high speed operation
		mov	(sp),d0					;	stack -> d0                     ;extention high speed operation
		udf20	d0,d0				;	d0 -> mdrq	(udf20 = putx)  ;extention high speed operation
		movm	(sp),[other]		;repair d0,d1,a0,a1,mdr,lir,lar
		rti

;*--- PSW ---*
_enable_irq						;PSW access
_0enable_irq	funcinfo _enable_irq,0,[]
		or	0x0800,psw				;enable all interrupt
		nop
		nop
		ret

_disable_irq					;PSW access
_0disable_irq 	funcinfo _disable_irq,0,[]
		and	0xF7FF,psw				;disable interrupt
		nop
		nop
		ret

;set interrupt level to register D0
_set_imask						;PSW access
_0set_imask	funcinfo _set_imask,0,[]
		mov	psw,d1
		and	0xf8ff,d1
		and	0x0007,d0
		asl	8,d0					;bp10-bp8 = IM
		or	d0,d1
		mov	d1,psw
		nop
		nop
		ret

;*------------------------------------------------------*
;* reset start  		 								*
;*------------------------------------------------------*
;*--- program initialize routine ---*
_start

;*--- init control registers ---*
		mov	0x0005,d0
		movhu	d0,(CHWAIT)			; flash cycle access 

;*--- init CPU mode registers ---*
		mov	0xCA,d0
		movbu	d0,(PRTCPUM)		; CPUM register protecting free
		clr	d0
		movhu	d0,(CPUM)			; CPUM NORMAL mode
		nop
		nop
		mov	0xFF,d0
		movbu	d0,(PRTCPUM)		; CPUM register protecting set

;*--- set Interrupt Enable Flag ---*
		mov	IRQ_DISABLE,d0			;disable all interrupt
		mov	d0,psw
		nop
		nop


; set ivar
		mov	irq_lev0,d0			;set interrupt vector register of level 0
		movhu	d0,(IVAR0)
		mov	irq_lev1,d0			;set interrupt vector register of level 1
		movhu	d0,(IVAR1)
		mov	irq_lev2,d0			;set interrupt vector register of level 2
		movhu	d0,(IVAR2)
		mov	irq_lev3,d0			;set interrupt vector register of level 3
		movhu	d0,(IVAR3)
		mov	irq_lev4,d0			;set interrupt vector register of level 4
		movhu	d0,(IVAR4)
		mov	irq_lev5,d0			;set interrupt vector register of level 5
		movhu	d0,(IVAR5)
		mov	irq_lev6,d0			;set interrupt vector register of level 6
		movhu	d0,(IVAR6)



;*--- stack clear ---*
		mov	stack+STACK_SIZE, a0		;set stack pointer
		mov	a0, sp

		add	-4, sp

;*--- Watchdog setting ---*
;		mov	0xd9,d0
;		movbu	d0,(PRTWDT)			; Watch Dog timer control register protecting free
;		mov	0x00,d0					; stop WDT
;		or	0xa3,d0
;		movbu	d0,(WDCTR)			; Change at Watch Dog timer cycle
;		mov	0xff,d0
;		movbu	d0,(PRTWDT)			; Watch Dog timer control register protecting

		mov	0x77,d0
		movbu	d0,(PRTWDT2)		; Watch Dog Timer 2 control register protect free
		mov	0x00,d0					; stop WDT2
		movbu	d0,(WD2MDSEL)		
		mov	0xFF,d0
		movbu	d0,(PRTWDT2)		; Watch Dog Timer 2 control register protect

;*--- clock setting---*

		mov	0x3c,d0
		movbu	d0,(PRTCKCNT)		; PCNT register protecting free
		mov	0xe2,d0
		movbu	d0,(PRTCKGEN)		; CKCTR register protecting free

		mov	PLL_SEL,d0				;PLL multiply ratio is selected.
		movhu	d0,(PCNT)			;The setting is forwarded to the PCNT register.
		or	PLL_ON,d0				;PLL ON setting.
		movhu	d0,(PCNT)			;The setting is forwarded to the PCNT register.

		mov	WAIT_200US,d1			;The counter for the loop is set. 

cloop
		setlb

		add	-1,d1			; The counter for the loop is decreased. 
		nop					; correction for branch.
		lne					; It loops when the condition is unapproval.

		nop					; correction for branch.
		nop					; correction for branch.

		mov	IOCLK_SEL,d0		; IOCLK is selected.
		or	0x0083,d0			; bit7-6 is set to 10, bit1-0 is set to 11
		movhu	d0,(CKCTR)		; The setting is forwarded to the CKCTR register.

		movhu	(PCNT),d0
		or	PLL_OUT,d0			;PLL output is selected.
		movhu	d0,(PCNT)		;The setting is forwarded to the PCNT register.

		mov	0xFF,d0
		movbu	d0,(PRTCKCNT)	; PCNT register protecting 
		movbu	d0,(PRTCKGEN)	; CKCTR register protecting 

;*--- RAM clear ---*
;clear _BSS, _GBSS				;clear RAM of non-initialized area
		mov	RAM_SIZE, d1		;clear all RAM area
		cmp	0, d1
		ble	bc_skip				;skip if there are no BSS,GBSS.

		mov	0, a0				;address of top of RAM
		clr	d0					;clear register D0
bc_loop
		mov	d0, (a0)	 		;clear RAM
		inc4	a0				;address decrement(by 4 Byte)
		add	-4, d1				;update loop counter
		bgt	bc_loop				;jump to bc_loop if it is not end of RAM area
bc_skip

;*--- RAM init ---*
;	copy _ROMDATA, _GROMDATA -> _DATA, _GDATA	;
		mov	.MN.romdataend - .MN.romdata,d1	;set size of initialized data area
		cmp	0, d1
		ble	dc_skip				;skip if there are no initialized data

		mov	.MN.romdata, a0		;top address of initialized data area
		mov	.MN.data, a1		;top address of initialized data area (destination)
dc_loop
		mov	(a0), d0			;read initialized data
		inc4	a0				;address increment(by 4 Byte)
		mov	d0, (a1) 			;write initialized data
		inc4	a1				;address increment(by 4 Byte)
		add	-4, d1				;update loop counter
		bgt	dc_loop				;jump to bc_loop if it is not end of RAM area

dc_skip
rec_skip
		clr	d0					;clear register DO
		mov	d0, d1				;clear register D1
		mov	d0, d2				;clear register D2
		mov	d0, d3				;clear register D3
		mov	d0, a0				;clear register A0
		mov	d0, a1				;clear register A1
		mov	d0, a2				;clear register A2
		mov	d0, a3				;clear register A3

		call	_0main			;branch to main routine

		bra	rec_skip

		end
