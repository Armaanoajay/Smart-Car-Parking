#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

byte readCard[4];
String MasterTags[] = {"2AF0FAB0", "1BF78744", "55DE2C5C"}; // Add new tag IDs here
String MasterNames[] = {"Master", "Akshaj", "Armaano"}; // Corresponding names for master tags

String tagID = "";
String activeMasterTag = ""; // Track the currently active master tag

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

int red = 2;
int green = 3;
int ir = 7;
int buzzer = 6;

unsigned long firstMasterScanTime = 0; // Stores time of the first master tag scan
bool isFirstScan = true; // Flag to indicate if it's the first or second scan

void setup() {
  Serial.begin(9600);
  SPI.begin();
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ir, INPUT);

  mfrc522.PCD_Init();
  delay(4);
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  mfrc522.PCD_DumpVersionToSerial();

  Serial.println("--------------------------");
  Serial.println("Access Control");
  Serial.println("Scan Your Card>>");

  myServo.attach(8);
}

void loop() {
  unsigned int AnalogValue;

  AnalogValue = analogRead(A0);
  if (AnalogValue > 500) {
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(7, HIGH);
  }
  else {
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    digitalWrite(7, LOW);
  }

  if (getID()) {
    bool isMaster = false;
    for (int i = 0; i < sizeof(MasterTags) / sizeof(MasterTags[0]); i++) {
      if (tagID == MasterTags[i]) {
        isMaster = true;
        break;
      }
    }

    if (isMaster) {
      if (tagID == activeMasterTag || activeMasterTag == "") {
        activeMasterTag = tagID; // Set the current active master tag

        // Find the index of the scanned master tag in the MasterTags array
        int index = 0;
        for (int i = 0; i < sizeof(MasterTags) / sizeof(MasterTags[0]); i++) {
          if (tagID == MasterTags[i]) {
            index = i;
            break;
          }
        }

        Serial.print("Master Tag Scanned- ");
        Serial.print(MasterNames[index]); // Print the name corresponding to the scanned tag

        digitalWrite(red, LOW);
        digitalWrite(green, HIGH);
        if (isFirstScan) {
          // First scan of the master tag - open servo
          myServo.write(180);
          delay(2000);
          myServo.write(90);
          firstMasterScanTime = millis();
          isFirstScan = false;
        } else {
          // Second scan of the master tag - calculate elapsed time in seconds
          unsigned long currentScanTime = millis();
          unsigned long elapsedTime = (currentScanTime - firstMasterScanTime) / 1000;
          Serial.print("   Elapsed Time: ");
          Serial.print(elapsedTime);
          Serial.println(" seconds");
          myServo.write(180);
          delay(2000);
          myServo.write(90);
          activeMasterTag = "";

          // Reset flags for next cycle
          isFirstScan = true;
          firstMasterScanTime = 0;
        }
        digitalWrite(red, HIGH);
        digitalWrite(green, LOW);
      } else {
        Serial.println("Access Denied due to No free space!");
        Serial.println("--------------------------");
        digitalWrite(red, HIGH);
        delay(100);
        digitalWrite(red, LOW);
        delay(100);
        digitalWrite(red, HIGH);
        delay(100);
        digitalWrite(red, LOW);
        delay(100);
        digitalWrite(red, HIGH);
        delay(100);
        digitalWrite(red, LOW);
        delay(100);
        digitalWrite(red, HIGH);
        delay(100);
        digitalWrite(red, LOW);
        delay(100);
        digitalWrite(red, HIGH);
        digitalWrite(green, LOW);
      }
    } else {
      Serial.println("Access Denied!");
      Serial.println("--------------------------");
      tone(buzzer, 800);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
      delay(100);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
      delay(100);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
      delay(100);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
      delay(100);
      noTone(buzzer);
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    }

    delay(2000);
    Serial.println("Access Control");
    Serial.println("Scan Your Card>>");
  }
}

boolean getID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  tagID = "";

  for (uint8_t i = 0; i < 4; i++) {
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(readCard[i], HEX));
  }

  tagID.toUpperCase();

  mfrc522.PICC_HaltA();

  return true;
}
