/*
This header contains all the files needed for handling the WiFi 
communication for the SmartAquarium.
*/
#ifndef __WIFIHANDLER_H__
#define __WIFIHANDLER_H__

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "arduino_secrets.h"

#define CONNECTED_LED GPIO_NUM_2
#define TEMPERATURE_READ_TOPIC "home/livingroom/smartaquarium/temperature/readings"
#define TEMPERATURE_SET_TOPIC "home/livingroom/smartaquarium/temperature/settings"
#define LIGHT_TOPIC "home/livingroom/smartaquarium/light"
#define TOP_OFF_TOPIC "home/livingroom/smartaquarium/autotopoff"

typedef struct {
    char topicName[60];
    char payload[20];
} MQTTMessage;

extern QueueHandle_t publishQueue;
extern QueueHandle_t subscribeQueue;
extern SemaphoreHandle_t lightSemaphore;
volatile extern int tempSet;
volatile extern float tempHysteresis;
volatile extern bool lightStatus;

static WiFiClient espClient;
static PubSubClient client(espClient);

void setupWifi();
void setupMqtt();
void reconnectMqtt();
void WiFiTask(void *pvParameters);
void incomingMessageTask(void *pvParameters);
void callback(char *topic, byte *payload, unsigned int length);

#endif