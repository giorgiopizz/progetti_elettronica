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

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	//#define vrefint_cal (0x1FFFF7BAU)
	//extern int j;
	//extern uint8_t initdata[16];
	#define LCD_ADDRESS (0X3E<<1) //indirizzo lcd, ruotato a sx per lasciare bit di r/w
	#define LM76_ADDRESS (0X48<<1)
	volatile int vec[3];
	uint32_t vrefint_data;
	int wait;
	volatile int loops=0;
	extern int i;
	extern int loaded;
	extern int lenght_da_trasmettere;
	extern char * puntatore;
	extern char msg[10];
	int led_on=0;
	extern void wait_tim();
	extern uint8_t initdata[16];
	extern int NBYTES;
	char temp[]={'T','e','m','p',':','.','.','.','.'};
	volatile uint16_t t;
	uint32_t t1, t2;
	int j=0;
	int perc[4];
	//int status=0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void scelta(){
	//controllo se ho appena finito la trasmissione o la ricezione
	//inserisco nel buffer di trasmissione il carattere da trasmettere
	i++;
	if(i<lenght_da_trasmettere){
		USART2->TDR=*(puntatore+i);
	}	
}
void led(){
	if(!led_on){
		GPIOB->ODR|= GPIO_ODR_0;
		GPIOB->ODR|= GPIO_ODR_1;
		GPIOB->ODR|= GPIO_ODR_2;
		GPIOB->ODR|= GPIO_ODR_3; 
		led_on=1;
	}
	else{
		GPIOB->ODR &= ~GPIO_ODR_0;
		GPIOB->ODR &= ~GPIO_ODR_1;
		GPIOB->ODR &= ~GPIO_ODR_2;
		GPIOB->ODR &= ~GPIO_ODR_3;
		led_on=0;
	}
}
/*void scelta(){
	switch(status){
		case(0):
			//devo trasmettere i caratteri di inizializzazione
			break;
		case(1):
			//leggo i dati dal termometro
			//gestione del termometro
			I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
			I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
			I2C1->CR2 |= LM76_ADDRESS; //Dico all'I2C di parlare con il termometro
			NBYTES = 2 << 16; //Mi aspetto 2 valori
			I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES
			I2C1->CR2 |= NBYTES; //Rimetto il registro NBYTES a '2'
			I2C1->CR2 |= I2C_CR2_RD_WRN; //Imposto la sola lettura
			I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
			break;
		case(2):
			//trasmetto i dati del termometro e del potenziale
			break;
	}
}*/
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim7;
extern UART_HandleTypeDef huart2;
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
  * @brief This function handles ADC and COMP interrupts (COMP interrupts through EXTI lines 21 and 22).
  */
