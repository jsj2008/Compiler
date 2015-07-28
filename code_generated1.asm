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

; Declaration: a = 5
PUSH 5D
MOV SI, 100D
POP [SI]

; Declaration: d = 0
MOV SI, 102D
PUSH 0D
POP [SI]

; Declaration: b = 0
MOV SI, 104D
PUSH 0D
POP [SI]

; Declaration: c = 0
MOV SI, 106D
PUSH 0D
POP [SI]
; ------------------ END OF DECLARATION OPERATIONS -------------------

;ASSIGNMENT STATEMENT, Line no : 4
MOV BX, 23

;ASSIGNMENT STATEMENT, Line no : 5
;Added lhs to [  a  ]'s register

;ASSIGNMENT STATEMENT, Line no : 6
;Added lhs to [  a  ]'s register

;ASSIGNMENT STATEMENT, Line no : 7
MOV CX, BX
MOV AX, CX
ADD AX, 6D
PUSH AX
;Right hand side pushed
POP CX

;ASSIGNMENT STATEMENT, Line no : 8
;Added lhs to [  a  ]'s register

;ASSIGNMENT STATEMENT, Line no : 9
MOV DX, BX
INC DX

;ASSIGNMENT STATEMENT, Line no : 10
;Ignoring assignment (of kind a=a+0)

;ASSIGNMENT STATEMENT, Line no : 11
;Ignoring assignment (of kind a=a)

;ASSIGNMENT STATEMENT, Line no : 12
PUSH BX
PUSH DX
;Right hand side pushed
MOV SI, 106D
MOV [SI], CX 	;Flushing CX
MOV CX, BX
POP CX
POP DX

;ASSIGNMENT STATEMENT, Line no : 13
MOV AX, DX
ADD AX, CX
PUSH AX
POP AX
ADD AX, BX
PUSH AX
;Right hand side pushed
POP BX

;ASSIGNMENT STATEMENT, Line no : 14
PUSH CX
PUSH DX
PUSH BX
;Right hand side pushed
POP DX
POP CX
POP BX

MOV SI, 102D
MOV [SI], BX 	;Flushing BX
MOV SI, 104D
MOV [SI], CX 	;Flushing CX
MOV SI, 100D
MOV [SI], DX 	;Flushing DX

.EXIT
END
