#include "c8051F020.h"
#define CR 13
#define LF 10

typedef unsigned char uchar;
extern void Init_Device();
idata stack[16];

//dico che non mi interessa la comunicazione multiprocessore
uchar key;

uchar idata msg_da_trasmettere[]={'C','i','a','o',CR,LF};
uchar lenght_da_trasmettere = 6;
uchar idata msg_errore[]={CR,LF,'E','r','r','o','r','e',CR,LF};
uchar lenght_errore=10;
uchar lenght_max = 10;

uchar i=0;
uchar * puntatore;
uchar idata msg_ricevuto[10];
uchar status=0;
uchar loaded=0;
/*la trasmissione inizia quando un byte viene scritto nel SBUF0
viene lanciato un interrupt quando è finita la trasmissione e viene settato
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
				i=0;
				status++;
				*puntatore=msg_ricevuto[0];
			}
			else{
				if(!loaded){
					loaded=1;
					SBUF0=key;
				}
			}
			break;
			
		case(2):
			//aggiungo il carattere a puntatore
			RI0=0;
			if(i<10){
				key=SBUF0;
				msg_ricevuto[i]=key;
				i++;
				if(key=='#'){
					//smetto di ricevere
					REN0=0;
					status=0;
					puntatore=msg_ricevuto;
					lenght_da_trasmettere=i-1;
					i=0;
					SBUF0=*puntatore;
				}
			}
			else{
				REN0=0;
				i=0;
				status=0;
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
		//resettare flag
		return;
	}
	else{
		//ho un problema
		return;
	}
}

/*trasmettere benvenuto da micro, e poi aspetta in modalità eco*/
void main(){
	SP=(char)&stack;
	Init_Device();
	//inizio con la trasmissione del messaggio di benvenuto
	puntatore = msg_da_trasmettere; 
	SBUF0=*puntatore;
	while(1);
}