NAME Uso_del_led
	$include (C8051F020.inc)
	EXTRN CODE (Init_Device);recuperiamo questa funzione da configurazione_micro.asm
	
	PRINCIPALE SEGMENT CODE

	
	CSEG AT 0 
		JMP START 
	
; spegnere il watchdog 

;impostare la frequenza del clock esterno a 22.1184

;impostare porta del led

;impostazione di un timer

CSEG AT 0x000B
	jmp TIMER
	

RSEG PRINCIPALE



TIMER:
	DJNZ R0, NULLA
	JMP SCELTA

NULLA:
	RETI
SCELTA:
	;SE R1 � 1 ALLORA ACCENDO, SE � 0 SPENGO
	MOV A, R1
	JZ ACCENSIONE
	JNZ SPEGNIMENTO

ACCENSIONE:
	SETB P1.6
	MOV R0, #1000
	MOV R1,#1
	RETI


SPEGNIMENTO:
	CLR P1.6
	MOV R0, #1000
	MOV R1,#0
	RETI
	
START: LCALL Init_Device
MOV R0, #1000
MOV R1, #0

jmp $ 

END