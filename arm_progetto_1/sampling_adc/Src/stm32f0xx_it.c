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
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
#define size 100
extern int lenght_triggering;
extern int triggered;
extern int triggering;
extern int inizio;
extern int V_th;
extern int end;
extern int loops;
extern int i;//indice di values
extern int j;//indice di small_values
extern int ADC_CC; //adc conversion complete
extern uint32_t valore;
extern char msg[6];
extern int resto,m,m_1;
extern char * puntatore;
extern int lenght_da_trasmettere;
volatile extern int vrefint_data;
extern int values[size];
int sballato=1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc;
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

	uint32_t m;
	float somma;
	uint32_t loops_2;
	//controllo se ha finito di convertire
	if((ADC1->ISR & ADC_ISR_ADRDY)!=0){
		//resetto l'arddy
		ADC1->ISR|=ADC_ISR_ADRDY;
		ADC1->CR|=ADC_CR_ADSTART;
}
	if((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC){
		
		if((ADC1->CHSELR & ADC_CHSELR_CHSEL0)!=0){
			//leggo dal registro ADC_DR
			m=ADC1->DR;
			if(!sballato){
			//uso due indici, inizio e triggering, il primo indicante l'inizio del pretrigger
			//ed il secondo indicante il dato appena inserito nel trigger
			if(!triggered){
				//riempio il vettore
				values[triggering]=(uint32_t)m;
				//cambio i valori di inizio e triggering
				//triggering varia da 0 a lenght-1, ciclando
				if(triggering==lenght_triggering-1){
					triggering=0;
					//inizio invece si sposta alla seconda cella quando triggering è tornato all'inizio
					inizio=1;
				}
				else{
					triggering++;
					if(inizio!=0){
						inizio++;
					}
				}
				if(m>V_th){
					triggered=1;
					
					//devo far in modo che i valori che verranno convertiti da adesso vengano messi 
					//dopo il triggering se il buffer di trigger non e pieno ovvero se inizio==0
					//altrimenti verranno messi dopo lenght_triggering-1
					if(inizio==0){
						j=triggering;
					}
					else{
						j=lenght_triggering;
					}
				}
			}
			else{
				if(j<size){
					//aggiungo a small_values
					values[j]=(uint32_t)m;
					j++;
				}
				else{
					//ho finito di campionare
					ADC_CC=1;//adc conversion complete
					return;
				}
			//aspetto 10 ms
			}
		}
		else{
			sballato=0;
		}
			//ADC1->CR|=ADC_CR_ADSTART;
			
		}
		else{
			//leggo il dato per la calibrazione
			vrefint_data=ADC1->DR;
			ADC1->CHSELR&=~ADC_CHSELR_CHSEL17;
			ADC1->CHSELR|=ADC_CHSELR_CHSEL0;
			ADC1->CR|=ADC_CR_ADSTART;
		}
	}
	TIM1->CR1&=~TIM_CR1_CEN;
	TIM1->CNT=0;
	TIM1->CR1|=TIM_CR1_CEN;
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

  /* USER CODE END TIM7_IRQn 1 */
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
		
			//trasmetto valore
			i++;
			if(i<lenght_da_trasmettere){
				USART2->TDR=*(puntatore+i);
			}
			else{
				if(j==-2){
						j++;
						m=vrefint_data;
					m_1=1000;
				msg[0]=(int)(m/m_1)+'0'; //m prima cifra
				resto=(m)%m_1;//resto
				msg[1]=(int)(resto*10/m_1)+'0';//m seconda cifra
				resto=(resto*10)%m_1;//resto
				msg[2]=(int)(resto*10/m_1)+'0';//m terza cifra
				resto=(resto*10)%m_1;
				msg[3]=(int)(resto*10/m_1)+'0';//quarta
				puntatore=msg;
				USART2->TDR=*puntatore;
				i=0;
				}
				else if(j<size-1){
						j++;
						m=values[j];
					m_1=1000;
				msg[0]=(int)(m/m_1)+'0'; //m prima cifra
				resto=(m)%m_1;//resto
				msg[1]=(int)(resto*10/m_1)+'0';//m seconda cifra
				resto=(resto*10)%m_1;//resto
				msg[2]=(int)(resto*10/m_1)+'0';//m terza cifra
				resto=(resto*10)%m_1;
				msg[3]=(int)(resto*10/m_1)+'0';//quarta
				puntatore=msg;
				USART2->TDR=*puntatore;
				i=0;
				}
				else{
					if(!end){
						msg[0]='#';
						msg[1]=' ';
						msg[2]=' ';
						msg[3]=' ';
						puntatore=msg;
						USART2->TDR=*puntatore;
						i=0;
						end=1;
					}
					else{
						i=0;
						return;
					}
				}
				
				
			}
		
	}
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
