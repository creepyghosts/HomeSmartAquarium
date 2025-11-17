#include "LightControlTask.h"

void lightControlTask(void *pvParameters) {
    pinMode(LIGHT_SWITCH_PIN, OUTPUT);
    while(1) {
        if(xSemaphoreTake(lightSemaphore, portMAX_DELAY) == pdTRUE) {
            if(lightStatus == true) {
            digitalWrite(LIGHT_SWITCH_PIN, HIGH);
            }
            else {
                digitalWrite(LIGHT_SWITCH_PIN, LOW);
            }
        }
    }
}