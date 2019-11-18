;$include (C8051f020.inc)

;extrn Init_Device()
;codice segment code
;stack segment idata
;vettore segment code
;start segment code
;	rseg stack
;		stack2: ds 
;cseg at 0x023A
;	jmp interruzione

;rseg vettore:
;	DS messaggio_benvenuto 10H
;		
;rseg codice
;creazione_vettore:
;mov R0, #messaggio_benvenuto ;adesso R0 punta a messaggio_ben
;mov @R0,#'C' ;scrivo nell'indirizzo a cui punta R0, 'C'
;mov @R0,#'i'
;mov @R0,#'a'
;mov @R0,#'o'
;mov @R0,#0

;ret

;interruzione:
;	jb R0, ricezione
;	jb T0, trasmissione


;trasmissione:
;	mov A,flag_fine_vettore
;	jnz fine_tr
;	inc R0
;	mov A,@R0
;	jz cambio_flag
;	mov SBUF0,@R0
;cambio_flag:
;	mov flag_fine_vettore,#1
;fine_tr: clr TI
;	reti

;ricezione:
;	mov R1,SBUF0
;	mov SBUF0,R1
;	
;	clr R0
;	reti

;rseg start
;start:
;	mov SP,
;	lcall Init_Device
;	call creazione_vettore
;	jmp $
;	end
	
NAME ASM_CODE

$INCLUDE (C8051F020.INC)

EXTRN 	CODE 	(Init_Device)
PUBLIC START,STACK2,N
CODICE SEGMENT CODE
VETTORE SEGMENT CODE
STACK SEGMENT IDATA
NEXT SEGMENT CODE
RSEG STACK
STACK2: DS 10H

RSEG VETTORE
VETT: DB	'Ciao',00H


RSEG NEXT
N:	MOVC A,@A+DPTR
	RET
RSEG CODICE
START:
	LCALL Init_Device
	MOV DPTR, #VETT
	RET
	
END
	