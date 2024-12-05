#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10 
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); .

byte card_ID[4];                 
byte Name1[4] = {0x70, 0x70, 0xDD, 0x58}; 
byte Name2[4] = {0xF3, 0x29, 0x53, 0xFE}; 

unsigned long entryTime[2] = {0, 0}; // 
unsigned long exitTime[2] = {0, 0};  // 
int j = 0; // Card index

int const RedLed = 6;
int const GreenLed = 5;
int const Buzzer = 8;

String Name; 
long Number; 

void setup() {
  Serial.begin(9600);          
  SPI.begin();                
  mfrc522.PCD_Init();         

  Serial.println("CLEARSHEET");                
  Serial.println("LABEL,Date,Time,Name,Number,Status,Duration (Hours:Minutes:Seconds)"); 

  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);
}

void loop() {
  
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return; 
  }

  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];
  }

 
  if (compareUID(card_ID, Name1, mfrc522.uid.size)) {
    Name = "SARAH";
    Number = 123456;
    j = 0;
  } else if (compareUID(card_ID, Name2, mfrc522.uid.size)) {
    Name = "ALI";
    Number = 789101;
    j = 1;
  } else {
    digitalWrite(GreenLed, LOW);
    digitalWrite(RedLed, HIGH);
    delay(1000);
    digitalWrite(RedLed, LOW);
    goto cont;
  }

  // Check entry and exit
  if (entryTime[j] == 0) {
    /
    entryTime[j] = millis();
    Serial.print("DATA,DATE,TIME,");
    Serial.print(Name);
    Serial.print(",");
    Serial.print(Number);
    Serial.print(",");
    Serial.println("Entered"); 
    digitalWrite(GreenLed, HIGH);
    digitalWrite(RedLed, LOW);
    digitalWrite(Buzzer, HIGH);
    delay(30);
    digitalWrite(Buzzer, LOW);
  } else {
    
    exitTime[j] = millis();
    unsigned long durationMillis = exitTime[j] - entryTime[j];
    unsigned long durationSeconds = durationMillis / 1000; 
    unsigned long hours = durationSeconds / 3600;
    unsigned long minutes = (durationSeconds % 3600) / 60;
    unsigned long seconds = durationSeconds % 60;

    Serial.print("DATA,DATE,TIME,");
    Serial.print(Name);
    Serial.print(",");
    Serial.print(Number);
    Serial.print(",");
    Serial.print("Exited,");
    Serial.print(hours);
    Serial.print(" hours ");
    Serial.print(minutes);
    Serial.print(" minutes ");
    Serial.print(seconds);
    Serial.println(" seconds"); 
    entryTime[j] = 0; 
    exitTime[j] = 0;  
  }

cont:
  delay(2000);
  digitalWrite(GreenLed, LOW);
  digitalWrite(RedLed, LOW);
}


bool compareUID(byte *card, byte *storedUID, byte size) {
  for (byte i = 0; i < size; i++) {
    if (card[i] != storedUID[i]) {
      return false;
    }
  }
  return true;
}
