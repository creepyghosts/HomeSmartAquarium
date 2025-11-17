#ifndef __LIGHTCONTROLTASK_H__
#define __LIGHTCONTROLTASK_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "WifiHandler.h"

#define LIGHT_SWITCH_PIN GPIO_NUM_14

extern bool lightStatus;
extern SemaphoreHandle_t lightSemaphore;

void lightControlTask(void *pvParameters);

#endif