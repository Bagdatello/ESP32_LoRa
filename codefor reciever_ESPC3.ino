
#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK    4
#define LORA_MISO   5
#define LORA_MOSI   6
#define LORA_SS     7
#define LORA_RST   10
#define LORA_DIO0   3

#define LED_PIN     1
#define BUZZER_PIN  0

bool ledState = false;

unsigned long lastSequence = 0;
bool receivedBefore = false;

void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  SPI.begin(LORA_SCK, LORA_MISO,
            LORA_MOSI, LORA_SS);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa ERROR!");
    while (true) {
      delay(1000);
    }
  }

  LoRa.enableCrc();

  Serial.println("RECEIVER READY!");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize == 0) {
    return;
  }

  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.println("Received: " + message);

  if (message.startsWith("TOGGLE:")) {
    String numberText = message.substring(7);

    unsigned long seq = numberText.toInt();

    if (seq == 0) {
      return;
    }

    if (receivedBefore && seq == lastSequence) {
      return;
    }

    lastSequence = seq;
    receivedBefore = true;

    ledState = !ledState;

    digitalWrite(LED_PIN,
                 ledState ? HIGH : LOW);

    beep();

    if (ledState) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }

    Serial.print("RSSI: ");
    Serial.print(LoRa.packetRssi());
    Serial.println(" dBm");
  }
}
