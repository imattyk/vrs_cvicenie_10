/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void proccesDmaData(uint8_t* sign, uint8_t len, uint8_t used_bytes);
void checkCommand(uint8_t total_len);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t count = 0;
	//pouzil som algoritmus ktory som vymyslel uz na cviceni 7
	uint8_t tx_data[1000];
	uint8_t intro[] = "Buffer capacity 256 bytes, occupied: ";
	uint8_t middle[] = " bytes, load [in %]: ";
	uint8_t percent_char[] = "% ";
	double used_percent;
	int used_memory = 0;
	char used_memory_char[3] = "0";
	char used_percent_char[100] = "0";


	uint8_t total_len = 0; //number
	uint8_t first_symbol = 0; //boolean
	uint8_t last_symbol = 0; //boolean
	uint8_t first_symbol_index = 0; //number
	uint8_t first_symbol_cycle = 0; //boolean
	uint8_t rx_data[35];

	// cvicenie 10 premenne
	int is_auto = 0;
	int PWM_man_value = 0;
	uint8_t pwm[10] = "";
	uint8_t value_of_pwm_$[10] = "";
	uint8_t value_of_pwm[10] = "";
	uint8_t D_value = 0;
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

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  USART2_RegisterCallback(proccesDmaData);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableIT_UPDATE(TIM2);
  LL_TIM_EnableCounter(TIM2);
  LL_TIM_EnableIT_UPDATE(TIM3);
  LL_TIM_EnableCounter(TIM3);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_Init1msTick(8000000);
  LL_SetSystemCoreClock(8000000);
}

/* USER CODE BEGIN 4 */
void proccesDmaData(uint8_t* sign, uint8_t len, uint8_t used_bytes)
{
	/* Process received data */

	//pouzil som algoritmus ktory som vymyslel uz na cviceni 7
	//algoritmus funguje tak, ze mozeme command rozdelit aj medzi stringy poslane
	//cize to funguje aj cez putty ak nemame zapnute posielanie stringov ale pravdaze berie to aj z jedneho stringu
	//malo by to fungovat pre vsetky pripady

	//toto sa deje zakazdym ked sme este nedostali znak '#'
	//program skontroluje teda prijate data ci sa tam nachadza
	//ak nie tak sa nic nestane a program nespravi nic
	if(first_symbol == 0){
		for(uint8_t i = 0; i < len; i++){
				//ak najde prvy znak flipne boolean, zapamata si jeho index a zapisuje dalsie data uz do rx_data pola
		    	if(*(sign+i) == '#'){
		    		first_symbol = 1;
		    		first_symbol_index = i;
		    		first_symbol_cycle = 1;
		    	}
		    	//tuto zacne zapisovat data do rx_data ak nasiel prvy znak
		    	if(first_symbol == 1){
		    		//ak by som dovrsil 35 prijatych znakov, prestane zapisovat a posle data na vyhodnotenie
		    		if(i-first_symbol_index == 35){
		    			last_symbol = 1;
		    			total_len = 35;
		    			break;
		    		}
		    		rx_data[i-first_symbol_index] = *(sign+i);
		    		//ak najde aj koncovy znak, flipne aj boolean koncoveho znaku a breakne for cyklus
		    		if(*(sign+i) == '$'){
		    			last_symbol = 1;
		    			total_len = i - first_symbol_index;
		    			break;
		    		}
		    		total_len = i - first_symbol_index;
		    	}
		}
	}

	//ak program nasiel prvy znak ale nenasiel v tom istom stringu aj koncovi stane sa toto
	if(first_symbol == 1 && first_symbol_cycle == 0){

	    for(uint8_t i = 0; i < len; i++)
	    {
	    	//ak by som dovrsil 35 prijatych znakov, prestane zapisovat a posle data na vyhodnotenie
	    	if(total_len + i == 35){
	    		total_len = 35;
	    		last_symbol = 1;
	    		break;
	    	}
	    	//klasika, len pozeram ci nemam koncovy znak a zapisujem popri tom data do rx_data
	    	rx_data[i+total_len]=*(sign+i);
	    	if(*(sign+i) == '$'){
	    		last_symbol = 1;
	    		total_len += i;
	    		break;
	    	}
	    }
	    total_len += len;

	}

	//toto je len toggle na to aby sme vedeli ci sme v cykle spracovania stringu v ktorom sa nasiel prvy znak
	//ak by pouzivatel poslal koncovy znak v dalsom stringu
	if(first_symbol_cycle == 1){
		first_symbol_cycle = 0;
	}

	//vyhodnotenie malych a velkych pismen ked mame flag last_symbol ako true, resetnu sa rx_data a vsetky pomocne premenne
	if(last_symbol == 1){
		checkCommand(total_len);
		for(uint8_t i = 0;i<35;i++){
			rx_data[i] = 0;
		}
		total_len = 0;
		last_symbol = 0;
		first_symbol = 0;
	}

	//ulozime si kolko nam zostava pamata v beffery, prepocitame na percenta a ulozime do arrayov charov.
	used_memory = (int)used_bytes;
	used_percent = (((double)used_memory)/256)*100;
	sprintf(used_memory_char, "%d", used_memory);
	sprintf(used_percent_char, "%d", (int)used_percent);

}

//v tejto funkcii uz len checkujem co mi prislo, aky command
//tato funkica sa zavola hore len ak som dostal # a ukoncilo sa to $ cize tu mam dlzku prijateho stringu medzi # a $ a samotne rx_data s commandom
void checkCommand(uint8_t total_len){

	//togglujem is_auto pomocou prikazov
	if(strcmp(rx_data, "#auto$") == 0){
		is_auto = 1;
	}
	if(strcmp(rx_data, "#manual$") == 0){
		is_auto = 0;
	}
	if(is_auto == 0) {
		//pozriem ci mam #PWM na zaciatku rx_data a zoberiem cislo spoza tohoto prikazu do PWM_man_value
		strncpy(pwm, rx_data, 4);
		strcat(pwm, '\0');
		if(strcmp(pwm, "#PWM") == 0){
			strcpy(value_of_pwm_$, &rx_data[4]);
			strncpy(value_of_pwm, value_of_pwm_$, 2);
			PWM_man_value = atoi(value_of_pwm);
		}
		memset(pwm,0,10);
		memset(value_of_pwm,0,10);
		memset(value_of_pwm_$,0,10);
	}
}

void setDutyCycle(uint8_t D){
	D_value = D;
    TIM2->CCR1 = D_value;
}

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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
