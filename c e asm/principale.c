//principale.c
#include "c8051F020.h"
#define n_loops 255
/*n_loops pu� essere decrementato per avere 
una percezione migliore della luminosit�. Diminuendo i loops
che deve fare, aumenta la frequenza.*/


//char idata stack1[16];
//extern char idata stack2;
//extern void START(void);
extern void Init_Device(void);
int luminosity = 25;
int loops_on;
int loops; //lo stato iniziale � 1
int led_status=0;
int lum_status=0;//lo stato luminosity indica le opzioni(25, 50, 75)


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
				loops=255-loops_on;
				//faccio ripartire
			}
	}
	TR0=1;
}
void interruzione_pulsante(void) interrupt 19{
	TR0=0;
	TF0=0;
	//lum status  pu� essere 0, 1, 2
	if(lum_status<2){
		lum_status++;
	}
	else{
		lum_status=0;
	}
	switch(lum_status){
		case 0:
			luminosity=25;
			break;
		case 1:
			luminosity=50;
			break;
		case 2:
			luminosity=75;
			break;
	}
	loops_on=n_loops*luminosity/100;
	//resetto tutto
	loops=loops_on;
	led_status=0;
	P1_6=1;
	
	/*se volessi resettare il flag dell'interrupt esterna 7
	dovrei scrivere*/
	//P3IF &= ~0x80;
	/*� necessario rimettere a 1 il flag della porta P3.7
	anche se probabilmente schiacciando il bottone verr�
	in automatico settato a 1 dopo*/
	P3_7=1;
	//poi pu� rincominciare
	TR0=1;
}
void main(){
	//SP = (char) (&stack1);
	//START();
	Init_Device();
	loops_on=n_loops*luminosity/100;
	loops=loops_on;//qui posso fare quello che voglio
	while(1);
}