NAME Uso_del_led
	$include (C8051F020.inc)
	EXTRN CODE (Init_Device);recuperiamo questa funzione da configurazione_micro.asm

	PRINCIPALE SEGMENT CODE


	CSEG AT 0
		JMP START


	;scopo di questo programma � eseguire delle istruzioni(jmp $) e gestire gli interrupt del timer. Dopo 255 interrupt il programma accende o spegne il led
	CSEG AT 0x000B ;dove punta l'interrupt del timer 0
	jmp TIMER


RSEG PRINCIPALE

TIMER:
    CLR TR0
    CLR TF0
	DJNZ R0, NULLA
	JMP SCELTA

NULLA:
    SETB TR0; faccio ripartire il micro
	RETI
	
SCELTA:
	;SE R1 � 1 ALLORA ACCENDO, SE � 0 SPENGO
	MOV A, R1
	JZ ACCENSIONE
	JNZ SPEGNIMENTO

ACCENSIONE:
	SETB P1.6
	MOV R0, #255
	MOV R1,#1
	SETB TR0; faccio ripartire il micro
	RETI

SPEGNIMENTO:
	CLR P1.6
	MOV R0, #255
	MOV R1,#0
	SETB TR0; faccio ripartire il micro
	RETI

START: LCALL Init_Device

MOV R0, #255
MOV R1, #0

jmp $

END
