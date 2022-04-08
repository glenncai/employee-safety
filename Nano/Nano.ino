/* Designed for CHUK-IE IERG4230 course
 * Created on 2016.01.20
 * Revised on 2018.01.12, 2021.07.20, 2021.07.29
 * Copyright reserved
 * 
 * Native Hardware Serial (Tools >>> Serial Monitor), 115200bps  for debug
 */

// Some partial code copy from IERG4230 Lab sources and Inrernet
// Edited by YANG Xu, CAI Longhua


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "IERG4230.h"
#include <SoftwareSerial.h>
#include <Arduino.h>

// ==== define program information, it will be shown after reset
#define programMark "IERG4230 simple OS"      //-- You can modify this line. 
#define versionMark 1234                    //-- Any number, You can modify this line. 

// === start #define =============================================================
#define debugPerformance            //Print Performance count, you can comment it 
#define YunI2CAddress  0x18         //Dont modify, YUN I2C address
#define ESP8266I2CAddress 0x66      //Dont modify, ESP8266 I2C address
unsigned int performanceCounter;    //Dont modify, for performance test
osEvent task0;                      //Dont modify, for performance test
// ===== end of #include, #define, essential object and golbal variables  ===========
// ===== create user defined objects
osEvent task1, task2, task3, task4, task5, task6, task7, task8, task9, task10; //10 tasks object are created.
dOut ledL(13);                       //create objects, LED(L)
dOut led1(3), led2(5), led3(6);      //create objects, three on project board LEDs as digital outputs
dIn sw11(2), sw12(7);                //create objects, two on project board switches as digital inputs

#define TRIGGER1 A1 // Nano analog output A1
#define ECHO1    A0 // Nano analog input A0
#define TRIGGER2 A3 // Nano analog output A3
#define ECHO2    A2 // Nano analog input A2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long pastTime, nowTime;
int buzzer= 4; // (Buzzer pin) use nano ND4, use esp8266 GPIO 14
boolean trafficMode = 0; // 0 manual, 1 auto
float duration1, distance1, duration2, distance2, carSpeed;


// == add your golbal variables below ==
//int temp_i;    //-- uncomment this line for Exp-1.9
////////////////////////////////////////////////////////////////
void setup()
{
  Wire.begin(); //start I2C bus
  Wire.setClock(50); //-- slow down I2C speed

  Serial.begin(115200);   //--enable hwUART for debug
//  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
//    Serial.println(F("SSD1306 allocation failed"));
//    for (;;);
//  }
  printProgramMark();
  osEvent::osTimer = millis();
  
  //=== dont modify above setup
  //=== user can start task at starting
  //OLED display 1st line
//  display.clearDisplay();
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 10);
//  display.println("IERG4230 HC-SR04 Module"); // Display static text
//  display.display();
  
  pinMode(TRIGGER1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIGGER2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(buzzer, OUTPUT);
  pastTime = 0;
  nowTime = 0;
  task0.timerSet(100);  //the first time to exectue performance test after 100ms
  task1.timerSet(100);  //the first time to exectue task1 after 100ms
  task2.timerSet(100);  //the first time to exectue task2 after 100ms
  task10.timerSet(100);  //the first time to exectue task10 after 100ms
}

void loop()
{
  while (Serial.available())
  {
    //read data from hardware buffer
  }
  performanceCounter++;   //dont remove, for performace test
  hal_eventUpdate();               //check Hardware events
  if (osEvent::osTimer != millis()) timeStampUpdate();
  if (task0.isSet()) task0_handler();  //for performance test
  
  //---- dont modify above lines, user add own task handlers below ---
  if (task1.isSet()) task1_handler(); //  when task-1 timeout, run task1_handler() subroutine
  if (task2.isSet()) task2_handler();
  if (sw11.eventDown) sw11_downHandler();  //-- for EXp-2.5
  if (sw12.eventDown) sw12_downHandler();  //-- for EXp-2.5
//    if (sw11.eventUp) sw11_upHandler();
//    if (task3.isSet()) task3_handler();
//    if (task4.isSet()) task4_handler();
  if (task10.isSet()) task10_handler(); //  when task-10 timeout, run task1_handler() subroutine
} 
//----------------------------------------
//--- user's add handlers and subroutines below here

