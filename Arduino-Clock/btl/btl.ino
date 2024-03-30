#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Display  I2C 20 x 4
RTC_DS1307 RTC;

int P1 = 6;  // Button SET MENU'
int P2 = 7;  // Button +
int P3 = 8;  // Button -
int P4 = 9;  // SWITCH Alarm

#define LED 13
#define buzzer 10

int menu = 0;
int stateOn = 0;
int setOn = 0;
int setAll = 0;

int gio = 0;
int phut = 0;
int giay = 0;
bool RUN = true;
uint8_t alarmHours = 0, alarmMinutes = 0, eHour = 0, eMin = 0, oHour = 0, oMin = 0;  // Holds the current alarm time

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(P1, INPUT_PULLUP);  // https://www.arduino.cc/en/Tutorial/InputPullupSerial
  pinMode(P2, INPUT_PULLUP);
  pinMode(P3, INPUT_PULLUP);
  pinMode(P4, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.setCursor(0, 3);
  lcd.print("Alarm: Off  ");

  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  if (!RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  int menu = 0;
}

void loop() {

  if (digitalRead(P1) == LOW) {
    menu = menu + 1;
  }

  if (menu == 0) {
    DisplayDateTime();
    Alarm();
  }
  if (menu == 1) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("2: Set Alarm");

    if (digitalRead(P2) == LOW) {
      setAlarm();
      menu = 0;
    }
  }
  if (menu == 2) {
    // lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("2: Set time");

    if (digitalRead(P2) == LOW) {
      countDown();
      menu=3;
    }
    delay(2000);
    lcd.clear();
    // count();
  }
  if (menu == 3) {
    count();

    if(digitalRead(P1) == LOW){
      menu=0;
    }
  }
  if (menu == 4) {
  }
  if (menu == 5) {
  }
  if (menu == 6) {
  }
  delay(100);
}

void setAlarm() {
  DisplaySetHourAll();
  DisplaySetMinuteAll();
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("ALARM");
  lcd.setCursor(5, 1);
  lcd.print(alarmHours, DEC);
  lcd.print(":");
  lcd.print(alarmMinutes, DEC);
  delay(1000);
  lcd.clear();
}

