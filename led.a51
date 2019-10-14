NAME Uso_del_led
	$include (C8051F020.inc)
	EXTRN CODE (Init_Device);recuperiamo questa funzione da configurazione_micro.asm
	
	PRINCIPALE SEGMENT CODE
	
	CSEG AT 0 
		JMP START 
	RSEG PRINCIPALE
; spegnere il watchdog 

;impostare la frequenza del clock esterno a 22.1184

;impostare porta del led

;impostazione di un timer


; funzione per aspettare un po' di tempo
TIMER:
;faccio trascorrere un po' di tempo
MOV R1, #10 ;voglio che venga eseguito due volte
FOR:
SETB TR0 ;faccio partire il cronometro
WHILE:
		JNB TF0, WHILE ; finchè è zero va a while altrimenti va avanti
CLR TR0 ; FERMO IL TIMER
CLR TF0 ;AZZERO IL FLAG DI OVERFLOW
SETB TR0 ; faccio ripartire il timer
DJNZ R1, FOR ;il loop è stato eseguito più di 3 volte?
CLR TR0; fermo il timer
CLR TF0 ;azzero il flag di overflow
RET
; FINE FUNZIONE

START: LCALL Init_Device
;ciclo
;parte accende il led torna su
MOV R0, #10
REPEAT:

SETB P1.6 ;accendo il led

LCALL TIMER

CLR P1.6 ;spengo il led

LCALL TIMER
DJNZ R0, REPEAT

jmp $ 
END