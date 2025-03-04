// Receiver Code
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "mp3tf16p.h"

#define CE_PIN 7
#define CSN_PIN 8
#define LED_PIN 4

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";
MP3Player mp3(2, 3);

bool ledState = false;
unsigned long ledTimer = 0;
const unsigned long ledDuration = 1000;

void setup() {
    Serial.begin(9600);
    
    mp3.initialize();
    Serial.println("MP3 module initialized.");

    if (!radio.begin()) {
        Serial.println("Radio module failed to initialize!");
        while (1);
    }

    radio.setRetries(5, 15);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setPALevel(RF24_PA_HIGH);
    radio.openReadingPipe(0, address);
    radio.startListening();

    pinMode(LED_PIN, OUTPUT);
}

bool isPlaying = false;

void activate() {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
    ledTimer = millis();

    if (!isPlaying) {
        mp3.playTrackNumber(1, 25);
        isPlaying = true;
        Serial.println("Track playing...");
        delay(100);  // Give MP3 module time to process
    }
}

void loop() {
    int message = 0;

    if (radio.available()) {
        radio.read(&message, sizeof(message));
        if (message == 1) {  
            Serial.println("Received: activate");
            activate();
            char ack[] = "ACK";
            radio.writeAckPayload(1, &ack, sizeof(ack));
        } else {
            Serial.print("Unexpected message: ");
            Serial.println(message);
        }
    }

    // Turn off LED after duration
    if (ledState && millis() - ledTimer >= ledDuration) {
        digitalWrite(LED_PIN, LOW);
        ledState = false;
        isPlaying = false;  // Reset track playing flag
    }
}
