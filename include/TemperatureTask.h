#ifndef __TEMPERATURETASK_H__
#define __TEMPERATURETASK_H__

#include <Arduino.h>

#include "OneWire.h"
#include "DallasTemperature.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "WiFiHandler.h"

#define ONE_WIRE_BUS GPIO_NUM_32
#define HEATER_PIN GPIO_NUM_27

static OneWire sensorBus(ONE_WIRE_BUS);
static DallasTemperature sensors(&sensorBus);
static DeviceAddress sensorAddress;

extern QueueHandle_t publishQueue;
volatile extern int tempSet;
volatile extern float tempHysteresis;

void initSensors();
void printAddress(DeviceAddress address);
void temperatureTask(void *pvParameters);

#endif