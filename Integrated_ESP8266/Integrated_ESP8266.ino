// Some partial code copy from IERG4230 Lab sources and Inrernet
// Edited by YANG Xu, CAI Longhua

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <NTPClient.h>
#include <WiFiUdp.h>
// #include <ThingSpeak.h> // always include thingspeak header file after other header files and custom macros
#include <Wire.h>
#include "IERG4230_ESP8266.h"


// Set Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP8266 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress1[] = {0x40, 0xF5, 0x20, 0x06, 0x1F, 0x80};

const char* ssid = "G10-AP";
const char* password = "987654321";

String serverName = "glenncai.com";
// HTTP PORT
const int serverPort = 80;

// HTTP
#include <ESP8266HTTPClient.h>


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id;
    float carSpeed;
    int disappearClock;
    float distance1;
    float distance2;
    int trafficMode;
    int esp32_test;
    int readingId;
} struct_message;

// Create a struct_message called test to store variables to be sent
struct_message test;

unsigned long previousMillis = 0;  
unsigned long interval = 1000;  // send readings timer
unsigned int readingId = 0;
int SpeedDisappearClock = 10;  // if this clock is 0, car speed is
float pastCarSpeed = 0;
float nowCarSpeed = 0;

//const long utcOffsetInSeconds = 28800; // Hong Kong Time Zone (8 * 60 * 60)
//// Week Days
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//
//WiFiClient  client;
//
//// Define NTP Client to get time
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);


dOut ledUsb(16, 1); //-- set this pin is activeLow
dOut led4(15);  // receive speed data from master
dOut led5(14); //red/green traffic light, 1 for green, 0 for red
dIn sw53(4);
dIn sw54(5);

#define ESP8266I2CAddress 0x66  //-- ESP8266 I2C Address

boolean trafficMode = 1; // 1 auto, 0 manual
String distance1 = "";
String distance2 = "";
String carSpeed = "";

void setup(void) 
{  
  Serial.begin(115200);
  for(int i=0; i<10; i++)
  {
    ledUsb.toggle();
    delay(100);    
  }
  
  led4.off();
  led5.on();
  Serial.print("ESP8266 I2C Slave address: 0x");  //-- for debug
  Serial.println(ESP8266I2CAddress, HEX);   //-- for debug
  Wire.begin(2,0, ESP8266I2CAddress);  //--Wire.begin(SDA-2, SCL-0, slave_address)  //-- ESP8266 I2C Address  
  Wire.setClock(10);   //-- for Exp-4.5
  Wire.onReceive(I2cRxEvent); //-- for Exp-4.5

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status()== WL_CONNECTED) {
     Serial.println(WiFi.localIP());
  }
}

//--------------------------- LOOP ---------------------------
void loop(void) 
{
//  timeClient.update();
  // print real time
//  Serial.println(daysOfTheWeek[timeClient.getDay()]);
//  Serial.println(timeClient.getFormattedTime());

  unsigned long currentMillis = millis();
  
  if(trafficMode)
  {
    test.trafficMode = 1;
    led5.on();
    delay(200);
    led5.off();
    delay(200);
  } else {
    sw54.dataUpdate();
    test.trafficMode = 0;
    if (sw54.eventDown) 
    {
      led5.toggle();
      Serial.print("LED5: ");
      Serial.println(led5.state());
      sw54.eventDown=0;
    }
  }

  if (currentMillis - previousMillis >= interval && led5.state()==1 ) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    if(pastCarSpeed != nowCarSpeed)
    {
      pastCarSpeed = nowCarSpeed;
      SpeedDisappearClock = 10;  // reset for show car speed
      WiFiClient client;
      HTTPClient http;
      if (client.connect(serverName.c_str(), serverPort)) {
        Serial.println("Connection successful!"); 
        http.begin(client, "http://ierg4230-group10.000webhostapp.com/handleData.php"); 
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String httpRequestData = "speed=" + String(test.carSpeed) + "&distance1=" + String(test.distance1) + "&distance2=" + String(test.distance2);
        int httpResponseCode = http.POST(httpRequestData);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        if (httpResponseCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
        // Free resources
        http.end();
      }

      
      Serial.print("carSpeed: ");
      Serial.println(test.carSpeed);
      Serial.print("distance1: ");
      Serial.println(test.distance1);
      Serial.print("distance2: ");
      Serial.println(test.distance2);
    } else {
      SpeedDisappearClock--;
      if(SpeedDisappearClock<=0)  test.carSpeed = 0;
    }
    //Set values to send
    test.id = BOARD_ID;
    test.disappearClock = SpeedDisappearClock;
    test.esp32_test = -1;
    test.readingId = readingId++;
    

//    Serial.print("loop");
  }
  
}

//====================== I2c Rx Event ============
void I2cRxEvent(size_t howMany)
{
  while ( Wire.available()>0) 
  { 
    char c = Wire.read(); // receive byte as a character
//    Serial.println(c);         // print the character
    if(c=='!') // '' is char, "" is string
    { 
      String tmp_str = "";
      for(int i=0; i<50; i++)
      {
        if(Wire.available()>0)
        {
           char tmp = char(Wire.read());
           tmp_str = tmp_str + tmp;
        }
      }
//      Serial.println(tmp_str);
      int index = tmp_str.indexOf('&');
      distance1 = tmp_str.substring(0, index);
      tmp_str = tmp_str.substring(index+1, tmp_str.length());
      index = tmp_str.indexOf('&'); // can found means have carSpeed, not found means only two distance
      if(index != -1){
        led4.toggle();
        distance2 = tmp_str.substring(0, index);
        carSpeed = tmp_str.substring(index+1, tmp_str.length());
      } else {
        distance2 = tmp_str.substring(index+1, tmp_str.length());
      }

      test.carSpeed = carSpeed.toFloat();
      test.distance1 = distance1.toFloat();
      test.distance2 = distance2.toFloat();
      nowCarSpeed = test.carSpeed;

//      Serial.println(distance1);
//      Serial.println(distance2);
//      Serial.print("carSpeed: ");
//      Serial.println(test.carSpeed);
//      Serial.print("now: ");
//      Serial.print(nowCarSpeed);
//      Serial.print("past: ");
//      Serial.println(pastCarSpeed);
      break;
    }
    if(c=='m' || c=='a') // '' is char, "" is string
    {
      trafficMode = !trafficMode;
      Serial.print("trafficMode:");
      Serial.println(trafficMode);
      
    }
    
  }
}

//========================================================
