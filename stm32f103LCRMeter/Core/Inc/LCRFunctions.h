/*
 * LCRFunctions.h
 *
 *  Created on: May 21, 2023
 *      Author: home
 */

#ifndef INC_LCRFUNCTIONS_H_
#define INC_LCRFUNCTIONS_H_

#include "main.h"

uint8_t compare(double valueTarget,double valueCompare, double maxDeviation);

double filterList(double maxDeviation, uint32_t *list, uint32_t list_len);

void timerSettings();

uint8_t measureLCR(uint32_t *buffer, uint32_t bufferSize,uint16_t delay);

uint8_t currentLeading();

double calculateLCR(measureSettings *settings);

void createSineWave(sineWaveSettings *settings, uint32_t bufferSize);



#endif /* INC_LCRFUNCTIONS_H_ */
