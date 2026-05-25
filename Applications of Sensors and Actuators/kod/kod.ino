//==================================================================BIBLIOTEKE=================================================
#include <Adafruit_BME280.h>
#include <RTClib.h>
Adafruit_BME280 bme;
RTC_DS1307 rtc;
//DC motor
#define in2Pin 11
#define in1Pin 10
#define enablePin 9
//servo
#include <Servo.h>
Servo Servo_mg995;
//OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//===================================================================PROMENLJIVE================================================
int stanje = 0;
//Senzor nivoa osvetljenosti (TEMP6000)
int temt6000 = A1;
float light;
int light_value;
//Pinovi za driver za Stepper motor
const int dirPin = 6;
const int stepPin = 7;
//Promenljiva koja određuje dužinu trajanja kretanja stepper motora
//(stepsPerRevolution/200 koraka)=br_revolucija
const int stepsPerRevolution = 5000;
//Senzor temp i pritiska (BME280)
int temperatura, pritisak, vlaznost_vazduha;
//Senzor kvaliteta vazduha
int analogni_kvalitet_vazduha;
//Senor vlaznosti zemlje
const int sensor_pin = A0;
int vlaznost_zemlje;
//led traka
int led = 8;
//Buzzer
int buzz = 12;
//kuler
int INA = 3;
int INB = 4;
//rtc sat
int brojacDana=0;
int sat, minut, sekunda;
char dan;
// UNCHANGABLE PARAMATERS
#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

// event on Monday, from 13:50 to 14:10
uint8_t WEEKLY_EVENT_DAY      = FRIDAY;
uint8_t WEEKLY_EVENT_START_HH = 12; // event start time: hour
uint8_t WEEKLY_EVENT_START_MM = 20; // event start time: minute
uint8_t WEEKLY_EVENT_START_ss = 00; // event start time: minute
//====================================================================FUNKCIJE=================================================
const char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};
void Ocitavanje_senzora() {
  light_value = analogRead(temt6000);
  light = light_value * 0.0976;
  Serial.println(light);
  temperatura = bme.readTemperature();
  pritisak = bme.readPressure();
  vlaznost_vazduha = bme.readHumidity();
  analogni_kvalitet_vazduha = analogRead(A7);

  int sensor_analog;
  sensor_analog = analogRead(sensor_pin);
  vlaznost_zemlje = ( 100 - ( (sensor_analog / 1023.00) * 100 ) );
  DateTime now = rtc.now();
  now = rtc.now();
  dan = now.dayOfTheWeek();
  sat = now.hour();
  minut = now.minute();
  sekunda = now.second();
  if ((dan == WEEKLY_EVENT_DAY)&&
      (sat == WEEKLY_EVENT_START_HH) &&
      (minut == WEEKLY_EVENT_START_MM) &&
      (sekunda == WEEKLY_EVENT_START_ss) )   {
      
      delay(1000);
      brojacDana++;
      delay(5000);
      brojacDana++;
  }
}

void Servo_otvori() {
  Servo_mg995.write(135);
  delay(580);
  Servo_mg995.write(90);
}
void Servo_zatvori() {
  Servo_mg995.write(45);
  delay(580);
  Servo_mg995.write(90);
  

}
void OLED_ispis() {
  DateTime now = rtc.now();
  display.clearDisplay();
  display.setTextColor(WHITE);
  //display.setFont(&FreeMonoBold9pt7b);
  display.setTextSize(2);
  display.setCursor(13, 27);
  display.print("TEMP: ");
  display.setTextSize(2);
  display.setCursor(73, 27);
  display.print(temperatura);
  display.drawCircle(100, 27, 2, WHITE);
  display.setCursor(105, 27);
  display.println("C");
  display.display();
  delay(2500);
//////////zemlja///////////////
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.println("Vlaznost zemlje: ");
  display.setTextSize(2);
  display.setCursor(52, 40);
  display.print(vlaznost_zemlje);
  display.println("%");
  display.display();
  delay(2500);
  ////////////vazduh//////////////
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(17, 20);
  display.println("Vlaznost vazduha: ");
  display.setTextSize(2);
  display.setCursor(48, 40);
  display.print(vlaznost_vazduha);
  display.println("%");
  display.display();
  delay(2500);
  /////kvalitet vazduha
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(18, 20);
  display.println("Kvalitet vazduha:");
  display.setTextSize(2);
  display.setCursor(35, 40);
  if(analogni_kvalitet_vazduha < 100){
    display.print("DOBAR");
  }else if(analogni_kvalitet_vazduha < 150 && analogni_kvalitet_vazduha > 100){
    display.setCursor(25, 40);
    display.print("SREDNJI");
  }else if(analogni_kvalitet_vazduha > 150){
    display.setCursor(40, 40);
    display.print("LOS");
  }
  display.display();
  delay(2500);
  //////////////vrijeme//////////////
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(35, 15);
  display.print(sat);
  display.print(":");
  display.setTextSize(2);
  display.setCursor(70, 15);
  if(minut < 10){
    
    display.print("0");
    }
  display.print(minut);
  /////////////datum///////////////
  display.setTextSize(2);
  display.setCursor(10, 40);
  display.print(now.day(), DEC);
  display.print(".");
  display.setTextSize(2);
  //display.setCursor(40, 40);
  display.print(now.month(), DEC);
  display.print(".");
  display.setTextSize(2);
  //display.setCursor(70, 40);
  display.print(now.year(), DEC);
  display.print(".");
  display.display();
  delay(2500);
}
//====================================================================SETUP====================================================