void ADC1_COMP_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_COMP_IRQn 0 */
	//leggo il dato da due celle di memoria a 8 bit
	uint32_t valore;
	uint32_t m;
	uint32_t resto,m_1;
	valore=*(uint16_t*)0x1FFFF7BA;
	float div;
	//valore = (punt[0]<<8)+punt[1];
	
	
	//controllo se ha finito di convertire
	if((ADC1->ISR & ADC_ISR_ADRDY)!=0){
		//resetto l'arddy
		ADC1->ISR|=ADC_ISR_ADRDY;
		//faccio partire la conversione
		ADC1->CR|=ADC_CR_ADSTART;
		
}
	if((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC){
		
		if((ADC1->CHSELR & ADC_CHSELR_CHSEL0)!=0){
			//leggo dal registro ADC_DR
			m=ADC1->DR;
			//ricalcolo il valore per 1V
			m_1=(int)10*(4095*vrefint_data)/(3.3*(valore)); //è 1241.2, ricordarsi di dividere sempre per 10
			div=0.9834*m-0.0026;
			m=(int)div;
			vec[0]=(int)(m*10/m_1); //m prima cifra
			resto=(m*10)%m_1;//resto
			vec[1]=(int)(resto*10/m_1);//m seconda cifra
			resto=(resto*10)%m_1;//resto
			vec[2]=(int)(resto*10/m_1);//m terza cifra
		}
		else{
			//leggo il dato per la calibrazione
			vrefint_data=ADC1->DR;
		}
		
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
	uint32_t resto,m_1;
	int i2c_loops=0;
	float pot;
	if((I2C1->ISR & I2C_ISR_STOPF)!=0){
		i2c_loops=100;
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
	if(j==0){
		led();
	}
	I2C1->TXDR = initdata[j]; //Mando il vettore di inizializzazione un elemento per volta
	j++; 
}
else{
	switch(j){
		case 16:
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
		case 17:
			 if(I2C1->ISR & I2C_ISR_RXNE){ //Se ha ricevuto...
			 t1 = I2C1->RXDR; //Metto in t1 il primo dato letto
			 j++;
			 }
			 break;
			 case 18:
			 if(I2C1->ISR & I2C_ISR_RXNE){
			 t2= I2C1->RXDR; //Metto in t2 il secondo dato letto
			 j++;
			 }
			 break;
			 case 19:
			 t1= t1<<8; //Ruoto di 8 bit per far in modo che siano i bit più significativi
			 t= ((t1+t2)>>3); //Ruoto di 3 per eliminare i bit di stato
			 temp[5]=(int)t/160+'0';
			 resto=t%160;
			 temp[6]=(int)(resto*10)/160+'0';
			 resto=(resto*10)%160;
			 temp[8]=(int)(resto*10)/160+'0';
			 puntatore=temp;
			 lenght_da_trasmettere=9;
			 j++;
			 break; 

case 20:
			I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
			I2C1->CR2 &= ~I2C_CR2_RD_WRN;
			I2C1->CR2 &= ~0xFF; //Azzero l'indirizzo di CR2
			I2C1->CR2 |= LCD_ADDRESS; //Dico all'I2C di parlare con il termometro
			NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
			I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
			I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
			I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
			j++;
			break; 
		case 21:
			I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
			j++;
			break;

		case 22:
		 I2C1->TXDR = 0x80; //indirizzo della prima cella della prima riga
		 j++;
		 break;

		case 23:
		 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
		 j++;
		 break;

		case 24: //Scrive 'temp'
		 if(i<lenght_da_trasmettere){
				I2C1->TXDR = *(puntatore+i);
				i++;
		 }
		 else{
				j++;
			 led();
		}
		break;
		 
	case 25:
			led();
			i2c_loops=100;
			while(i2c_loops>0){
				TIM7->SR=0;
				TIM7->CNT=0;
				TIM7->CR1|=TIM_CR1_CEN;
				while(TIM7->SR==0){}
				i2c_loops--;
			}
			TIM7->CR1&=~TIM_CR1_CEN;
			//converto in percentuale vec
			/*
			pot=(float)vec[0]+(float)vec[1]/10+(float)vec[2]/100;
			perc[0]=(int)(pot*10)/33;
			resto=((int)(pot*10))%33;
			perc[1]=(int)(resto*10)/33;
			resto=(resto*10)%33;
			perc[2]=(int)(resto*10)/33;
			resto=(resto*10)%33;
			perc[3]=(int)(resto*10)/33;
			if(perc[0]==0){
				msg[4]=' ';
				if(perc[1]==0){
					msg[5]=' ';
				}
				else{
					msg[5]=perc[1]+'0';
				}
			}
			else{
				msg[4]=perc[0]+'0';
				msg[5]=perc[1]+'0';
			}
			msg[6]=perc[2]+'0';
			msg[8]=perc[3]+'0';*/
			msg[4]=vec[0]+'0';
			msg[6]=vec[1]+'0';
			msg[7]=vec[2]+'0';
			msg[8]=' ';
			lenght_da_trasmettere=10;
			puntatore= msg;
			i=0;
			I2C1->ICR |= I2C_ICR_STOPCF; //Pulisco la flag dello stop-bit
			NBYTES = ((lenght_da_trasmettere+3) <<16); //+3 perché stiamo dando anche la posizione in cui deve stampare
			I2C1->CR2 &= ~(0xFF<<16); //Pulisco il registro NBYTES 6
			I2C1->CR2 |= NBYTES; //Do il nuovo NBYTES
			I2C1->CR2 |= I2C_CR2_START; //Do il bit di start
			j++;
			break; 
		case 26:
			I2C1->TXDR = 0x80; //Dice che il byte successivo è un byte di istruzione (come indicato sulle slides)
			j++;
			break;

		case 27:
		 I2C1->TXDR = 0xC0; //indirizzo della prima cella della prima riga
		 j++;
		 break;

		case 28:
		 I2C1->TXDR = 0x40; //Dice che i prossimi bytes sono caratteri
		 j++;
		 break;

		case 29: //Scrive 'temp'
		 if(i<lenght_da_trasmettere){
			I2C1->TXDR = *(puntatore+i);
			i++;
		 }
		 else{
			 //cambio puntatore e lenght_da_trasmettere
			 led();
			 i2c_loops=100;
		while(i2c_loops>0){
			TIM7->SR=0;
			TIM7->CNT=0;
			TIM7->CR1|=TIM_CR1_CEN;
			while(TIM7->SR==0){}
			i2c_loops--;
		}
		
		TIM7->CR1&=~TIM_CR1_CEN;
		ADC1->CR|=ADC_CR_ADSTART;
		j=25;
		 }
		 break; 
	 

		}
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

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	if((USART2->ISR & USART_ISR_TC) == USART_ISR_TC){
		//resetto il flag
		USART2->ICR |= USART_ICR_TCCF;		/* Clear transfer complete flag */
		//allora ho appena finito di trasmettere
		scelta();
		loaded=0;
		return;
	}
	else if((USART2->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		//allora ho appena finito di ricevere
		scelta();
		return;
	}
	
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
