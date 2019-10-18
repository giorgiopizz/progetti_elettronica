//principale.c
#include "c8051F020.h"
//#include <stdio.h>

char idata stack1[16];
extern char idata stack2;
int luminosity = 75;
int loops_on;
int loops; //lo stato iniziale è 1
int led_status=0;
extern void START(void);


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
	//setto luminoisty a 75
	luminosity=75;
	loops_on=luminosity*255/100;
	led_status=0;
	//poi può rincominciare
}
void main(){
	SP = (char) (&stack1);
	START();
	loops_on=(255*luminosity/100);
	loops=loops_on;
	//qui posso fare quello che voglio
 	while(1);
}