void setup() {
  if (!bme.begin(0x76)) {
    Serial.println(F("Senzor nije pronadjen"));
  }
  if(! rtc.begin()){
    Serial.println(F("Error: Sat nije pronađen!"));
    while(1);
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED error!"));
  }

  Serial.begin(9600);
  //Senzor nivoa osvetljenosti (TEMP6000)
  pinMode(temt6000, INPUT);
  //Senzor kvaliteta vazduha
  pinMode(A7, INPUT);
  //DC motor
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  //led
  analogWrite(led, 0);
  //Buzzer
  pinMode(buzz, OUTPUT);
  digitalWrite(buzz, LOW);
  //kuler
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  digitalWrite(INB, LOW);
  digitalWrite(INB, LOW);
  //servo
  Servo_mg995.attach(2);
  //Stepper
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(dirPin, HIGH);
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Odkomentarisati ovu liniju ukoliko je potrebno promeniti vreme pa je potrebno ponovo zakomentarisati i upload-ovati kod
}
//====================================================================LOOP=====================================================

void loop() {
  Ocitavanje_senzora();
  OLED_ispis();

  switch (stanje) {
    case 0:
      if (light > 0/*0.3*/) {
        analogWrite(led, 255);
        Serial.println("Ovde sam");
      } //else if (light > 1.2) {
        //analogWrite(led, 0);
        //Serial.println(light);
      //}
        if (temperatura > 28 /*|| analogni_kvalitet_vazduha > 150*/) {
       // Serial.println(analogni_kvalitet_vazduha);
        //Serial.println(temperatura);
        stanje = 1;
      }
      if(vlaznost_zemlje < 5){
        Serial.println(vlaznost_zemlje);
        stanje = 2;
      }
      if(brojacDana==2){
        brojacDana=0;
        stanje=3;
      }
      break;
      
    case 1:
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      Servo_otvori();
      delay(1000);
      while (temperatura > 26) {
        digitalWrite(INB, HIGH);
        digitalWrite(INA, LOW);
        temperatura = bme.readTemperature();
      }
      digitalWrite(INB, LOW);
      digitalWrite(INA, LOW);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(100);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(100);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(100);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(100);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      Servo_zatvori();
      stanje = 0;
      break;
      
    case 2:
    digitalWrite(buzz, HIGH);
    delay(100);
    digitalWrite(buzz, LOW);
     while (vlaznost_zemlje > 3) {
      analogWrite(enablePin, 255);
      digitalWrite(in1Pin, HIGH);
      digitalWrite(in2Pin, LOW);
      int sensor_analog = analogRead(sensor_pin);
      vlaznost_zemlje = ( 100 - ( (sensor_analog / 1023.00) * 100 ) );
      }
      analogWrite(enablePin, 0);
      digitalWrite(in1Pin, LOW);
      digitalWrite(in2Pin, LOW);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(100);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      stanje =0;
      break;
      
    case 3:
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      //STEPPER
      // Okretanje u smeru okretanja kazaljke na satu
      digitalWrite(dirPin, HIGH);
      
      // Generisanje impulsnog signala na stepPin-u
      for(int x = 0; x < stepsPerRevolution; x++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(800);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(800);
      }
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      delay(100);
      digitalWrite(buzz, HIGH);
      delay(100);
      digitalWrite(buzz, LOW);
      stanje=0;
      break;

  }
}
