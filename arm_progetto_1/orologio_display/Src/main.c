/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define LCD_ADDRESS (0X3E<<1) //indirizzo lcd, ruotato a sx per lasciare bit di r/w
#define LM76_ADDRESS (0X48<<1) //indirizzo termometro

int ora[]={2,0,1,9,1,1,2,3,1,9,2,8,0,0};
int hour,resto,day,month,year;
extern int wait,loops;
char msg[]={'.','.',':','.','.',':','.','.'};
char msg_2[]={'.','.','/','.','.','/','.','.','.','.'};
char * puntatore;
int change_big_date=0;
int bisestile=0;
int lenght_da_trasmettere;
//uint8_t initdata[]={0x38,0x39,0x14,0x74,0x54,0x6F,0x0F,0x01};
char initdata[]={0x00,0x38,0x00,0x39,0x00,0x14,0x00,0x74,0x00,0x54,0x00,0x6F,0x00,0x0F,0x00,0x01};
//int j=8;	
int NBYTES =(2<<16);
extern int j;
//anno, mese,giorno,ore,minuti, secondi

void incrementa(){
	if(ora[13]==9){
		//cifra meno significativa dei secondi
		ora[13]=0;
		//aumento ora[12]
		if(ora[12]==5){
			//cifra piu significativa dei secondi
			ora[12]=0;
			//aumento ora[11]
			if(ora[11]==9){
				//cifra meno significativa dei minuti
				ora[11]=0;
				//aumento ora[10]
				if(ora[10]==5){
					//cifra piu significativa dei minuti
					ora[10]=0;
					//aumento di un'ora
					hour=ora[9]+ora[8]*10;
					if(hour==23){
						ora[9]=0;
						ora[8]=0;
						//aumento di un giorno
						change_big_date=1;
						day=ora[7]+ora[6]*10;
						month=ora[5]+ora[4]*10;
						year=ora[0]*1000+ora[1]*100+ora[2]*10+ora[3];
						if((year%100==0 && year%400==0)||(year%100!=0 && year%4==0)){
							bisestile=1;
						}
						if((day==31 && (month==1|| month==3 || month==5 || month==7 || month==8 || month==10 || month==12))||
							(day==30 &&(month==4|| month==6 || month==9 || month==11))||(day==28 && month==2 && !bisestile)||
						(day==29 && month==2 && bisestile)){
							ora[6]=0;
							ora[7]=1;
							//aumento il il mese
							if(month==12){
								ora[4]=0;
								ora[5]=1;
								//aumento l'anno
								year++;
								//scompongo year su ora
								ora[0]=(int)year/1000;
								resto=year%1000;
								ora[1]=(int)(resto*10)/1000;
								resto=(resto*10)%1000;
								ora[2]=(int)(resto*10)/1000;
								resto=(resto*10)%1000;
								ora[3]=(int)(resto*10)/1000;
							}
							else{
								month++;
								ora[4]=(int)month/10;
								resto=month%10;
								ora[5]=(int)(resto*10)/10;
							}
						}
						else{
							day++;
							ora[6]=(int)day/10;
							resto=day%10;
							ora[7]=(int)(resto*10)/10;
						}
					}
					else{
						hour++;
						ora[8]=(int)hour/10;
						resto=hour%10;
						ora[9]=(int)(resto*10)/10;
					}
				}
				else{
					ora[10]++;
				}
			}
			else{
				ora[11]++;
			}
		}
		else{
			ora[12]++;
		}
	}
	else{
		ora[13]++;
	}
}

void wait_tim(){
	wait=1;
	TIM7->CNT=0;
	TIM7->SR=0;
	TIM7->CR1|=TIM_CR1_CEN;
	while(wait){}
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim7;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

	TIM7->SR=0;
	//TIM7->DIER|=TIM_DIER_UIE;
	ADC1->IER|=ADC_IER_ADRDYIE;
	
	
	//abilito e configuro l'adc
	//se fosse già abilitato lo disabilito per fare la calibrazione
	if ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		ADC1->CR |= ADC_CR_ADDIS;
	}
	//aspetto che non sia piu enable
	while ((ADC1->CR & ADC_CR_ADEN) != 0){}
	//faccio la calibrazione
	ADC1->CR|=ADC_CR_ADCAL;
	//ed aspetto che abbia finito
	while((ADC1->CR & ADC_CR_ADCAL)==ADC_CR_ADCAL){}
	//se adrdy fosse 1 la metto a 0
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)
	{
		ADC1->ISR |= ADC_ISR_ADRDY;
	}
	//abilito
	ADC1->CR|=ADC_CR_ADEN;
	
	
	loops=10;
	while(loops>0){
		TIM7->CNT=0;
		TIM7->SR=0;
		TIM7->CR1|=TIM_CR1_CEN;
		while(TIM7->SR==0){}
		loops--;
	}
	GPIOA->ODR |= GPIO_ODR_5;
	loops=10;
	while(loops>0){
		TIM7->CNT=0;
		TIM7->SR=0;
		TIM7->CR1|=TIM_CR1_CEN;
		while(TIM7->SR==0){}
		loops--;
	}
	

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
	
	I2C1->CR1|=I2C_CR1_RXIE;
	I2C1->CR1|=I2C_CR1_TXIE;
	I2C1->CR2|=I2C_CR2_AUTOEND;
	I2C1->CR1|=I2C_CR1_STOPIE;
	
	
	I2C1->CR2 |= NBYTES; //Setta il registro degli NBYTES al valore stabilito
	I2C1->CR2 |= LCD_ADDRESS; //Dà l'indirizzo dello schermo al microcontrollore
	I2C1->CR2 |= I2C_CR2_START; //Dà lo start-bit 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		/*loops=10;
		while(loops>0){
			TIM7->CNT=0;
			TIM7->SR=0;
			TIM7->CR1|=TIM_CR1_CEN;
			while(TIM7->SR==0){}
			loops--;
	}
		incrementa();*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel to be converted. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 8000;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
