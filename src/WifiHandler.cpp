#include "WiFiHandler.h"

void setupWifi() {
    WiFi.mode(WIFI_STA); // Set ESP32 as WiFi Station
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        digitalWrite(CONNECTED_LED, LOW);
        Serial.print(".");
        delay(100);
    }

    digitalWrite(CONNECTED_LED, HIGH);
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

void setupMqtt() {
    client.setServer(mqttBroker, mqttPort);
    client.setCallback(callback);
}

void reconnectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "SmartAquariumESP32";
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUsername, mqttPassword)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(TEMPERATURE_SET_TOPIC);
      client.subscribe(LIGHT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/*
    Main running task for handling all WiFi and MQTT connections
    and communications needed for the Smart Aquarium.
*/
void WiFiTask(void *pvParameters) {
    pinMode(CONNECTED_LED, OUTPUT);
    setupWifi();
    setupMqtt();
    MQTTMessage outgoingMessage;

    while(1) {
        if(!client.connected()) {
            reconnectMqtt();
        }
        client.loop();

        // Waiting for outgoing messages...
        if(xQueueReceive(publishQueue, &outgoingMessage, 100) == pdPASS) {
            client.publish(outgoingMessage.topicName, outgoingMessage.payload);
        }
        else {
            delay(100);
        }
    }
}

/*
    This task is mainly used for processing incoming messages and modifying the
    necessary variables for control.
*/

void incomingMessageTask(void *pvParameters) {
    while(1) {
        MQTTMessage incomingMessage;
        if(xQueueReceive(subscribeQueue, &incomingMessage, portMAX_DELAY) == pdPASS) {
            if(strcmp(incomingMessage.topicName, TEMPERATURE_SET_TOPIC) == 0) {
                if(incomingMessage.payload[0] == '0') { // The following two numbers are for setting desired temp.
                    char temp[3];
                    strncpy(temp, (incomingMessage.payload + 1), 2);
                    temp[2] = '\0'; // Null terminated
                    tempSet = atoi(temp);
                }
                else { // Following number is for setting temperature hysteresis band
                    char temp[4];
                    strncpy(temp, (incomingMessage.payload + 1), 3);
                    temp[3] = '\0';
                    tempHysteresis = atof(temp);
                }
            } 
            else if(strcmp(incomingMessage.topicName, LIGHT_TOPIC) == 0) {
                if(incomingMessage.payload[0] == '1') {
                    lightStatus = true;
                }
                else {
                    lightStatus = false;
                }
                xSemaphoreGive(lightSemaphore);
            }
        }
    }
}

/*
    This callback is used for receiving the commands from the NodeRed dashboard.
    Current functionality supported:
    Setting Desired Temperature Values
    Turning on and off lights at set times

*/
void callback(char *topic, byte *payload, unsigned int length) {
    MQTTMessage incomingMessage;
    strcpy(incomingMessage.topicName, topic);
    strcpy(incomingMessage.payload, (char*)payload);
    xQueueSend(subscribeQueue, &incomingMessage, 1000);
}