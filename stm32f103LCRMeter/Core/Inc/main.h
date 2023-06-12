/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define lowpassFilter_Pin GPIO_PIN_1
#define lowpassFilter_GPIO_Port GPIOB
#define res20_Pin GPIO_PIN_10
#define res20_GPIO_Port GPIOB
#define res1k_Pin GPIO_PIN_11
#define res1k_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define sineWaveArray_len 8192
extern uint8_t sineWaveArray[sineWaveArray_len];

#define clockFreq 72000000
#define clockMHz (clockFreq / 1000000)
#define voltageRising TIM_SR_CC1IF_Msk
#define voltageFalling TIM_SR_CC2IF_Msk
#define currentRising TIM_SR_CC3IF_Msk
#define currentFalling TIM_SR_CC4IF_Msk

typedef struct sineWaveSettings{

    uint32_t frequency;
    double amplitudeCorrection;
    uint8_t lowpassFilter;

}sineWaveSettings;

#define resultSize 5
typedef struct measureSettings{
  uint32_t sample[resultSize];
  uint32_t frequency;

  double measureResistance;
  double Farad_HenryResult[resultSize];
  double Farad_HenryResultR20[resultSize];
  double esrResult[resultSize];
  double reactance[resultSize];
  uint8_t resultIndex;
  uint8_t currentLeading;


  uint8_t r20Active;


}measureSettings;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
