#ifndef __AUTOTOPOFFTASK_H__
#define __AUTOTOPOFFTASK_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "WiFiHandler.h"

#define PUMP_PIN GPIO_NUM_26
#define FLOAT_SWITCH_PIN GPIO_NUM_25
#define MAXIMUM_ON_TIME 120000

extern SemaphoreHandle_t floatSwitchChange;

typedef enum {
    TANK_LOW,
    TANK_FULL
} tankLevel;

typedef enum {
    ATO_IDLE,
    ATO_FILLING,
    ATO_FAULT
} ATO_STATE;

void autoTopOffTask(void *pvParameters);
void floatSwitchISR(void);
tankLevel readFloatSwitchWithDebounce(void);


#endif