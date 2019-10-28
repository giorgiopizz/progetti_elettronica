#include "c8051F020.h"
#define CR 13
#define LF 10
#define ML 10
#define n_loops 5
//ML � Max Lenght
/*il programma ha 3 sezioni, divise dallo switch in scelta. Nella prima il micro trasmette un messaggio
di benvenuto, nella seconda trasmetto i caratteri ricevuto, uno alla volta, nella terza memorizza
10 caratteri(al massimo) per poi trasmetterli. Per passare da una modalit� all'altra si usa il #*/
typedef unsigned char uchar;
extern void Init_Device();
idata stack[16];

//dico che non mi interessa la comunicazione multiprocessore
uchar key;

uchar idata msg_da_trasmettere[]={'C','i','a','o',CR,LF};
uchar lenght_da_trasmettere = 6;
uchar idata msg_errore[]={CR,LF,'E','r','r','o','r','e',CR,LF};
uchar lenght_errore=10;
//uchar lenght_max = 10;

uchar i=0;
uchar * puntatore;
uchar idata msg_ricevuto[ML+4];
uchar status=0;
uchar loaded=0;

//variabili led
/*
int luminosity = 0;
int loops_on;
int loops; //lo stato iniziale � 1
int led_status=0;
int lum_status=0;*/

/*la trasmissione inizia quando un byte viene scritto nel SBUF0
viene lanciato un interrupt quando � finita la trasmissione e viene settato
il flag TI0.
Per la ricezione bisogna settare REN0(SCON0.4) ad 1. Finisce quando riceve
il bit di stop, quest'ultimo a seconda che SM20 sia settato ad 1 o a 0 deve
essere 1 o 0*/

void scelta(void){
	//controllo lo status
	switch(status){
		case(0):
			//trasmetto il messaggio di benvenuto
			i++;
			if (i<lenght_da_trasmettere){
				SBUF0=*(puntatore+i);
			}
			else{
				//vado a ricevere
				status++;
				i=0;
				REN0=1;
			}
			break;
		case(1):
			//ricevo
			//leggo SBUF0
			key=SBUF0;
			RI0=0;
			if(key=='#'){
				i=2;
				status++;
				//*puntatore=msg_ricevuto[0];
			}
			else{
				if(!loaded){
					loaded=1;
					SBUF0=key;
				}
			}
			break;
			
		case(2):
			RI0=0;
			if(i<ML+3){
				key=SBUF0;
				//aggiungo il carattere letto al vettore
				msg_ricevuto[i]=key;
				i++;
				if(key=='#'){
					//smetto di ricevere
					REN0=0;
					status=0;
					puntatore=msg_ricevuto;
					msg_ricevuto[i-1]=CR;
					msg_ricevuto[i]=LF;
					lenght_da_trasmettere=i+1;
					i=0;
					//trasmetto il messaggio ricevuto
					SBUF0=*puntatore;
				}
			}
			else{
				REN0=0;
				i=0;
				status=0;
				//trasmetto il messaggio di errore
				puntatore=msg_errore;
				lenght_da_trasmettere=lenght_errore;
				SBUF0=*puntatore;
			}
			break;
	}
}

void UARTO() interrupt 4{

	if(TI0==1){
		//allora ho appena finito di trasmettere
		scelta();
		loaded=0;
		//resettare flag
		TI0=0;
		return;
	}
	else if(RI0==1){
		//allora ho appena finito di ricevere
		scelta();
		return;
	}
	else{
		//ho un problema
		return;
	}
}

//aggiungo le funzionalit� del led
/*
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
	//lum status  pu� essere 0, 1, 2,3,4
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
	
//	se volessi resettare il flag dell'interrupt esterna 7
//	dovrei scrivere
	P3IF &= ~0x80;
//	� necessario rimettere a 1 il flag della porta P3.7
//	anche se probabilmente schiacciando il bottone verr�
//	in automatico settato a 1 dopo
	//P3_7=0;
	//poi pu� rincominciare
	TR0=1;
}*/
/*trasmettere benvenuto da micro, e poi aspetta in modalit� eco*/
void main(){
	SP=(char)&stack;
	Init_Device();
	/*loops_on=n_loops*luminosity/100;
	loops=loops_on;*/
	//inizio con la trasmissione del messaggio di benvenuto
	puntatore = msg_da_trasmettere; 
	SBUF0=*puntatore;
	msg_ricevuto[0]=CR;
	msg_ricevuto[1]=LF;
	
	while(1);
}