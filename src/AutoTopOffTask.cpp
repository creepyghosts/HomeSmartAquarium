#include "AutoTopOffTask.h"

void autoTopOffTask(void *pvParameters) {
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW); // Start pump OFF
    pinMode(FLOAT_SWITCH_PIN, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(FLOAT_SWITCH_PIN), floatSwitchISR, FALLING);

    unsigned long pumpStartTime = 0;
    bool pumpOn = false;
    bool paused = false;
    ATO_STATE currentState;

    while(1) {

        if(ulTaskNotifyTake(pdTRUE, 0) == pdTRUE) {
            paused = !paused;
        }

        if(paused) {
            if(pumpOn) {
                digitalWrite(PUMP_PIN, LOW);
            }
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            if(pumpOn) {
                digitalWrite(PUMP_PIN, HIGH);
            }
            paused = !paused;
        }

        switch(currentState) {

            case ATO_IDLE:
            
                if(xSemaphoreTake(floatSwitchChange, pdMS_TO_TICKS(500)) == pdTRUE) {
                    currentState = ATO_FILLING;
                }
                break;

            case ATO_FILLING: {
                
                tankLevel currentTankLevel = readFloatSwitchWithDebounce(); // Polling tank level

                if(currentTankLevel == TANK_FULL) {
                    pumpOn = false;
                    digitalWrite(PUMP_PIN, LOW);
                    currentState = ATO_IDLE;
                }
                
                if (!pumpOn) {
                    pumpOn = true;
                    digitalWrite(PUMP_PIN, HIGH);
                    pumpStartTime = xTaskGetTickCount();
                }

                if (pumpOn && (xTaskGetTickCount() - pumpStartTime > pdMS_TO_TICKS(MAXIMUM_ON_TIME))) {
                    pumpOn = false;
                    digitalWrite(PUMP_PIN, LOW);
                    currentState = ATO_FAULT;
                }

                vTaskDelay(pdMS_TO_TICKS(50));

                break;
            }

            case ATO_FAULT:
                break;
        }
    }
}

void floatSwitchISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(floatSwitchChange, &xHigherPriorityTaskWoken); // Signal that float switch state has changed
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

tankLevel readFloatSwitchWithDebounce(void) {
    const int samples = 5;
    const int sampleIntervalMs = 5;

    int highCount = 0;
    int lowCount = 0;

    for(int i = 0; i < samples; i++) {
        int reading = digitalRead(FLOAT_SWITCH_PIN);
        if(reading == HIGH) {
            highCount++;
        }
        else {
            lowCount++;
        }

        vTaskDelay(pdMS_TO_TICKS(sampleIntervalMs)); // Wait 5ms between samples
        
    }

    return highCount > lowCount ? TANK_FULL : TANK_LOW;

}