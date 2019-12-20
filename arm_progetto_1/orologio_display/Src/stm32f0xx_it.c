/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
volatile int loops,wait;
#define LCD_ADDRESS (0X3E<<1) //indirizzo lcd, ruotato a sx per lasciare bit di r/w
#define LM76_ADDRESS (0X48<<1) //indirizzo termometro


	extern int lenght_da_trasmettere;
	extern char * puntatore;
	extern char msg[8];
	extern char msg_2[10];
	extern uint8_t initdata[16];
	extern int NBYTES;
	int j=0;
	int i=0;
	int acceso=1;
	int menu=1;	//indica il livello del menu(0,1). Al livello 0 si ha la selezione della sezione
	//livello 1 indica che si è in una sezione
	int sezione=0; //sezione 0 è orologio, sezione 1 temperatura, sezione 2 cronometro ...
	int scelta_adc,scelta_precedente;
	int loaded=0,trasmesso=0;
	extern int ora[14];
	extern void incrementa();
	extern int change_big_date;
	int i2c_loops=0;
	int vec[3];
	char menu_1[]={'O','r','o','l','o','g','i','o'}; //lenght 8
	char menu_2[]={'T','e','m','p','e','r','a','t','u','r','a'}; //lenght 11
	char menu_3[]={'C','r','o','n','o','m','e','t','r','o'}; //lenght 10
	char menu_4[]={'I','m','p','o','s','t','a','z','i','o','n','i'}; //lenght 12
	char temp[]={'T','e','m','p',':','.','.','.','.'};
	char reset_display[]={0x00,0x01,0x00,0x02};
	volatile uint16_t t;
	uint32_t t1, t2;
	int char_da_trasmettere=0;
	int numero_adc,numero_precedente, status;
	char num[]={'0','1','2','3','4','5','6','7','8','9'};
	//status indica se adc è in scelta o numero
	/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void scelta(){
	uint32_t resto,m_1;
	if(menu==0){
		if(!trasmesso){
			i2c_loops=10;
				while(i2c_loops>0){
				TIM7->SR=0;
				TIM7->CNT=0;
				TIM7->CR1|=TIM_CR1_CEN;
				while(TIM7->SR==0){}
				i2c_loops--;
				}
				TIM7->CR1&=~TIM_CR1_CEN;
				
				switch(j){
					case(16):
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = (2 <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=2;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					case(17):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(18):
						I2C1->TXDR=0x01;
						j++;
						char_da_trasmettere--;
						break;
					case(19):
						if((I2C1->ISR & I2C_ISR_STOPF)!=0){
						char_da_trasmettere=2;
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
						I2C1->CR2 |= I2C_CR2_START; //Faccio partire lo START
						j++;}
						break;
					case(20):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(21):
						I2C1->TXDR=0x02;
						j++;
					char_da_trasmettere--;
						break;
				
				case 22:
					I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=lenght_da_trasmettere+3;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					break;
				case 23:
					I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
					j++;
					char_da_trasmettere--;
					break;
				case 24:
				 I2C1->TXDR = 0x80; //indirizzo della prima cella della prima riga
				 j++;
					char_da_trasmettere--;
				 break;
				case 25:
				 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
				 j++;
				char_da_trasmettere--;
				 break;

				case 26: //Scrive 'temp'
				 if(i<lenght_da_trasmettere){
						I2C1->TXDR = *(puntatore+i);
						i++;
					 char_da_trasmettere--;
				 }
				 else{
						j++;
					 trasmesso=1;
				 }
				 break;
			 }
		}
		else if(trasmesso){
			if(loaded==1){
				if(scelta_adc!=scelta_precedente){
					switch(scelta_adc){
						case(0):
							puntatore=menu_1;
							lenght_da_trasmettere=8;
							break;
						case(1):
							puntatore=menu_2;
							lenght_da_trasmettere=11;
							break;
						case(2):
							puntatore=menu_3;
							lenght_da_trasmettere=10;
							break;
						case(3):
							puntatore=menu_4;
							lenght_da_trasmettere=12;
							break;
					}
					i=0;
					j=16;
					
					trasmesso=0;
					scelta_precedente=scelta_adc;
				}
				loaded=0;
			}
			else if(loaded==0){
				/*i2c_loops=3000;
				while(i2c_loops>0){
				TIM7->SR=0;
				TIM7->CNT=0;
				TIM7->CR1|=TIM_CR1_CEN;
				while(TIM7->SR==0){}
				i2c_loops--;
				}
				TIM7->CR1&=~TIM_CR1_CEN;*/
				I2C1->CR1&=~I2C_CR1_RXIE;
				I2C1->CR1&=~I2C_CR1_TXIE;
				I2C1->CR2&=~I2C_CR2_AUTOEND;
				I2C1->CR1&=~I2C_CR1_STOPIE;
				ADC1->IER|=ADC_IER_EOCIE;
				ADC1->CR|=ADC_CR_ADSTART;
				loaded=1;
			}
		}
	}
	else if(menu==1 && sezione==0){
		//orologio
			switch(j){
				case(16):
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = (2 <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=2;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					case(17):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(18):
						I2C1->TXDR=0x01;
						j++;
						char_da_trasmettere--;
						break;
					case(19):
						if((I2C1->ISR & I2C_ISR_STOPF)!=0){
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
						I2C1->CR2 |= I2C_CR2_START; //Faccio partire lo START
						char_da_trasmettere=2;
						j++;}
						break;
					case(20):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(21):
						I2C1->TXDR=0x02;
						j++;
						char_da_trasmettere--;
						break;
				case 22:
					I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=lenght_da_trasmettere+3;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					break; 
				case 23:
					I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
					j++;
					char_da_trasmettere--;
					break;

				case 24:
				 I2C1->TXDR = 0x80; //indirizzo della prima cella della prima riga
				 j++;
					char_da_trasmettere--;
				 break;

				case 25:
					/*
					i2c_loops=10;
						while(i2c_loops>0){
						TIM7->SR=0;
						TIM7->CNT=0;
						TIM7->CR1|=TIM_CR1_CEN;
						while(TIM7->SR==0){}
						i2c_loops--;
						}
						TIM7->CR1&=~TIM_CR1_CEN;*/
				 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
						char_da_trasmettere--;
				 j++;
				 break;

				case 26: //Scrive 'temp'
				 if(i<lenght_da_trasmettere){
						I2C1->TXDR = *(puntatore+i);
						i++;
					 char_da_trasmettere--;
				 }
				 else{
						j++;
						i=0;
						puntatore=msg;
						lenght_da_trasmettere=8;
				 }
				 break;
				 
				case 27:
					I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=lenght_da_trasmettere+3;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					break; 
				case 28:
					I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
					j++;
					char_da_trasmettere--;
					break;

				case 29:
				 I2C1->TXDR = 0xC0; //indirizzo della prima cella della seconda riga
				 j++;
					char_da_trasmettere--;
				 break;

				case 30:
				 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
				 j++;
				char_da_trasmettere--;
				 break;

				case 31: //Scrive 'temp'
				 if(i<lenght_da_trasmettere){
						I2C1->TXDR = *(puntatore+i);
						i++;
					 char_da_trasmettere--;
				 }
				 else{
						j++;
						//aspetto un secondo ed incremento
						i2c_loops=980;
						while(i2c_loops>0){
						TIM7->SR=0;
						TIM7->CNT=0;
						TIM7->CR1|=TIM_CR1_CEN;
						while(TIM7->SR==0){}
						i2c_loops--;
						}
						TIM7->CR1&=~TIM_CR1_CEN;
						incrementa();
						if(change_big_date){
							msg_2[0]=ora[6]+'0';
							msg_2[1]=ora[7]+'0';
							msg_2[3]=ora[4]+'0';
							msg_2[4]=ora[5]+'0';
							msg_2[6]=ora[0]+'0';
							msg_2[7]=ora[1]+'0';
							msg_2[8]=ora[2]+'0';
							msg_2[9]=ora[3]+'0';
							j=22;
							puntatore=msg_2;
							lenght_da_trasmettere=10;
							change_big_date=0;
						}
						else{
							puntatore=msg;
							lenght_da_trasmettere=8;
							j=27;
						}
							
						msg[0]=ora[8]+'0';
						msg[1]=ora[9]+'0';
						msg[3]=ora[10]+'0';
						msg[4]=ora[11]+'0';
						msg[6]=ora[12]+'0';
						msg[7]=ora[13]+'0';
						
						i=0;
				 }
				break;
			}
	}
	else if(menu==1 && sezione==1){
		//temperatura
		switch(j){
			case(16):
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = (2 <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=2;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					case(17):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(18):
						I2C1->TXDR=0x01;
						j++;
						char_da_trasmettere--;
						break;
					case(19):
						if((I2C1->ISR & I2C_ISR_STOPF)!=0){
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
						char_da_trasmettere=2;
						I2C1->CR2 |= I2C_CR2_START; //Faccio partire lo START
						j++;}
						break;
					case(20):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(21):
						I2C1->TXDR=0x02;
						j++;
						char_da_trasmettere--;
						break;
		case 22:
			 I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
			 I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
			 I2C1->CR2 |= LM76_ADDRESS; //Dico all'I2C di parlare con il termometro
			 NBYTES = 2 << 16; //Mi aspetto 2 valori
			 I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES
			 I2C1->CR2 |= NBYTES; //Rimetto il registro NBYTES a '2'
			 I2C1->CR2 |= I2C_CR2_RD_WRN; //Imposto la sola lettura
			 I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
			 j++;
			 break;
		case 23:
			 if(I2C1->ISR & I2C_ISR_RXNE){ //Se ha ricevuto...
			 t1 = I2C1->RXDR; //Metto in t1 il primo dato letto
			 j++;
			 }
			 break;
			 case 24:
			 if(I2C1->ISR & I2C_ISR_RXNE){
			 t2= I2C1->RXDR; //Metto in t2 il secondo dato letto
			 j++;
			 }
			 break;
			 case 25:
			 t1= t1<<8; //Ruoto di 8 bit per far in modo che siano i bit più significativi
			 t= ((t1+t2)>>3); //Ruoto di 3 per eliminare i bit di stato
			 temp[5]=(int)t/160+'0';
			 resto=t%160;
			 temp[6]=(int)(resto*10)/160+'0';
			 resto=(resto*10)%160;
			 temp[8]=(int)(resto*10)/160+'0';
			 puntatore=temp;
			 lenght_da_trasmettere=9;
			 i=0;
			 j++;
			 break; 

		case 26:
			I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
			I2C1->CR2 &= ~I2C_CR2_RD_WRN;
			I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
			I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
			NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
			char_da_trasmettere=lenght_da_trasmettere+3;
			I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
			I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
			I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
			j++;
			break; 
	
		case 27:
			I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
			j++;
			char_da_trasmettere--;
			break;

		case 28:
		 I2C1->TXDR = 0x80; //indirizzo della prima cella della prima riga
		 j++;
		char_da_trasmettere--;
		 break;

		case 29:
		 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
		 j++;
			char_da_trasmettere--;
		 break;

		case 30: //Scrive 'temp'
		 if(i<lenght_da_trasmettere){
				I2C1->TXDR = *(puntatore+i);
				i++;
				char_da_trasmettere--;
		 }
		 else{
				j++;
			}
		break;
	}
	}
	else if(menu==1 && sezione==3){
		if(!trasmesso){
			i2c_loops=10;
				while(i2c_loops>0){
				TIM7->SR=0;
				TIM7->CNT=0;
				TIM7->CR1|=TIM_CR1_CEN;
				while(TIM7->SR==0){}
				i2c_loops--;
				}
				TIM7->CR1&=~TIM_CR1_CEN;
				
				switch(j){
					case(16):
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = (2 <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=2;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					case(17):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(18):
						I2C1->TXDR=0x01;
						j++;
						char_da_trasmettere--;
						break;
					case(19):
						if((I2C1->ISR & I2C_ISR_STOPF)!=0){
						char_da_trasmettere=2;
						I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
						I2C1->CR2 |= I2C_CR2_START; //Faccio partire lo START
						j++;}
						break;
					case(20):
						I2C1->TXDR=0x00;
						j++;
						char_da_trasmettere--;
						break;
					case(21):
						I2C1->TXDR=0x02;
						j++;
					char_da_trasmettere--;
						break;
				
				case 22:
					I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
					I2C1->CR2 &= ~I2C_CR2_RD_WRN;
					I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
					I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
					NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
					char_da_trasmettere=lenght_da_trasmettere+3;
					I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
					I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
					I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
					j++;
					break;
				case 23:
					I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
					j++;
					char_da_trasmettere--;
					break;
				case 24:
				 I2C1->TXDR = 0x80; //indirizzo della prima cella della prima riga
				 j++;
					char_da_trasmettere--;
				 break;
				case 25:
				 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
				 j++;
				char_da_trasmettere--;
				 break;

				case 26: //Scrive 'temp'
				 if(i<lenght_da_trasmettere){
						I2C1->TXDR = *(puntatore+i);
						i++;
					 char_da_trasmettere--;
				 }
				 else{
						j++;
					 trasmesso=1;
				 }
				 break;
			 }
		}
		else if(trasmesso){
			if(loaded==1){
				if(numero_adc!=numero_precedente){
					switch(numero_adc){
						case(0):
							puntatore=num+0;
							lenght_da_trasmettere=1;
							break;
						case(1):
							puntatore=num+1;
							lenght_da_trasmettere=1;
							break;
						case(2):
							puntatore=num+2;
							lenght_da_trasmettere=1;
							break;
						case(3):
							puntatore=num+3;
							lenght_da_trasmettere=1;
							break;
							case(4):
							puntatore=num+4;
							lenght_da_trasmettere=1;
							break;
								case(5):
							puntatore=num+5;
							lenght_da_trasmettere=1;
							break;
									case(6):
							puntatore=num+6;
							lenght_da_trasmettere=1;
							break;
						case(7):
							puntatore=num+7;
							lenght_da_trasmettere=1;
							break;
							case(8):
							puntatore=num+8;
							lenght_da_trasmettere=1;
							break;
								case(9):
							puntatore=num+9;
							lenght_da_trasmettere=1;
							break;
					}
					i=0;
					j=16;
					
					trasmesso=0;
					numero_precedente=numero_adc;
				}
				loaded=0;
			}
			else if(loaded==0){
				I2C1->CR1&=~I2C_CR1_RXIE;
				I2C1->CR1&=~I2C_CR1_TXIE;
				I2C1->CR2&=~I2C_CR2_AUTOEND;
				I2C1->CR1&=~I2C_CR1_STOPIE;
				ADC1->IER|=ADC_IER_EOCIE;
				ADC1->CR|=ADC_CR_ADSTART;
				loaded=1;
			}
		}
	}
	else if(sezione==20){
		//devo trasmettere qualcosa finchè 
		if(char_da_trasmettere>0){
			I2C1->TXDR=0xC0;
			char_da_trasmettere--;
		}
		else{
			EXTI4_15_IRQHandler();
		}
	}
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim7;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */
	if(char_da_trasmettere!=0){
		sezione=20;
	}
	else{
		switch(menu){
			case(1):
				//faccio il display dei menu
				menu=0;
				loaded=0;
				trasmesso=0;
				j=16;
				scelta_precedente=0;
				puntatore=menu_1;
				lenght_da_trasmettere=8;
				i=0;
				i2c_loops=0;
				status=0;
				break;
			case(0):
					menu=1;
				sezione=scelta_adc;
				i2c_loops=0;
				switch(sezione){
					case(0):
						msg_2[0]=ora[6]+'0';
						msg_2[1]=ora[7]+'0';
						msg_2[3]=ora[4]+'0';
						msg_2[4]=ora[5]+'0';
						msg_2[6]=ora[0]+'0';
						msg_2[7]=ora[1]+'0';
						msg_2[8]=ora[2]+'0';
						msg_2[9]=ora[3]+'0';
						puntatore=msg_2;
						lenght_da_trasmettere=10;
						j=16;
						i=0;
						break;
					case(1):
						i=0;
						j=16;
					case(3):
						loaded=0;
						trasmesso=0;
						j=16;
						numero_precedente=0;
						puntatore=num;
						lenght_da_trasmettere=1;
						i=0;
						i2c_loops=0;
						status=1;
				}
				break;
		}
	}
	/*if(acceso){
		//spengo
		I2C1->CR1&=~I2C_CR1_RXIE;
		I2C1->CR1&=~I2C_CR1_TXIE;
		I2C1->CR2&=~I2C_CR2_AUTOEND;
		I2C1->CR1&=~I2C_CR1_STOPIE;
		acceso=0;
	}
	else{
		I2C1->CR1|=I2C_CR1_RXIE;
		I2C1->CR1|=I2C_CR1_TXIE;
		I2C1->CR2|=I2C_CR2_AUTOEND;
		I2C1->CR1|=I2C_CR1_STOPIE;
		acceso=1;
	}*/
  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles ADC and COMP interrupts (COMP interrupts through EXTI lines 21 and 22).
  */
void ADC1_COMP_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_COMP_IRQn 0 */
	uint32_t m;
	uint32_t resto,m_1;
	//valore = (punt[0]<<8)+punt[1];
	
	
	//controllo se ha finito di convertire
	if((ADC1->ISR & ADC_ISR_ADRDY)!=0){
		//resetto l'arddy
		ADC1->ISR|=ADC_ISR_ADRDY;
		//faccio partire la conversione
		ADC1->CR|=ADC_CR_ADSTART;
		
}
	if((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC){
		
		//leggo dal registro ADC_DR
		m=ADC1->DR;
		if(status==0){
			m_1=(int)(4095)/(3);
			scelta_adc=(int)m/m_1;
		}
		else if(status==1){
			m_1=(int)(4095)/(9);
			numero_adc=(int)m/m_1;
		}
		I2C1->CR1|=I2C_CR1_RXIE;
		I2C1->CR1|=I2C_CR1_TXIE;
		I2C1->CR2|=I2C_CR2_AUTOEND;
		I2C1->CR1|=I2C_CR1_STOPIE;
	}
  /* USER CODE END ADC1_COMP_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc);
  /* USER CODE BEGIN ADC1_COMP_IRQn 1 */

  /* USER CODE END ADC1_COMP_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */
	if(loops>0){
		loops-=1;
		TIM7->CR1|=TIM_CR1_CEN;
	}
	else{
		wait=0;
		TIM7->CR1&=~TIM_CR1_CEN;
	}
  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles I2C1 event global interrupt / I2C1 wake-up interrupt through EXTI line 23.
  */
void I2C1_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_IRQn 0 */
	
	if((I2C1->ISR & I2C_ISR_STOPF)!=0){
		i2c_loops=10;
		while(i2c_loops>0){
			TIM7->SR=0;
			TIM7->CNT=0;
			TIM7->CR1|=TIM_CR1_CEN;
			while(TIM7->SR==0){}
			i2c_loops--;
		}
		TIM7->CR1&=~TIM_CR1_CEN;
		if(j<16){
			I2C1->CR2 |= I2C_CR2_START; //Faccio partire lo START
			I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
		}
	}
	if (j<16){
		I2C1->TXDR = initdata[j]; //Mando il vettore di inizializzazione un elemento per volta
		j++; 
	}
	else{
		scelta();
	}
/* USER CODE END I2C1_IRQn 0 */
  if (hi2c1.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR)) {
    HAL_I2C_ER_IRQHandler(&hi2c1);
  } else {
    HAL_I2C_EV_IRQHandler(&hi2c1);
  }
  /* USER CODE BEGIN I2C1_IRQn 1 */

  /* USER CODE END I2C1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
