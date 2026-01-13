#include <SPI.h>
#include <MFRC522.h>

// RFID pins
#define SS_PIN 10
#define RST_PIN 9

// LEDs
#define BUILTIN_LED 13
#define EXT_LED_PIN 7

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(EXT_LED_PIN, OUTPUT);

  digitalWrite(BUILTIN_LED, LOW);
  digitalWrite(EXT_LED_PIN, LOW);

  Serial.println("RFID ready.");
  delay(2000); // allow Python to connect (Arduino resets on serial open)
}

void loop() {
  // Wait for a card
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // ---- VISUAL FEEDBACK ----
  digitalWrite(BUILTIN_LED, HIGH);
  digitalWrite(EXT_LED_PIN, HIGH);

  // ---- PRINT UID ----
  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(" ");
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  // ---- REQUEST PC TIME ----
  Serial.println(" | REQUEST_TIME");

  // ---- RECEIVE PC TIME (max 3s) ----
  String pcTime = "";
  unsigned long start = millis();

  while (millis() - start < 3000) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') break;
      pcTime += c;
    }
  }

  if (pcTime.length() == 0) {
    pcTime = "TIME NOT RECEIVED";
  }

  Serial.print("Card scanned at: ");
  Serial.println(pcTime);

  // ---- TURN OFF LEDS ----
  delay(300);
  digitalWrite(BUILTIN_LED, LOW);
  digitalWrite(EXT_LED_PIN, LOW);

  // Stop RFID communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(500); // prevent double scans
}
