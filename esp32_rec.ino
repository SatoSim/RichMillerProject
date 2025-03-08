#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

#define CE_PIN 4
#define CSN_PIN 5
#define BUTTON_PIN 34 // Pin connected to the button

// Declare the radio object for NRF24 communication
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// Create a hardware serial instance for the MP3 module
HardwareSerial mySerial(1); // Using UART1 for DFPlayer Mini communication

// Create the MP3 player object
DFRobotDFPlayerMini mp3;

// Setup button state variable
bool lastButtonState = LOW;
bool buttonPressed = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize the button pin as input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize the MP3 player with the serial connection
  mySerial.begin(9600, SERIAL_8N1, 16, 17);  // RX, TX pins for DFPlayer Mini
  if (!mp3.begin(mySerial)) {
    Serial.println("MP3 player initialization failed.");
    while (1);  // Halt if the MP3 player doesn't initialize
  }
  mp3.volume(20);  // Set volume level (0 to 30)

  // Initialize the radio (NRF24)
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();
  Serial.println("Receiver ready. Waiting for signal...");
}

void loop() {
  // Check for button press
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Button pressed (LOW means button is pressed)
    Serial.println("Button pressed!");
    playTrack();
    delay(200);  // Debounce delay
  }
  lastButtonState = buttonState;

  // Listen for incoming messages (button press from another device)
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.print("Received: ");
    Serial.println(text);
    if (strcmp(text, "Hello World") == 0) {
      playTrack();  // Play track when the signal is received
    }
  }
}

void playTrack() {
  // Play MP3 track
  mp3.play(1);  // Play track number 1
  Serial.println("Playing track 1...");
}
