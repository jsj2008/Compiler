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

; Declaration: a = 0
MOV SI, 100D
PUSH 0D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------

; Declaration: p_x = 6
PUSH 6D
MOV SI, 102D
POP [SI]

; Declaration: p_y = 9
PUSH 9D
MOV SI, 104D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------

; Declaration: p1_x = 0
PUSH 0D
MOV SI, 106D
POP [SI]

; Declaration: p1_y = 0
PUSH 0D
MOV SI, 108D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------

;FOR STATEMENT
;Initialisation =>
;ASSIGNMENT STATEMENT, Line no : 7
MOV SI, 100D	
PUSH 0
POP [SI]
L0: 		;Loop
MOV SI, 100D
MOV AX, [SI]
CMP AX, 2D
JGE L1 	;Jump to end of loop if expr not satisfied

;ADDV STATEMENT 
MOV SI, 102D 
MOV DI, 106D 
MOV AX, [DI]
ADD [SI], AX
MOV SI, 104D 
MOV DI, 108D 
MOV AX, [DI]
ADD [SI], AX
;ADDV DONE
MOV SI, 100D
INC word ptr [SI]
JMP L0 	;Jump back up!
L1:		;End of loop

; Declaration: b1_x = 0
MOV SI, 102D
PUSH [SI]
MOV SI, 110D
POP [SI]

; Declaration: b1_y = 0
MOV SI, 104D
PUSH [SI]
MOV SI, 112D
POP [SI]

; Declaration: b1_dir = 100
PUSH 100D
MOV SI, 114D
POP [SI]

; Declaration: b1_color = 20
PUSH 20D
MOV SI, 116D
POP [SI]

; Declaration: b1_active = 1
PUSH 1D
MOV SI, 118D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------


;FW :: MOVE AHEAD!
PUSHA
PUSH 6D
POP DI
MOV DX, 110
;SET GRAPHICS MODE
MOV AH, 00H
MOV AL, 3
INT 10H
L2:
MOV AH, 02H
MOV BH, 0
INT 10H
MOV AH, 09H
MOV AL, 32		;display character
MOV BH, 0
MOV BL, 01111011B
MOV CX, 1
INT 10H
;WAIT
MOV AH, 86H
MOV CX, 0FH
PUSH DX
MOV DX, 4240H
INT 15H
POP DX
DEC DI
INC DX
CMP DI, 0
JG L2
POPA



.EXIT
END
