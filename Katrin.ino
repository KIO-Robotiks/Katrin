#include <Wire.h>
#include <AHT10.h> // подключаем библиотеку Датчика температуры и влажности.
#include <LiquidCrystal_I2C.h> // подключаем библиотеку Дисплея.
#include <iarduino_RTC.h> // подключаем библиотеку для работы с часами.
#include <pitches.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // объявляем переменную lcd c параметрами дисплея.

iarduino_RTC time(RTC_DS3231); // объявляем переменную time для работы с библиотекой, указывая название модуля RTC_DS3231.

AHT10Class AHT10; // это нужно для работы Датчика температуры и влажности.

float T;
float H;
float D;
String UpTime = "06:30:00";
String DownTime = "21:30:00";

int ALARM = 2; // датчик движения  
int SOUND = 7; // звук
int BUT1 = 3; // кнопка 1
int BUT2 = 5; // кнопка 2

unsigned long tmr1 = millis(); // таймер опроса датчика TH
unsigned long tmr2 = millis(); // таймер проверки движения
unsigned long tmr3 = millis(); // длительность подсветки дисплея
unsigned long tmr4 = millis(); // таймер кнопки 1
unsigned long tmr5 = millis(); // таймер кнопки 2

void(* resetFunc) (void) = 0; // функция перезагрузки.

// мелодия Portal2
int Portal2[] = {E3,2,A3,2,B3,2,CS4,3,D4,1,CS4,4,0,2,A3,2,A3,2,E3,2,B3,2,A3,2,GS3,2,A3,2,0,3,E3,1,A3,2,B3,2,CS4,2,D4,2,CS4,2,B3,2,A3,4,B3,2,GS3,6,0,6,E3,2,A3,2,B3,2,CS4,3,D4,1,CS4,4,0,2,A3,2,A3,2,E3,2,B3,2,A3,2,GS3,2,A3,2,0,2,E3,2,A3,2,B3,2,CS4,2,D4,2,CS4,2,B3,2,A3,4,FS4,4,F4,2,CS4,2,B3,2,AS3,6,FS3,4,AS3,2,B3,2,CS4,2,CS4,4,AS4,4,GS4,4,FS4,10,FS3,4,B3,2,AS3,2,FS3,4,B3,2,AS3,2,0,2,FS3,10};


void setup()
{
  time.begin(); // инициируем RTC модуль
  lcd.init(); // проводим инициализацию дисплея
  lcd.backlight(); // включаем подсветку
  //Serial.begin(9600); // запуск порта для отладки
  
  Wire.begin();
  AHT10.begin(0x38);

  pinMode(ALARM, INPUT); // датчик движения  
  pinMode(SOUND, OUTPUT); // звук
  pinMode(BUT1, INPUT_PULLUP); // кнопка 1
  pinMode(BUT2, INPUT_PULLUP); // кнопка 2

  loading(); // приветствие
  readTH(0); // читаем данные Т и Вл
}


void loop()
{
  lcd.setCursor(0, 0);
  lcd.print(String(T,1) + "C  " + String(time.gettime("D H:i")));
  lcd.setCursor(0, 1);
  lcd.print(String(H,1) + "%  " + "DP  "+String(D,1)+'C');
  //----------------------------------------------------------------------
  readTH(5000);
  alarm();
  press1();
  press2();   
}


void readTH(int x){
  if (millis() - tmr1 >= x){
      T = AHT10.GetTemperature();                
      H = AHT10.GetHumidity();
      D = AHT10.GetDewPoint();
      tmr1 = millis();
  }  
}


void alarm(){
  if (millis() - tmr2 >= 500){
    if (digitalRead(ALARM) == HIGH){
      lcd.backlight();
      tmr3 = millis();
    }
    else{
      display_time();
    }
    tmr2 = millis();
  }
}


void display_time(){
  if (millis() - tmr3 >= 10000){
    if (digitalRead(ALARM) == LOW){
      lcd.noBacklight();
    }
    tmr3 = millis();    
  }
}


void loading(){
  delay(3000);
  String l = ".";
  lcd.setCursor(0, 0);
  lcd.print(" Katrin loading");
  delay(1000);
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(0, 1);
    lcd.print(l);
    l = l+".";
    delay(200);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    (o)_(o)");
  delay(2000);  
  lcd.setCursor(0, 1);
  lcd.print("  Hello World!");
  delay(2000);
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("    (o)_(o)");
  delay(2000);
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("################");
  lcd.setCursor(0, 1);
  lcd.print("################");
  delay(2000);
  lcd.clear();
}


void press1()
{
  if (!digitalRead(BUT1) && millis() - tmr4 > 250) {
    Serial.println("press 1");
    music();    
    tmr4 = millis();   
  } 
}

void press2()
{
  if (!digitalRead(BUT2) && millis() - tmr5 > 250) {
    resetFunc();
    tmr5 = millis();
  }  
}


void music()
{
  int sizeMusic = sizeof(Portal2)/sizeof(int);
  for (int thisNote = 0; thisNote < sizeMusic; thisNote+=2) {
  //e.g. четверть ноты= 1000 / 4, восьмая часть = 1000/8, etc.
  int noteDuration = 700/Portal2[thisNote+1];
  tone (SOUND, Portal2[thisNote],noteDuration);
  delay(200);        
  // noTone (8);
  }
}

