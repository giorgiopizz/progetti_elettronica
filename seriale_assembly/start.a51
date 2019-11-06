$include (C8051f020.inc)

extrn Init_Device()
codice segment code


cseg at 0x023A
	jmp interruzione

rseg vettore:
	DS messaggio_benvenuto 10H
rseg codice
creazione_vettore:
;mov R0, #messaggio_benvenuto
mov R0,#'C'
ret

interruzione:
	jb R0, ricezione
	jb T0, trasmissione

ricezione:
	clr R0
	reti
trasmissione:
	clr T0
	reti
	
jmp $
end
	
	