void sw11_downHandler() //-- for Exp-2.5
{
  led3.on();

  // Send data to ESP8266
  if(trafficMode) // change auto
  {
    Wire.beginTransmission(ESP8266I2CAddress);
    Wire.write("m");
    Wire.endTransmission();
    Serial.print("trafficMode:");
    Serial.println(trafficMode);
    trafficMode = !trafficMode;
    
  } else {
    Wire.beginTransmission(ESP8266I2CAddress);
    Wire.write("a");
    Wire.endTransmission();
    Serial.print("trafficMode:");
    Serial.println(trafficMode);
    trafficMode = !trafficMode;
    
  }
  delay(100);
  led3.off();
  sw11.eventDown=0;   // reset sw11 press down event 
}

void  sw12_downHandler()  //-- for Exp-24..5
{
  led3.toggle();   //-- uncomment for debug
  Wire.requestFrom(ESP8266I2CAddress, 2);  //--Wire.requestFrom(address, datasize)
  if (Wire.available()) 
  {
    Serial.print("Data returned: ");
    while (Wire.available()) Serial.print((char) Wire.read());
    Serial.println();
    }
    Wire.endTransmission() ;
    sw12.eventDown=0;   // reset sw11 press down event 
}


void task1_handler(void)
{
  task1.clean(); //-- clear the planning flag
  detectSpeed();  //detect distance and calculate speed
  task1.timerSet(1000); //-- set timer (1000ms), planning flag will be set when it timeout.
}

void task2_handler(void)
{
  task2.clean(); //-- clear the planning flag
  transmitDistance();  //transmit distance data
  task2.timerSet(200); //-- set timer (200ms), planning flag will be set when it timeout.
}

void task10_handler(void) //--LED(L) on Nano
{
  task10.clean(); //-- clear the planning flag
  ledL.toggle(); //-- toggle the LED(L) 
  task10.timerSet(100); //-- set timer (1000ms), planning flag will be set when it timeout.
}

void transmitDistance()
{
  led3.on();
  String line =  "SPEED ACCEPTED!" +  String(distance1) +"&" + String(distance2);
  
  Wire.beginTransmission(ESP8266I2CAddress);
  Wire.write(line.c_str());
  Wire.endTransmission();
  
  Serial.println("transmit Data");
  Serial.print("Distance1: ");
  Serial.println(distance1);
  Serial.print("Distance2: ");
  Serial.print(distance2);
  
  delay(100);
  led3.off();
}

void detectSpeed()
{
  
  digitalWrite(TRIGGER1, LOW); 
  delayMicroseconds(2);
  digitalWrite(TRIGGER1, HIGH);
  delayMicroseconds(10); // Create a 10us pulse
  digitalWrite(TRIGGER1, LOW);
//  display.clearDisplay();

  duration1 = pulseIn(ECHO1, HIGH, 23520); // Returns the length of the pulse in microseconds
  
  if (duration1 == 0)
  {
    distance1 = -1;
//    Serial.print("Range exceed!  time: ");
//    Serial.println(pastTime);
    //OLED diplay 1rd line
//    display.setCursor(0, 0);
//    display.println("Distance1:Range exceed");      // Display static text
  }
  else
  {
    distance1 = (duration1 / 2) / 29.1; //ideal 200
    
    if (distance1<30) //threshold 30
    {
      led1.on();
      pastTime = millis(); // get the time through first sensor
    }
    else
      led1.off();
    
//    Serial.print("Distance1: ");
//    Serial.print(distance1);
//    Serial.print(" CM    time: ");
//    Serial.println(pastTime);
    //OLED diplay 1rd line
//    display.setCursor(0, 0);
//    display.println("Distance1:       cm");      // Display static text
//    display.setCursor(62, 0); // one line can own 21 chars
//    display.print(distance1);
  }


  digitalWrite(TRIGGER2, LOW); 
  delayMicroseconds(2);
  digitalWrite(TRIGGER2, HIGH);
  delayMicroseconds(10); // Create a 10us pulse
  digitalWrite(TRIGGER2, LOW);

  duration2 = pulseIn(ECHO2, HIGH, 23520);
  
  if (duration2 == 0) 
  {
    distance2 = -1;
//    Serial.print("Range exceed!    time: ");
//    Serial.println(nowTime);
    //OLED diplay 3rd line
//    display.setCursor(0, 20);
//    display.println("Distance2:Range exceed");      // Display static text
  }
  else
  {
    distance2 = (duration2 / 2) / 29.1;

    if (distance2<30)
    {
      led2.on();
      nowTime = millis(); // get the time through second sensor
    }
    else
      led2.off();
    
    
//    Serial.print("Distance2: ");
//    Serial.print(distance2);
//    Serial.print(" CM    time: ");
//    Serial.println(nowTime);
    //OLED diplay 3rd line
//    display.setCursor(0, 20);
//    display.println("Distance2:       cm");      // Display static text
//    display.setCursor(62, 20);
//    display.println(distance2);
  }


  if(pastTime>0 && nowTime>0)
  {
    carSpeed = 200 / (nowTime - pastTime) * 1000 *3.6; // real distance between distance1 (sensor1) and distance2 (sensor2) is 200m
    
    if (nowTime < pastTime) {
      Serial.println("Wrong direction");
//      display.println("Wrong direction");
      nowTime = 0;
      pastTime = 0;
    } else {
      Serial.print("Car speed: ");
      Serial.println(carSpeed);
      //OLED diplay 5rd line
//      display.setCursor(0, 40);
//      display.print("Speed:");      // Display static text
//      display.print(carSpeed);
//      display.println("km/h");

      // when car speed is larger than 10km/h, buzzer ring, transmit car speed
      if (carSpeed > 10) {
        alert();
        transmitSpeed();  
      }
      
      nowTime = 0;
      pastTime = 0;
    }
  }
  else
  {
//    Serial.println("Cannot detect speed");
//    display.setCursor(0, 40);
//    display.println("Cannot detect speed");      // Display static text
  }

//  display.display();

}
void alert()
{
  tone(buzzer, 450);
  delay(500);
  noTone(buzzer);
}

