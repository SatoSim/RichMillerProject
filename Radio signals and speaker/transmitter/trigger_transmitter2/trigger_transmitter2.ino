// Transmitter Code
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8
#define BUTTON_PIN 2

RF24 radio(CE_PIN, CSN_PIN);
const byte address1[6] = "00001";
volatile bool sendSignal = false;
unsigned long lastPressTime = 0;

void buttonPressed() {
    unsigned long currentTime = millis();
    if (currentTime - lastPressTime > 500) {  
        sendSignal = true;
        lastPressTime = currentTime;  
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, FALLING);

    radio.begin();
    radio.setRetries(5, 15);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setPALevel(RF24_PA_HIGH);
    radio.stopListening(); // Set as transmitter
    radio.openWritingPipe(address1);
}

void loop() {
    if (sendSignal) {
        sendSignal = false;

        int message = 1;

        for (int i = 0; i < 3; i++) {  
            bool success = radio.write(&message, sizeof(message));
            if (success) {
                Serial.println("Pulse sent successfully!");
                break; 
            } else {
                Serial.println("Retry sending pulse...");
                delay(50);
            }
        }
    }
}