#include "TemperatureTask.h"

// Function that initializes all connected DS18B20 sensors
void initSensors() {
    // locate devices on the bus
    Serial.print("Locating devices...");
    sensors.begin();
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount(), DEC);
    Serial.println(" devices.");

    if (!sensors.getAddress(sensorAddress, 0)) Serial.println("Unable to find address for Device 0");

    Serial.print("Device 0 Address: ");
    printAddress(sensorAddress);
    Serial.println();

    sensors.setResolution(sensorAddress, 9);

    Serial.print("Device 0 Resolution: ");
    Serial.print(sensors.getResolution(sensorAddress), DEC);
    Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void temperatureTask(void *pvParameters) {

    initSensors();
    pinMode(HEATER_PIN, OUTPUT);
    digitalWrite(HEATER_PIN, HIGH);

    float temperatureReading {0};
    tempSet = 76;
    tempHysteresis = 1;

    while(1) {
        sensors.requestTemperatures();
        vTaskDelay(pdMS_TO_TICKS(5000));
        temperatureReading = sensors.getTempF(sensorAddress);
        if(temperatureReading != DEVICE_DISCONNECTED_F) {
            // Hysteresis control of the heater power.
            if(temperatureReading >= (tempSet + tempHysteresis)) {
                digitalWrite(HEATER_PIN, HIGH);
            }
            else if(temperatureReading <= (tempSet - tempHysteresis)) {
                digitalWrite(HEATER_PIN, LOW);
            }
            MQTTMessage temperature;
            strcpy(temperature.topicName, TEMPERATURE_READ_TOPIC);
            dtostrf(temperatureReading, 6, 2, temperature.payload);
            xQueueSend(publishQueue, &temperature, pdMS_TO_TICKS(100));
        }
        else {
            Serial.println("Unable to read temperature sensor data!");
        }
    }
}
