// Include the libraries we need
#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "WiFiHandler.h"
#include "TemperatureTask.h"
#include "LightControlTask.h"

QueueHandle_t publishQueue;
QueueHandle_t subscribeQueue;
SemaphoreHandle_t lightSemaphore;

int tempSet;
float tempHysteresis;
bool lightStatus;

void setup(void)
{
    Serial.begin(9600);
    // // Creating the needed queues and semaphores for signaling and resource sharing
    publishQueue = xQueueCreate(10, sizeof(MQTTMessage));
    subscribeQueue = xQueueCreate(5, sizeof(MQTTMessage));
    lightSemaphore = xSemaphoreCreateBinary();

    // // Core 0 will handle WiFi and MQTT communications
    xTaskCreatePinnedToCore(WiFiTask, "wifi", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(incomingMessageTask, "incoming", 4096, NULL, 3, NULL, 0);

    // Core 1 will handle control and monitoring of sensors and actuators.
    xTaskCreatePinnedToCore(temperatureTask, "temperature", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(lightControlTask, "light", 1024, NULL, 1, NULL, 1);
}

void loop(void)
{
    // FreeRTOS handles all scheduling and looping.   
}

