
#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26

#define BUTTON_PIN 27

bool buttonState = HIGH;
bool lastButtonReading = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

unsigned long signalNumber = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SPI.begin(
    LORA_SCK,
    LORA_MISO,
    LORA_MOSI,
    LORA_SS
  );

  LoRa.setPins(
    LORA_SS,
    LORA_RST,
    LORA_DIO0
  );

  if (!LoRa.begin(433E6)) {
    Serial.println("LORA ERROR!");
    while (true) {
      delay(1000);
    }
  }

  LoRa.enableCrc();
  LoRa.setTxPower(10);

  Serial.println("TRANSMITTER READY!");
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      // Кнопка басылған сәт
      if (buttonState == LOW) {

        signalNumber++;

        String message =
          "TOGGLE:" + String(signalNumber);

        LoRa.beginPacket();
        LoRa.print(message);
        LoRa.endPacket();

        Serial.print("SIGNAL SENT: ");
        Serial.println(message);
      }
    }
  }

  lastButtonReading = reading;
}
