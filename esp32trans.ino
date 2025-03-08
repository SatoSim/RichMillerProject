#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 5
#define BUT_PIN 25

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

unsigned long lastPressTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUT_PIN, INPUT_PULLUP);

  if (!radio.begin()) {
    Serial.println("NRF24 initialization failed.");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Transmitter ready.");
}

void loop() {
  // Check if button is pressed (LOW due to INPUT_PULLUP)
  if (digitalRead(BUT_PIN) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastPressTime > 500) {  // Debounce delay
      sendMessage();
      lastPressTime = currentTime;
    }
  }
}

void sendMessage() {
  const char text[] = "Hello World";
  bool success = radio.write(&text, sizeof(text));
  if (success) {
    Serial.println("Message sent: Hello World");
  } else {
    Serial.println("Failed to send message.");
  }
}
