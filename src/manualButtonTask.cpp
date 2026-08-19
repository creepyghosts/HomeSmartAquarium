#include "manualButtonTask.h"

void buttonTask(void *pvParameters) {

    uint32_t buttonPressed = 0;
    bool lightOn = true;
    bool heaterOn = true;
    bool atoOn = true;
    bool filterOn = true;

    initButtons();

    while(1) {

        xTaskNotifyWait(0, 0xFFFFFFFF, &buttonPressed, portMAX_DELAY);

        if(buttonPressed & LIGHT_BIT) {
            if(readButtonWithDebounce(LIGHT_BUTTON) == LOW) {
                xTaskNotifyGive(lightTaskHandle);
                if(lightOn) {
                    digitalWrite(LIGHT_STATUS_LED, LOW);
                    lightOn = false;
                }
                else {
                    digitalWrite(LIGHT_STATUS_LED, HIGH);
                    lightOn = true;
                }
            }
        }

        if(buttonPressed & HEATER_BIT) {
            if(readButtonWithDebounce(HEATER_BUTTON) == LOW) {
                xTaskNotifyGive(temperatureTaskHandle);
                if(heaterOn) {
                    digitalWrite(HEATER_STATUS_LED, LOW);
                    heaterOn = false;
                }
                else {
                    digitalWrite(HEATER_STATUS_LED, HIGH);
                    heaterOn = true;
                }
            }
        }

        if(buttonPressed & ATO_BIT) {
            if(readButtonWithDebounce(ATO_BUTTON) == LOW) {
                xTaskNotifyGive(autoTopOffTaskHandle);
                if(atoOn) {
                    digitalWrite(ATO_STATUS_LED, LOW);
                    atoOn = false;
                }
                else {
                    digitalWrite(ATO_STATUS_LED, HIGH);
                    atoOn = true;
                }
            }
        }

        if(buttonPressed & FILTER_BIT) { // No filter task so handle toggling filter here
            if(readButtonWithDebounce(FILTER_BUTTON) == LOW) {
                if(filterOn) {
                    digitalWrite(FILTER_STATUS_LED, LOW);
                    digitalWrite(FILTER_CONTROL, LOW);
                    filterOn = false;
                }
                else {
                    digitalWrite(FILTER_STATUS_LED, HIGH);
                    digitalWrite(FILTER_CONTROL, HIGH);
                    filterOn = true;
                }
            }
        }

    }

}

bool readButtonWithDebounce(int buttonPin) {
    const int samples = 5;
    const int sampleIntervalMs = 5;

    int highCount = 0;
    int lowCount = 0;

    for(int i = 0; i < samples; i++) {
        int reading = digitalRead(buttonPin);
        if(reading == HIGH) {
            highCount++;
        }
        else {
            lowCount++;
        }

        vTaskDelay(pdMS_TO_TICKS(sampleIntervalMs)); // Wait 5ms between samples
    }

    return highCount > lowCount ? HIGH : LOW;
}

void lightButtonISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(buttonTaskHandle, LIGHT_BIT, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void heaterButtonISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(buttonTaskHandle, HEATER_BIT, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ATOButtonISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(buttonTaskHandle, ATO_BIT, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void filterButtonISR(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(buttonTaskHandle, FILTER_BIT, eSetBits, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void initButtons(void) {
    pinMode(LIGHT_BUTTON, INPUT_PULLUP);
    pinMode(HEATER_BUTTON, INPUT_PULLUP);
    pinMode(ATO_BUTTON, INPUT_PULLUP);
    pinMode(FILTER_BUTTON, INPUT_PULLUP);
    pinMode(FILTER_CONTROL, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(LIGHT_BUTTON), lightButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(HEATER_BUTTON), heaterButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(ATO_BUTTON), ATOButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(FILTER_BUTTON), filterButtonISR, FALLING);

    digitalWrite(FILTER_CONTROL, LOW);
}