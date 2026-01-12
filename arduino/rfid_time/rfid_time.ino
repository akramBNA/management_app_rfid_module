#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_PIN 13

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("RFID ready.");
  delay(2000); // give Python script time to connect
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Blink LED
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);

  // Print UID
  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(" ");
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  // Request PC time
  Serial.println(" | REQUEST_TIME");

  // Wait for PC time with timeout
  String pcTime = "";
  unsigned long start = millis();
  while (millis() - start < 3000) { // wait max 3 seconds
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') break;
      pcTime += c;
    }
    if (pcTime.length() > 0) break;
  }

  if (pcTime.length() == 0) pcTime = "TIME NOT RECEIVED";

  Serial.print("Card scanned at: ");
  Serial.println(pcTime);

  // Halt PICC
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(500);
}

