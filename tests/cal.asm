;  get_data.asm - a data entry subroutine example
;
; George Mobus
;
;
; The program demonstrates the use of subroutines, including a two-deep nesting of one subroutine
; calling another subroutine.
;
		.orig		x3000
;
		JSR		GETDATA
		ST		R1,FIRST
		AND		R5,R5,#0
		JSR		GETDATA
		ST		R1,SECOND
STOP		HALT
FIRST		.BLKW		1
SECOND		.BLKW		1

;
; Subroutine to get a two digit number
;
GETDATA		ST		R7,GDREG7		; save return address
		ST		R6,GDREG6		; registers to be used need saving
		ST		R5,GDREG5
		ST		R2,GDREG2
;
		AND		R1,R1,#0
		AND		R2,R2,#0
		LEA		R0,PROMPT
		LD		R6,THIRTY
		PUTS	
		GETC
		OUT
		ADD		R1,R0,#0	; move R0 to R1
		ADD		R1,R1,R6	; subtract x30 from R1 to get digit
		LD		R2,TEN
		JSR		MULT		; multiply R1 by 10
		GETC
		OUT
		ADD		R0,R0,R6	; subtract x30 from next character
		ADD		R1,R0,R1	; add two registers to get decimal value
		LD		R0,CR		; output a carriage return
		OUT		
		LD		R0,LF
		OUT
		LD		R2,GDREG2
		LD		R5,GDREG5		; restore register values
		LD		R6,GDREG6
		LD		R7,GDREG7
		RET				; value returned in R1
GDREG7		.BLKW		1		; use to keep subroutine return address
GDREG6 		.BLKW		1		; save registers modified in this subroutine
GDREG5		.BLKW		1
GDREG2		.BLKW		1
;
TEN		.FILL		#10
THIRTY		.FILL		xFFD0		; -48 or neg x30
PROMPT		.STRINGZ	"Enter a two digit number: "
CR		.FILL		#13		; carriage return
LF		.FILL		#10		; line feed
;
;
; Subroutine to multiply R5 * R1
;
MULT		ST		R7,MUREG7		; save return address
		ST		R3,MUREG3		; registers to be used need saving
		ST		R5,MUREG5
		AND		R3,R3,#0	; R3 will be the result
AGAIN		ADD		R3,R3,R1
		ADD		R5,R5,#-1
		BRp		AGAIN
		AND		R1,R1,#0	; falls through when R5 == 0 or -1
		ADD		R1,R3,#0
		LD		R5,MUREG5
		LD		R3,MUREG3
		LD		R7,MUREG7	;restore return address
		RET				;answer in R1
MUREG7		.BLKW		1
MUREG3		.BLKW		1
MUREG5		.BLKW		1
		.END