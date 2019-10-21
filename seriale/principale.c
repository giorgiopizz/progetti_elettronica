#include "c8051F020.h"

extern void Init_Device();
idata stack[16];

void uart() interrupt 4{
	//lavorare su RI0 e TI0
	//bisogna fare il clear dei flag via software
}
void main(){
	SP=(char)&stack;
	Init_Device();
	
}