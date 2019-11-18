//principale.c
#include "c8051F020.h"
#define n_loops 5
/*n_loops può essere decrementato per avere 
una percezione migliore della luminosità. Diminuendo i loops
che deve fare, aumenta la frequenza.*/


char idata stack1[16];
//extern char idata stack2;
//extern void START(void);
extern void Init_Device(void);
int luminosity = 0;
int loops_on;
int loops; //lo stato iniziale è 1
int led_status=0;
int lum_status=0;//lo stato luminosity indica le opzioni(0,25, 50, 75,100)


void interruzione_timer(void) interrupt 1{
	//fermo
	TR0=0;
	TF0=0;
	loops--;
	if (loops<=0){
			if(led_status){
				//accendo
				P1_6=1;
				led_status=0;
				loops=loops_on;
			}
			else{
	
				//spengo
				P1_6=0;
				led_status=1;
				loops=n_loops-loops_on;
				//faccio ripartire
			}
	}
	TR0=1;
}
void interruzione_pulsante(void) interrupt 19{
	TR0=0;
	TF0=0;
	//lum status  può essere 0, 1, 2,3,4
	if(lum_status<4){
		lum_status++;
	}
	else{
		lum_status=0;
	}
	luminosity=lum_status*25;
	loops_on=n_loops*luminosity/100;
	//resetto tutto
	loops=loops_on;
	led_status=0;
	P1_6=1;
	
	//reset del flag dell'interrupt esterna 7
	P3IF &= ~0x80;
	TR0=1;//faccio rpartire il timer
}
void main(){
	SP = (char) (&stack1);
	//START();
	Init_Device();
	loops_on=n_loops*luminosity/100;
	loops=loops_on;//qui posso fare quello che voglio
	//P3IF &= ~0x80;
	while(1);
}