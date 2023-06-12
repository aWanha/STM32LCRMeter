/*
 * LCRFunctions.c
 *
 *  Created on: May 21, 2023
 *      Author: home
 */
#include "main.h"
#include "tim.h"
#include "math.h"

void timerSettings(){

  HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_2);

  HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_3);

  HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_1);

  uint32_t filter = 8;

  TIM3->CCMR2 &= ~(0xF << 4);
  TIM3->CCMR2 |= (filter<<12);

  TIM3->CCMR1 &= ~(0xF << 12);
  TIM3->CCMR1 |= (filter<<12);

  }

uint8_t compare(double valueTarget,double valueCompare, double maxDeviation){

     if( ( (1 - maxDeviation) * valueCompare <= valueTarget) && (valueTarget <= (1 + maxDeviation) * valueCompare) )return 1;

    else return 0;

}

double filterList(double maxDeviation, uint32_t *list, uint32_t list_len){

    uint16_t acceptedValues[list_len];

     for(uint32_t i = 0; i < list_len; i++){

        acceptedValues[i] = 0;
     }

    uint32_t highestWeight = 0;
    uint32_t highestWeight_index = 0;

    for(uint32_t i = 0; i < list_len; i++){

        if(acceptedValues[i] != 0)continue; //skip iteration if sample[i] has already received weight, reduces amount of iteration but will drift the center of values towards first matching sample[settings->resultIndex]

        for(uint32_t j = 0; j < list_len; j++){

            if(i == j)continue;

            if(compare(list[i],list[j],maxDeviation)){

                acceptedValues[i] +=1;

                acceptedValues[j] +=1;

                if(acceptedValues[j] > highestWeight){

                    highestWeight = acceptedValues[j];
                    highestWeight_index = j;
                }
            }
        }
    }
    double average = 0;
    uint16_t count = 0;

    for(uint32_t i = 0; i < list_len; i++){

        if(compare(list[i],list[highestWeight_index],maxDeviation*1.2)){

            average += list[i];
            count +=1;
        }
    }
    return average / count;
}


uint8_t measureLCR(uint32_t *buffer, uint32_t bufferSize,uint16_t delay){

  for(uint32_t i = 0; i < bufferSize; i++){

    HAL_Delay(delay);
    buffer[i] = TIM3->CCR2 + (TIM2->CCR3)*65535;
  }

  return 1;
  }

double calculateLCR(measureSettings *settings){

  double result = 0;

  uint8_t i = settings->resultIndex;

  //if(settings->Farad_HenryResult[i] == -1)return -1; //if reactance measurement was discarded, esr measurement is skipped

  if( clockMHz *10000 < settings->sample[i]||settings->sample[i] < clockMHz/2){ // if sample[i] width is over 10mS or below 1/2 uS it is discarded and indicator "-1" is placed (too large reactance to resistance difference)

    if(settings->r20Active){ //ESR MEASUREMENT

      settings->esrResult[i] = settings->sample[i];

    }else{

      settings->Farad_HenryResult[i] = -1;
    }

    return -1;
  }

  if(settings->r20Active){ //ESR MEASUREMENT

    if( settings->currentLeading ){//CAPACITIVE

      settings->Farad_HenryResultR20[i] = -1/(settings->measureResistance * tan(M_PI * 2 * settings->sample[i] / (double)clockFreq * settings->frequency) * 2 * M_PI * settings->frequency);

      settings->reactance[i] = -1/(settings->Farad_HenryResult[i] * 2 *M_PI * settings->frequency);

      settings->esrResult[i] = (settings->Farad_HenryResult[i] /settings->Farad_HenryResultR20[i] - 1) * settings->measureResistance;

      //settings->esrResult[i] = ( settings->reactance[i]/(tan(M_PI * 2 * settings->sample[i] / (double)clockFreq * settings->frequency) * 2 * M_PI * settings->frequency) ) - settings->measureResistance;

    }else{//INDUCTIVE

      settings->Farad_HenryResultR20[i] = tan(M_PI * 2 * settings->sample[i] / (double)clockFreq * settings->frequency) * settings->measureResistance / 2 / M_PI / settings->frequency;

      settings->reactance[i] = (settings->Farad_HenryResult[i] * 2 *M_PI * settings->frequency);

      settings->esrResult[i] = ( settings->Farad_HenryResultR20[i] / settings->Farad_HenryResult[i] - 1) * settings->measureResistance;

      //settings->esrResult[i] = ( (settings->reactance[i] / tan(M_PI * 2 * settings->sample[i] / (double)clockFreq * settings->frequency) * 2 * M_PI * settings->frequency) - settings->measureResistance);
    }

  }else{ //REACTANCE MEASUREMENT

    if( settings->currentLeading ){//CAPACITIVE

      settings->Farad_HenryResult[i] = -1/(settings->measureResistance * tan(M_PI * 2 * settings->sample[i] / (double)clockFreq *settings->frequency) * 2 * M_PI * settings->frequency);

    }else{//INDUCTIVE

      settings->Farad_HenryResult[i] = tan(M_PI * 2 * settings->sample[i] / (double)clockFreq *settings->frequency) * settings->measureResistance / 2 / M_PI / settings->frequency;
    }

  }

  return result;
}

