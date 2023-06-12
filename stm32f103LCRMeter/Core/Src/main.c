/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "LCRFunctions.h"
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t sineWaveArray[sineWaveArray_len];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  //amplitudeCorrection to set the amplitude to within 200mV pk-pk, value reduces dutycycle width to lower amplitude
  sineWaveSettings Hz50; Hz50.frequency = 50; Hz50.amplitudeCorrection = 0.267857143, Hz50.lowpassFilter = 1;
  sineWaveSettings Hz1000; Hz1000.frequency = 1000; Hz1000.amplitudeCorrection = 0.240384615,Hz1000.lowpassFilter = 1;
  sineWaveSettings Hz10000; Hz10000.frequency = 10000; Hz10000.amplitudeCorrection = 0.319672131,Hz10000.lowpassFilter = 0;
  sineWaveSettings Hz50000; Hz50000.frequency = 50000; Hz50000.amplitudeCorrection = 0.339673913,Hz50000.lowpassFilter = 0;
  sineWaveSettings Hz100000; Hz100000.frequency = 100000; Hz100000.amplitudeCorrection = 1.0, Hz100000.lowpassFilter = 0;

  measureSettings measureSettings;
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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  timerSettings();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  const uint16_t sampleN = 10;
  uint32_t average[sampleN];
  measureSettings.r20Active = 0;

  while (1)
  {
    for(uint8_t i = 0; i < resultSize; i ++){ //null all samples

      measureSettings.reactance[i] = 0;
      measureSettings.Farad_HenryResult[i] = 0;
      measureSettings.Farad_HenryResultR20[i]= 0;
      measureSettings.esrResult[i] = 0;
    }

    for(uint8_t k = 0; k < 2; k++){


      for(uint8_t j = 0; j < 5; j ++){ //simple states to iterate through frequencies

        if(j==0){
          createSineWave(&Hz50, sineWaveArray_len);
          measureSettings.frequency = Hz50.frequency;
        }
        if(j==1){
          createSineWave(&Hz1000,sineWaveArray_len);
          measureSettings.frequency = Hz1000.frequency;
        }
        if(j==2){
          createSineWave(&Hz10000,sineWaveArray_len);
          measureSettings.frequency = Hz10000.frequency;
        }
        if(j==3){
          createSineWave(&Hz50000,sineWaveArray_len);
          measureSettings.frequency = Hz50000.frequency;
        }
        if(j==4){
          createSineWave(&Hz100000,sineWaveArray_len);
          measureSettings.frequency = Hz100000.frequency;
        }

        if(measureSettings.r20Active){

          HAL_GPIO_WritePin(res20_GPIO_Port, res20_Pin, 1);

          HAL_GPIO_WritePin(res1k_GPIO_Port, res1k_Pin, 0);
          measureSettings.measureResistance = 20.54;
        }else{

          HAL_GPIO_WritePin(res20_GPIO_Port, res20_Pin, 0);

          HAL_GPIO_WritePin(res1k_GPIO_Port, res1k_Pin, 1);
          measureSettings.measureResistance = 1000;
        }

        HAL_Delay(500);
        measureSettings.currentLeading = currentLeading();

        measureLCR(average, sampleN,50);

        measureSettings.sample[j] = filterList(0.3, average, sampleN);
        measureSettings.resultIndex = j;
        calculateLCR(&measureSettings);
      }
      measureSettings.r20Active = !(measureSettings.r20Active);
    }
  }
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
