.MODEL TINY
.386
.STACK 100
.DATA
MAX1 DB 32
ACT1 DB ?
INP1 DB 32 DUP(0)
.CODE
.STARTUP

; Global Variables
; Following is the code of the main function.
MOV AX, 0
MOV SI, 100D
MOV [SI], AX
MOV SI, 102D
MOV [SI], AX
MOV SI, 104D
MOV [SI], AX
MOV SI, 106D
MOV [SI], AX
MOV SI, 108D
MOV [SI], AX
MOV SI, 110D
MOV [SI], AX
MOV SI, 112D
MOV [SI], AX
MOV SI, 114D
MOV [SI], AX
MOV SI, 116D
MOV [SI], AX
MOV SI, 118D
MOV [SI], AX
; ------------------ END OF DECLARATION OPERATIONS -------------------

;ASSIGNMENT STATEMENT, Line no : 4
MOV SI, 122D
MOV AX, [SI]
MOV SI, 124D
ADD AX, [SI]
PUSH AX
;Right hand side pushed
MOV SI, 120D	
POP [SI]

;ASSIGNMENT STATEMENT, Line no : 5
PUSH 5D
;Right hand side pushed
MOV SI, 118D	
POP [SI]

;ASSIGNMENT STATEMENT, Line no : 6
PUSH 4D
PUSH 5D

;Splice encountered in expression, pushing values on stack.
MOV SI, 122D
MOV DI, 120D
L0:
PUSH [SI]
DEC SI
DEC SI
CMP SI, DI
JGE L0

;Right hand side pushed
MOV SI, 126D
MOV DI, 120D
L1:
POP [SI]
DEC SI
DEC SI
CMP SI, DI
JGE L1


.EXIT
END
