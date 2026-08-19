#include "LightControlTask.h"

static bool paused;

void lightControlTask(void *pvParameters) {
    pinMode(LIGHT_SWITCH_PIN, OUTPUT);
    digitalWrite(LIGHT_SWITCH_PIN, HIGH);
    while(1) {

        if(ulTaskNotifyTake(pdTRUE, 0) == pdTRUE) {
            paused = !paused;
        }

        if(paused) {
            if(lightStatus == true) {
                digitalWrite(LIGHT_SWITCH_PIN, HIGH);
            }
            else {
                digitalWrite(LIGHT_SWITCH_PIN, LOW);
            }

            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

            if(lightStatus == true) {
                digitalWrite(LIGHT_SWITCH_PIN, LOW);
            }
            else {
                digitalWrite(LIGHT_SWITCH_PIN, HIGH);
            }
            paused = !paused;
        }


        if(xSemaphoreTake(lightSemaphore, pdMS_TO_TICKS(500)) == pdTRUE) {
            if(lightStatus == true) {
                digitalWrite(LIGHT_SWITCH_PIN, LOW);
            }
            else {
                digitalWrite(LIGHT_SWITCH_PIN, HIGH);
            }
        }
    }
}