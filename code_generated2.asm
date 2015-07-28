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
MOV SI, 100D
PUSH 0D
POP [SI]

MOV SI, 102D
PUSH 9D
POP [SI]

; Following is the code of the main function.

; Declaration: a = 0
MOV SI, 104D
PUSH 0D
POP [SI]

; Declaration: b = 0
MOV SI, 106D
PUSH 0D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------

; ----------------- USER INPUT STATEMENT ------------
; Pushing all registers onto stack before user input.
PUSHA

; User Input. Reading an integer using READI >> a.
LEA DX,MAX1
MOV AH,0AH
INT 21H
; Converting the input read as string to an integer.
LEA SI, INP1
MOV CL, ACT1
MOV AX, 0
MOV BX, 10
MOV DX, 0
L10:
ADD SI, CX
DEC SI
MOV DL, byte ptr [SI]
SUB DL, 30H
ADD AX, DX 		;In the end AX has the input integer.
MUL BX
DEC CX
JNZ L10

; Copy the value from AX register to the memory location of input variable.
MOV SI, 104D
MOV [SI], AX

; Restoring the values in registers as before the user input.
POPA
; ------------------ END OF READ OPERATION -------------------

;CONDITIONAL 
MOV SI, 104D
MOV BX, [SI]
CMP BX, 13D
JGE L2

; Declaration: l = 0
MOV SI, 108D
PUSH 0D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------

;ASSIGNMENT STATEMENT, Line no : 11
MOV AX, BX
ADD AX, 2D
PUSH AX
;Right hand side pushed
MOV SI, 108D	
POP [SI]
JMP L1
L2:

;CONDITIONAL 
CMP BX, 5D
JGE L3

;ASSIGNMENT STATEMENT, Line no : 15
INC CX

;ASSIGNMENT STATEMENT, Line no : 16
MOV AX, BX
ADD AX, 3D
PUSH AX
;Right hand side pushed
POP CX
JMP L1
L3:

;ASSIGNMENT STATEMENT, Line no : 19
MOV SI, 102D	
PUSH 2
POP [SI]
L1:

MOV SI, 100D
MOV [SI], CX 	;Flushing CX

.EXIT
END