void transmitSpeed()
{
  led3.on();
  String line =  "SPEED ACCEPTED!" +  String(distance1) +"&" + String(distance2) + "&" + String(carSpeed);
  
  Wire.beginTransmission(ESP8266I2CAddress);
  Wire.write(line.c_str());
  Wire.endTransmission();
  
  Serial.println("Send Data");
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.print(" CM    pasttime: ");
  Serial.println(pastTime);
  Serial.print("Distance2: ");
  Serial.print(distance2);
  Serial.print(" CM    nowtime: ");
  Serial.println(nowTime);
  
  delay(200);
  led3.off();
}

//-------------------  basic OS subroutines -------------------
//---You may need to modify them if you change the hardware ---
void hal_eventUpdate(void)
{
  //== comment/uncomment the followings for used hardware
  sw11.dataUpdate();
  sw12.dataUpdate();
  //aI0.dataUpdate();   //-- comment this line will get higher performance, Analog In takes time
  //aI1.dataUpdate();   //-- comment this line will get higher performance, Analog In takes time
  //aI2.dataUpdate();   //-- comment this line will get higher performance, Analog In takes time
  //aI3.dataUpdate(); //-- comment this line will get higher performance, Analog In takes time
  //aI6.dataUpdate(); //-- comment this line will get higher performance, Analog In takes time
  //aI7.dataUpdate();     //--uncomment this line for Exp1.6
}

void timeStampUpdate(void)   // no need to modify this function unless you know what you are doing.
{
  int i;
  unsigned long temp;
  temp = millis();
  if (osEvent::osTimer > temp) i = 1;
  else i = (int)(temp - osEvent::osTimer);
  osEvent::osTimer = temp;
  //---- user add their own tasks if necessary
  task0.timerUpdate(i);
  task1.timerUpdate(i);
  task2.timerUpdate(i);
  task3.timerUpdate(i);
  task4.timerUpdate(i);
  task5.timerUpdate(i);
  task6.timerUpdate(i);
  task7.timerUpdate(i);
  task8.timerUpdate(i);
  task9.timerUpdate(i);
  task10.timerUpdate(i);  
}

void task0_handler() // Dont modify, for performace test
{ 
  task0.clean();
  #ifdef debugPerformance
//    Serial.print("Performance: ");
//    Serial.println(performanceCounter/3);
  #endif
  performanceCounter = 0;
  task0.timerSet(3000);
}

void printProgramMark()   //-- Serial print Program Information for identification
{
  Serial.println("");
  Serial.print("Program: ");
  Serial.println(programMark);
  Serial.print("Version: ");
  Serial.println(versionMark);  
}
//====  End of OS ========================
