;------------------------------------
;-  Generated Initialization File  --
;------------------------------------

$include (C8051F020.inc)

public  Init_Device

INIT SEGMENT CODE
    rseg INIT

; Peripheral specific initialization functions,
; Called from the Init_Device label
Reset_Sources_Init:
    mov  WDTCN,     #0DEh
    mov  WDTCN,     #0ADh
    ret

Timer_Init:
    mov  CKCON,     #008h
    mov  TCON,      #010h
    mov  TMOD,      #001h
    mov  T2CON,     #004h
    ret

UART_Init:
    mov  SCON0,     #010h
    ret

Port_IO_Init:
    ; P0.0  -  TX0 (UART0), Open-Drain, Digital
    ; P0.1  -  RX0 (UART0), Open-Drain, Digital
    ; P0.2  -  Unassigned,  Open-Drain, Digital
    ; P0.3  -  Unassigned,  Open-Drain, Digital
    ; P0.4  -  Unassigned,  Open-Drain, Digital
    ; P0.5  -  Unassigned,  Open-Drain, Digital
    ; P0.6  -  Unassigned,  Open-Drain, Digital
    ; P0.7  -  Unassigned,  Open-Drain, Digital

    ; P1.0  -  Unassigned,  Open-Drain, Digital
    ; P1.1  -  Unassigned,  Open-Drain, Digital
    ; P1.2  -  Unassigned,  Open-Drain, Digital
    ; P1.3  -  Unassigned,  Open-Drain, Digital
    ; P1.4  -  Unassigned,  Open-Drain, Digital
    ; P1.5  -  Unassigned,  Open-Drain, Digital
    ; P1.6  -  Unassigned,  Push-Pull,  Digital
    ; P1.7  -  Unassigned,  Open-Drain, Digital

    ; P2.0  -  Unassigned,  Open-Drain, Digital
    ; P2.1  -  Unassigned,  Open-Drain, Digital
    ; P2.2  -  Unassigned,  Open-Drain, Digital
    ; P2.3  -  Unassigned,  Open-Drain, Digital
    ; P2.4  -  Unassigned,  Open-Drain, Digital
    ; P2.5  -  Unassigned,  Open-Drain, Digital
    ; P2.6  -  Unassigned,  Open-Drain, Digital
    ; P2.7  -  Unassigned,  Open-Drain, Digital

    ; P3.0  -  Unassigned,  Open-Drain, Digital
    ; P3.1  -  Unassigned,  Open-Drain, Digital
    ; P3.2  -  Unassigned,  Open-Drain, Digital
    ; P3.3  -  Unassigned,  Open-Drain, Digital
    ; P3.4  -  Unassigned,  Open-Drain, Digital
    ; P3.5  -  Unassigned,  Open-Drain, Digital
    ; P3.6  -  Unassigned,  Open-Drain, Digital
    ; P3.7  -  Unassigned,  Open-Drain, Digital

    mov  P1MDOUT,   #040h
    mov  XBR0,      #004h
    mov  XBR2,      #040h
    ret

Oscillator_Init:
    mov  OSCXCN,    #067h
    mov  R0,        #030        ; Wait 1ms for initialization
Osc_Wait1:
    clr  A
    djnz ACC,       $
    djnz R0,        Osc_Wait1
Osc_Wait2:
    mov  A,         OSCXCN
    jnb  ACC.7,     Osc_Wait2
    mov  OSCICN,    #008h
    ret

Interrupts_Init:
    mov  IE,        #092h
    mov  EIE2,      #020h
    ret

; Initialization function for device,
; Call Init_Device from your main program
Init_Device:
    lcall Reset_Sources_Init
    lcall Timer_Init
    lcall UART_Init
    lcall Port_IO_Init
    lcall Oscillator_Init
    lcall Interrupts_Init
    ret

end
