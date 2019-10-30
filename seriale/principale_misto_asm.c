/*#include "c8051F020.h"
#define CR 13
#define LF 10
#define ML 10
#define n_loops 5
//ML è Max Lenght
//il programma ha 3 sezioni, divise dallo switch in scelta. Nella prima il micro trasmette un messaggio
//di benvenuto, nella seconda trasmetto i caratteri ricevuto, uno alla volta, nella terza memorizza
//10 caratteri(al massimo) per poi trasmetterli. Per passare da una modalità all'altra si usa il #
typedef unsigned char uchar;
//extern void Init_Device();
extern void Start();
extern void N();
extern idata stack2;
//idata stack[16];

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




void scelta(void){
	//controllo lo status
	//trasmetto il messaggio di benvenuto
	i++;
	if (i<lenght_da_trasmettere){
		ACC=i;
		N();
		*puntatore =(uchar) ACC;
		SBUF0=*puntatore;
	}
	else{
		//vado a ricevere
		status++;
		i=0;
		REN0=1;
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


trasmettere benvenuto da micro, e poi aspetta in modalità eco
void main(){
	SP=(uchar)&stack2;
	//Init_Device();
	Start();
	
	loops_on=n_loops*luminosity/100;
	loops=loops_on;
	//inizio con la trasmissione del messaggio di benvenuto
	ACC=i;
	N();
	*puntatore =(uchar) ACC;
	//puntatore =datapointer; 
	SBUF0=*puntatore;
	while(1);
}*/