void DisplayDateTime() {
  DateTime now = RTC.now();

  lcd.setCursor(0, 2);
  lcd.print("Hour : ");

  if (now.hour() <= 9) {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  if (now.minute() <= 9) {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  if (now.second() <= 9) {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);

  lcd.setCursor(0, 1);
  lcd.print("Date : ");
  if (now.day() <= 9) {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  lcd.print("/");
  if (now.month() <= 9) {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.year(), DEC);

  char DOW[][10] = { "Sunday   ", "Monday   ", "Tuesday  ", "Wednesday", "Thursday ", "Friday   ", "Saturday " };
  lcd.setCursor(0, 0);
  lcd.print("Day  : ");
  lcd.print(DOW[now.dayOfTheWeek()]);
}

void DisplaySetHourAll() {
  while (digitalRead(P4) == HIGH) {

    lcd.clear();

    if (digitalRead(P2) == LOW) {
      if (alarmHours == 23) {
        alarmHours = 0;
      } else {
        alarmHours = alarmHours + 1;
      }
    }
    if (digitalRead(P3) == LOW) {
      if (alarmHours == 0) {
        alarmHours = 23;
      } else {
        alarmHours = alarmHours - 1;
      }
    }
    EEPROM.write(0, alarmHours);
    lcd.setCursor(0, 0);
    lcd.print("Set HOUR Alarm:");
    lcd.setCursor(0, 1);
    lcd.print(alarmHours, DEC);
    delay(200);
  }
  delay(200);
}

void DisplaySetMinuteAll()  // Setting the alarm minutes
{
  while (digitalRead(P4) == HIGH) {

    lcd.clear();
    if (digitalRead(P2) == LOW) {
      if (alarmMinutes == 59) {
        alarmMinutes = 0;
      } else {
        alarmMinutes = alarmMinutes + 1;
      }
    }
    if (digitalRead(P3) == LOW) {
      if (alarmMinutes == 0) {
        alarmMinutes = 59;
      } else {
        alarmMinutes = alarmMinutes - 1;
      }
    }
    EEPROM.write(1, alarmMinutes);
    lcd.setCursor(0, 0);
    lcd.print("Set MIN. Alarm:");
    lcd.setCursor(0, 1);
    lcd.print(alarmMinutes, DEC);
    delay(200);
  }
  delay(200);
}

void printAllOn() {
  lcd.setCursor(0, 3);
  lcd.print("Alarm: ");

  if (alarmHours <= 9) {
    lcd.print("0");
  }
  lcd.print(alarmHours, DEC);

  lcd.print(":");
  if (alarmMinutes <= 9) {
    lcd.print("0");
  }
  lcd.print(alarmMinutes, DEC);
}
void printAllOff() {
  lcd.setCursor(0, 3);
  lcd.print("Alarm: Off  ");
}

void Alarm() {
  if (digitalRead(P4) == LOW) {
    setAll = setAll + 1;
  }
  if (setAll == 0) {
    printAllOff();
    noTone(buzzer);
    digitalWrite(LED, LOW);
  }
  if (setAll == 1) {

    printAllOn();

    eHour = EEPROM.read(0);
    eMin = EEPROM.read(1);

    Serial.print("Alarm: ");
    Serial.print(eHour);
    Serial.print(eMin);
    Serial.println();

    DateTime now = RTC.now();
    oHour = now.hour();
    oMin = now.minute();

    Serial.print("Time: ");
    Serial.print(oHour);
    Serial.print(oMin);
    Serial.println();



    if (now.hour() == eHour && now.minute() == eMin) {
      lcd.noBacklight();
      DateTime now = RTC.now();
      digitalWrite(LED, HIGH);
      tone(buzzer, 880);
      delay(300);
      tone(buzzer, 698);
      lcd.backlight();
    } else {
      noTone(buzzer);
      digitalWrite(LED, LOW);
    }
  }
  if (setAll == 2) {
    setAll = 0;
  }
  delay(200);
}

void countDown() {
  lcd.clear();

  SetHour();
  SetMinute();
  SetSecond();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Count down:");
  lcd.setCursor(0, 1);
  lcd.print(gio, DEC);
  lcd.print(":");
  lcd.print(phut, DEC);
  lcd.print(":");
  lcd.print(giay, DEC);
  delay(1000);
  lcd.clear();
}

void count() {
  // countDown();

  gio = EEPROM.read(2);
  phut = EEPROM.read(3);
  giay = EEPROM.read(4);

  giay = giay - 1;
  delay(1000);
  if (giay == -1) {
    giay = 59;
    phut = phut - 1;
  }
  if (phut == -1) {
    phut = 59;
    gio = gio - 1;
  }
  if (gio == -1) gio = 0;
  lcd.setCursor(0, 1);
  lcd.print("Count down: ");
  if (gio <= 9) {
    lcd.print('0');
  }
  lcd.print(gio);
  lcd.print(':');
  if (phut <= 9) {
    lcd.print('0');
  }
  lcd.print(phut);
  lcd.print(':');
  if (giay <= 9) {
    lcd.print('0');
  }
  lcd.print(giay);
  if (gio == 0 && phut == 0 && giay == 0) {
    digitalWrite(LED, LOW);
    RUN = false;
    for (int i = 0; i < 20; i++) {
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
    }
  }
  EEPROM.write(2, gio);
  EEPROM.write(3, phut);
  EEPROM.write(4, giay);
}

void SetHour() {
  while (digitalRead(P4) == HIGH) {

    lcd.clear();

    if (digitalRead(P2) == LOW) {
      gio = gio + 1;
    }
    if (digitalRead(P3) == LOW) {
      if (gio == 0) {
        gio = 99;
      } else {
        gio = gio - 1;
      }
    }
    EEPROM.write(2, gio);
    lcd.setCursor(0, 0);
    lcd.print("Choose hour:");
    lcd.setCursor(0, 1);
    lcd.print(gio, DEC);
    delay(200);
  }
  delay(200);
}

void SetMinute()  // Setting the alarm minutes
{
  while (digitalRead(P4) == HIGH) {

    lcd.clear();
    if (digitalRead(P2) == LOW) {
      if (phut == 59) {
        phut = 0;
      } else {
        phut = phut + 1;
      }
    }
    if (digitalRead(P3) == LOW) {
      if (phut == 0) {
        phut = 59;
      } else {
        phut = phut - 1;
      }
    }
    EEPROM.write(3, phut);
    lcd.setCursor(0, 0);
    lcd.print("Choose minute: ");
    lcd.setCursor(0, 1);
    lcd.print(phut, DEC);
    delay(200);
  }
  delay(200);
}

void SetSecond() {
  while (digitalRead(P4) == HIGH) {

    lcd.clear();

    if (digitalRead(P2) == LOW) {
      if (giay == 59) {
        giay = 0;
      } else {
        giay = giay + 1;
      }
    }
    if (digitalRead(P3) == LOW) {
      if (giay == 0) {
        giay = 59;
      } else {
        giay = giay - 1;
      }
    }
    EEPROM.write(4, giay);
    lcd.setCursor(0, 0);
    lcd.print("Choose second:");
    lcd.setCursor(0, 1);
    lcd.print(giay, DEC);
    delay(200);
  }
  delay(200);
}
