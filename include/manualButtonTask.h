#ifndef __MANUAL_BUTTON_TASK_H__
#define __MANUAL_BUTTON_TASK_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"

#define LIGHT_BUTTON      4
#define HEATER_BUTTON     5
#define ATO_BUTTON       13
#define FILTER_BUTTON    16
#define FILTER_CONTROL   17

#define LIGHT_STATUS_LED 18
#define HEATER_STATUS_LED 19
#define ATO_STATUS_LED 21
#define FILTER_STATUS_LED 22

#define LIGHT_BIT        (1 << 0)
#define HEATER_BIT       (1 << 1)
#define ATO_BIT          (1 << 2)
#define FILTER_BIT       (1 << 3)

extern TaskHandle_t buttonTaskHandle;
extern TaskHandle_t lightTaskHandle;
extern TaskHandle_t temperatureTaskHandle;
extern TaskHandle_t autoTopOffTaskHandle;

void buttonTask(void* pvParameters);
bool readButtonWithDebounce(int buttonPin);
void lightButtonISR(void);
void heaterButtonISR(void);
void ATOButtonISR(void);
void filterButtonISR(void);
void initButtons(void);

#endif