// Include the libraries we need
#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "WiFiHandler.h"
#include "TemperatureTask.h"
#include "LightControlTask.h"
#include "AutoTopOffTask.h"
#include "manualButtonTask.h"

#define LIGHT_BIT (1 << 0)
#define HEATER_BIT (1 << 1)
#define ATO_BIT (1 << 2)
#define FILTER_BIT (1 << 3)

QueueHandle_t publishQueue;
QueueHandle_t subscribeQueue;
SemaphoreHandle_t lightSemaphore;
SemaphoreHandle_t floatSwitchChange;
EventGroupHandle_t pauseTaskEventBits;

TaskHandle_t buttonTaskHandle;
TaskHandle_t lightTaskHandle;
TaskHandle_t temperatureTaskHandle;
TaskHandle_t autoTopOffTaskHandle;

volatile int tempSet;
volatile float tempHysteresis;
volatile bool lightStatus;

bool initResourceSync();

void setup(void)
{
    Serial.begin(9600);
    // // Creating the needed queues and semaphores for signaling and resource sharing
    if(initResourceSync() == false) {
        Serial.println("Failed to create semaphores/queues!");
        while(1);
    }

    // // // Core 0 will handle WiFi and MQTT communications
    xTaskCreatePinnedToCore(WiFiTask, "wifi", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(incomingMessageTask, "incoming", 4096, NULL, 3, NULL, 0);

    // // Core 1 will handle control and monitoring of sensors and actuators.
    xTaskCreatePinnedToCore(temperatureTask, "temperature", 4096, NULL, 1, &temperatureTaskHandle, 1);
    xTaskCreatePinnedToCore(lightControlTask, "light", 1024, NULL, 1, &lightTaskHandle, 1);
    xTaskCreatePinnedToCore(autoTopOffTask, "ato", 1024, NULL, 1, &autoTopOffTaskHandle, 1);
    xTaskCreatePinnedToCore(buttonTask, "buttons", 1024, NULL, 1, &buttonTaskHandle, 1);
}

void loop(void)
{
    // FreeRTOS handles all scheduling and looping.
}

bool initResourceSync() {
    publishQueue = xQueueCreate(10, sizeof(MQTTMessage));
    subscribeQueue = xQueueCreate(5, sizeof(MQTTMessage));
    lightSemaphore = xSemaphoreCreateBinary();
    floatSwitchChange = xSemaphoreCreateBinary();
    pauseTaskEventBits = xEventGroupCreate();

    if(publishQueue == NULL || subscribeQueue == NULL || lightSemaphore == NULL ||
        floatSwitchChange == NULL || pauseTaskEventBits == NULL) {
        return false;
    }
    else {
        return true;
    }
}
