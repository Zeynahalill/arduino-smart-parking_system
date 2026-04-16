#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int entrySensor = 2;
const int exitSensor  = 3;

Servo barrier;
const int servoPin = 9;

int totalSlots = 5;
int emptySlots = 5;

bool entryActive = false;
bool exitActive  = false;

// sensör debounce için
unsigned long lastEntryTime = 0;
unsigned long lastExitTime  = 0;
const unsigned long debounceDelay = 200; // ms

void setup() {
  pinMode(entrySensor, INPUT);
  pinMode(exitSensor, INPUT);

  barrier.attach(servoPin);
  barrier.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Hosgeldiniz");
  delay(1000);
  lcd.clear();
}

void openBarrier() {
  barrier.write(90);
}

void closeBarrier() {
  barrier.write(0);
}

void loop() {
  int entryState = digitalRead(entrySensor);
  int exitState  = digitalRead(exitSensor);
  unsigned long currentTime = millis();

  // ===== GİRİŞ =====
  if (!entryActive && entryState == LOW && emptySlots > 0 && (currentTime - lastEntryTime > debounceDelay)) {
    openBarrier();
    entryActive = true;
  }

  if (entryActive && entryState == HIGH) {
    emptySlots--;
    closeBarrier();
    entryActive = false;
    lastEntryTime = currentTime;
  }

  // ===== ÇIKIŞ =====
  if (!exitActive && exitState == LOW && emptySlots < totalSlots && (currentTime - lastExitTime > debounceDelay)) {
    openBarrier();
    exitActive = true;
  }

  if (exitActive && exitState == HIGH) {
    emptySlots++;
    closeBarrier();
    exitActive = false;
    lastExitTime = currentTime;
  }

  // ===== LCD =====
  lcd.setCursor(0,0);
  lcd.print("Bos Yer:");

  lcd.setCursor(0,1);
  lcd.print(emptySlots);
  lcd.print(" / ");
  lcd.print(totalSlots);
  lcd.print("   "); // Eski değerleri temizlemek için

  delay(50);
}