uint8_t currentLeading(){

  uint16_t count = 0;
  uint16_t timeoutStart = TIM4->CNT;
  uint16_t timeout = 50000; //50 ms, 65ms max
  while( (count < 20) && ( (uint16_t)(TIM4->CNT - timeoutStart) < timeout) ){ //wait until both signals have been high for 20 iterations, iterations for filtering

    if((GPIOB->IDR & GPIO_PIN_4) && (GPIOB->IDR & GPIO_PIN_5)){

      count +=1;

    }else count = 0;
  }

  timeoutStart = TIM4->CNT;
  count = 0;
  while(count < 20 && ( (uint16_t)(TIM4->CNT - timeoutStart) < timeout) ){ //wait until both signals have been low for 20 iterations, iterations for filtering

    if(!(GPIOB->IDR & GPIO_PIN_4) && !(GPIOB->IDR & GPIO_PIN_5)){

      count +=1;

    }else{

      count = 0;

    }
  }


  timeoutStart = TIM4->CNT;
  while( (uint16_t)(TIM4->CNT - timeoutStart < 2) ){} //2 uS delay

  timeoutStart = TIM4->CNT;
  while( (uint16_t)(TIM4->CNT - timeoutStart) < timeout ){ //which signal rises first

    if(GPIOB->IDR & GPIO_PIN_4) return 1;
    if(GPIOB->IDR & GPIO_PIN_5) return 0;
  }

  return 99; //error
}


void createSineWave(sineWaveSettings *settings, uint32_t bufferSize){

    uint16_t maxPwmVal = 40;
    uint8_t divider = 1;
    uint32_t dataPoints = clockFreq / settings->frequency;

    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(lowpassFilter_GPIO_Port, lowpassFilter_Pin, settings->lowpassFilter);

    //lazy division
    for(;maxPwmVal < 255-1; maxPwmVal +=1){ //increase resolution and slice duration

      if(dataPoints/maxPwmVal < sineWaveArray_len-5)break;

    }

    dataPoints /= maxPwmVal;
    TIM1->ARR = maxPwmVal;

    for(;divider < 255-1; divider +=1){ // if signal cant be sliced into given array size, make slices longer by slowing timer (prescaler)

      if(dataPoints/divider < bufferSize-5)break;

    }
    TIM1->PSC = divider - 1;
    dataPoints /= divider;
    float stepSize = 2*3.1416/ dataPoints;

   for(uint16_t i = 0;i < dataPoints; i++){

    float result = i*stepSize;

    sineWaveArray[i] = ( 1 + ( sin(result) * settings->amplitudeCorrection ) ) * maxPwmVal / 2;

   }
   HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)sineWaveArray, dataPoints);

}
