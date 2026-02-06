#include <RTClib.h>
#include <Wire.h>
#include <U8glib.h>

RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

const int buzzer = 8;
const int led1 = 11;
const int led2 = 12;
const int ledPins[4] = { 3, 5, 7, 10 };
const int buttonPins[4] = { 2, 4, 6, 9 };

int alarmHour = 13;
int alarmMinute = 59;

bool alarmActive = false;
bool alarmTrigger = false;
bool gameactive = false;

const int sequenceLength = 5;
int sequence[sequenceLength];
int userIndex = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  u8g.setFont(u8g_font_helvB10);

  randomSeed(analogRead(A0));

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(buzzer, OUTPUT);
  noTone(buzzer);

  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
}

void loop() {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  int s = now.second();


  oledDraw(getTime(), getDate(), alarmHour, alarmMinute);

  if (h == alarmHour && m == alarmMinute && !alarmTrigger) {
    alarmActive = true;
    gameactive = true;
    alarmTrigger = true;

    if (alarmActive) {
      tone(buzzer, 100);
    }

    startNewGame();
  }

  if (m != alarmMinute) {
    alarmTrigger = false;
  }

  

  if (!alarmActive) {
    noTone(buzzer);
  }



  if (gameactive) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        flashLed(i);

        if (i == sequence[userIndex]) {
          userIndex++;
          if (userIndex == sequenceLength) {
            winEffect();
            gameactive = false;
            alarmActive = false;
          }
        } else {
          loseEffect();
          startNewGame();
        }

        while (digitalRead(buttonPins[i]) == LOW) {}
      }
    }
  }
}

String getTime() {
  DateTime now = rtc.now();
  return twoDigit(now.hour()) + ":" + twoDigit(now.minute()) + ":" + twoDigit(now.second());
}

String getDate() {
  DateTime now = rtc.now();
  return String(now.year()) + "-" + twoDigit(now.month()) + "-" + twoDigit(now.day());
}

String twoDigit(int n) {
  if (n < 10) return "0" + String(n);
  return String(n);
}

void oledDraw(String time, String date, int ah, int am) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 15, ("Time: " + time).c_str());
    u8g.drawStr(0, 35, ("Date: " + date).c_str());
    u8g.drawStr(0, 55, ("Alarm: " + twoDigit(ah) + ":" + twoDigit(am)).c_str());
  } while (u8g.nextPage());
}

void startNewGame() {
  userIndex = 0;
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(0, 4);
  }
  delay(400);
  showSequence();
}

void showSequence() {
  for (int i = 0; i < sequenceLength; i++) {
    digitalWrite(ledPins[sequence[i]], HIGH);
    delay(350);
    digitalWrite(ledPins[sequence[i]], LOW);
    delay(200);
  }
}

void flashLed(int i) {
  digitalWrite(ledPins[i], HIGH);
  delay(150);
  digitalWrite(ledPins[i], LOW);
}

void loseEffect() {
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], HIGH);
    delay(200);
    for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], LOW);
    delay(200);
  }
}

void winEffect() {
  for (int w = 0; w < 10; w++) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(20);
      digitalWrite(ledPins[i], LOW);
    }
    for (int i = 3; i >= 0; i--) {
      digitalWrite(ledPins[i], HIGH);
      delay(20);
      digitalWrite(ledPins[i], LOW);
    }
  }
}
