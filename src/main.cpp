#include <Arduino.h>
#include <Adafruit_ILI9341.h> // include ILI9341 library
#include <WiFi.h> // library to connect to Wi-Fi network
#include "NTPClient.h"
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include "mtime.h"
#include "Timer.h"
#include "indMode.h"
#include "IndMsg.h"
#include "movestat.h"
#include "managerLed.h"
#include "OneLed.h"
#include <Wire.h>
#include <VL53L1X.h>
#include <BH1750.h> 
#include "FreeMono24pt7b.h"
#include "FreeMono12pt7b.h"
//с большим дисплеем у меня на столе

VL53L1X sensor;
Timer* timer53L1;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 3600000);
NTPClient* ptk = &timeClient;
const int tftCS = 5; // screen chip select pin
const int tftDC = 26; // data command select pinp
const int tftRST = 25; // reset pin

const int16_t pinMove = 27;
volatile bool motion{0};  //признак изменения движения

MTime oldTime;
Timer* timer;
Timer* tInd;
Timer* tHeard;
bool bHeard{false};
//---------------------------------------------
const char* ssid = "ivanych";
const char* password = "stroykomitet";
WiFiClient espClient;
PubSubClient client(espClient);
//---------------------------------------------
const char* msg_lidar= "kitcen/lidar";
const char* mqtt_server = "192.168.1.34";
const char* msgMotion = "aisle/motion";
const char* msgHSMotion = "hall_small/motion";
const char* msgTemper="balcony/temper";
const char* msgPressure="balcony/pressure";
//---------------------------------------------
BH1750 lightMeter(0x23);  //0x5c
float lux{8000};  //яркость света в помещении
//--------------------------------------------- кнопки
const int16_t pinBut1{34};
const int16_t pinBut2{35};
volatile int16_t buttonStatus_1{0}, ft_1{0}, ft_1_dr{0}, nClickBut_1{0};
volatile int16_t buttonStatus_2{0}, ft_2{0}, ft_2_dr{0}, nClickBut_2{0};
const int drDelay{250};
const int pauseDelay{2000};
int16_t ft_1_pause{0};
Timer tLux(500);
Timer tButt_1(1000);
Timer tDrebezg_1(drDelay);
Timer tPause_1(pauseDelay);
Timer tButt_2(1000);
Timer tDrebezg_2(drDelay);
//---------------------------------------------
//положение часов
const int16_t cursPosX = 2;
const int16_t cursPosY = 60;
const int16_t cursDatePosY = 80;
const int16_t wSigns = 95;
const int16_t hSigns = 60;
const int16_t pointHM = 122;  //смещение точки разделителя от начала экрана
const int16_t dt = 30;  //промежуток между часами, минутами и секундами
const int16_t lidarPosY = 160;
const int16_t temperPosX = 10;
const int16_t pressDPosX = 165;
const int16_t temperPosY = 200;
const int16_t temperLength = 150;

const int16_t sdvY = 55;
const int16_t sdvX = 50;
//---------------------------------------------
// associate tft with ILI9341 lib
Adafruit_ILI9341 tft = Adafruit_ILI9341(tftCS, tftDC, tftRST);
#define TFT_BLACK 0x0
#define TFT_DACKGRAY 0x618C
#define TFT_GRAY 0x7BEF
#define TFT_WHITE 0xFFFF
#define TFT_RED 0x01F
#define TFT_CYAN 0xFE73
#define TFT_TURQUOISE 0x7FE0
#define TFT_BLUE 0xF800
#define TFT_GREEN 0x0FE0
#define TFT_YELLOW 0x07FF
#define TFT_ORANGE 0x3FF
unsigned int colors[ ] = // color codes
{TFT_WHITE, TFT_CYAN, TFT_WHITE, TFT_GREEN, TFT_WHITE, TFT_TURQUOISE, TFT_WHITE, TFT_GREEN};
uint16_t nColor{0};
// {ILI9341_BLACK, ILI9341_WHITE, ILI9341_LIGHTGREY, ILI9341_CYAN, ILI9341_BLUE, ILI9341_GREEN, ILI9341_MAGENTA, ILI9341_YELLOW, ILI9341_ORANGE, ILI9341_RED};
unsigned int color, chkTime;
//---------------------------------------------
const int16_t pinLed{32};
const int16_t MEDIUM_LEVEL = 50;
IndMsg indMsg(&tft);
IndMode indMode(&tft);
ManagerLed light_1(pinLed, 0, MEDIUM_LEVEL, &timeClient);
//*********************************************
void IRAM_ATTR button_interr_1(){ //IRAM_ATTR
    detachInterrupt(pinBut1);
  ft_1_dr = 2;
  if(!ft_1){
    ft_1 = 3;
    nClickBut_1 = 1;
  } else if(ft_1 == 2){
    nClickBut_1++;
  }
}
//*********************************************
void IRAM_ATTR button_interr_2(){ //IRAM_ATTR
    detachInterrupt(pinBut2);
  ft_2_dr = 2;
  if(!ft_2){
    ft_2 = 3;
    nClickBut_2 = 1;
  } else if(ft_2 == 2){
    nClickBut_2++;
  }
}
//--------------------------------------
uint16_t getColorTemper(String str){
  uint16_t colorTemper = TFT_WHITE;
      double dTemper = std::stod(str.c_str());
      if(dTemper < -8) colorTemper = TFT_BLUE;  //синий
      else if(dTemper < -5) colorTemper = TFT_CYAN; //голубой
      else if(dTemper < 0) colorTemper = TFT_GRAY; //
      else if(dTemper < 10) colorTemper = 0xFFF0;  //
      else if(dTemper < 20) colorTemper = TFT_YELLOW; //желтый
      else if(dTemper < 28) colorTemper = TFT_ORANGE;  //оранжевый
      else colorTemper = TFT_RED; //красный
      return colorTemper;
}
//--------------------------------------
uint16_t getColorPressure(String str){
  uint16_t colorPressure = ILI9341_WHITE;
      double dPressure = std::stod(str.c_str());
      if(dPressure < 740) colorPressure = TFT_BLUE;  //синий
      else if(dPressure < 750) colorPressure = TFT_CYAN; //голубой
      else if(dPressure < 760) colorPressure = TFT_WHITE;  //белый
      else if(dPressure < 770) colorPressure = TFT_YELLOW; //желтый
      else if(dPressure < 780) colorPressure = TFT_ORANGE;  //оранжевый
      else dPressure = TFT_RED; //красный
      return colorPressure;
}
//--------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String str = {};
  String strTopic = topic;
  String sOut{};
  tft.setTextSize(2); // set text size

  for (int i = 0; i < length; i++) {
    str += (char)payload[i];
  }

  if(strTopic == msgMotion){
    tft.setTextColor(TFT_RED);
    tft.setCursor(cursPosX, cursPosY + sdvY);
    if(str == "1") {
      tft.print("V");
    } else {
      tft.fillRect( cursPosX, cursPosY + sdvY, 30,37, TFT_BLACK );  
    }
  } else if(strTopic == msgHSMotion){
    tft.setTextColor(TFT_ORANGE);
    tft.setCursor(cursPosX + sdvX, cursPosY + sdvY);
    tft.print("K");
    tInd->setTimer();
  } else if(strTopic == msgTemper){
      tft.setTextColor(getColorTemper(str));
      tft.fillRect( temperPosX, temperPosY, temperLength, 30, TFT_BLACK );  
      tft.setCursor(temperPosX, temperPosY + 15);
      tft.setFont(&FreeMono12pt7b);
      tft.setTextSize(1);
      tft.print("T: " + str);
      tft.setFont();
  } else if(strTopic == msgPressure){
      tft.setTextColor(getColorPressure(str));
      tft.fillRect( temperPosX + pressDPosX, temperPosY, temperLength, 30, TFT_BLACK );  
      tft.setCursor(temperPosX + pressDPosX, temperPosY + 15 );
      tft.setFont(&FreeMono12pt7b);
      tft.setTextSize(1);
      tft.print("P: " + str);
      tft.setFont();
  }
}
//-----------------------------------
void reconnect_mqtt() {
  // Loop until we're reconnected
  if(!client.connected()) {
    while (!client.connected()) {
      Serial.println("Attempting MQTT connection...");
      String clientId = "Kitchen-Ptr";
      // Attempt to connect
      Serial.println(clientId);
      if (client.connect(clientId.c_str())) {
        Serial.println("connected");
        client.subscribe(msgMotion, 0);
        client.subscribe(msgHSMotion, 0);
        client.subscribe(msgTemper, 0);
        client.subscribe(msgPressure, 0);
        tft.fillRect(cursPosX, lidarPosY, 200, hSigns + 1, TFT_BLACK);
      } else {
        tft.setCursor(cursPosX, lidarPosY);
        tft.fillRect(cursPosX, lidarPosY, 200, hSigns + 1, TFT_BLACK);
        tft.print("Check the server MQTT.");
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}
//*********************************************
void IRAM_ATTR moving(){
  motion = true;
//  Serial.println("ir_interr");
}
//******************************
void setup()
{
    pinMode(pinMove, INPUT);

    tft.begin(); // initialise screen
    tft.setRotation(0); // portrait, connections at top
    // tft.fillScreen(ILI9341_BLACK);
    tft.fillRect(0,0, 320, 240, TFT_BLACK);
    tLux.setTimer();
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C

    sensor.setTimeout(500);
    if (!sensor.init())
    {
      Serial.println("Failed to detect and initialize sensor!");
      while (1);
    }

    // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
    // You can change these settings to adjust the performance of the sensor, but
    // the minimum timing budget is 20 ms for short distance mode and 33 ms for
    // medium and long distance modes. See the VL53L1X datasheet for more
    // information on range and timing limits.
    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(50000);
    sensor.setROISize(16, 16);

    // Start continuous readings at a rate of one measurement every 50 ms (the
    // inter-measurement period). This period should be at least as long as the
    // timing budget.
    sensor.startContinuous(50);
    light_1.linkLidar(&sensor);

    WiFi.begin(ssid, password); // initialise Wi-Fi and wait
    tft.print("Connecting...");
    while (WiFi.status() != WL_CONNECTED){
        tft.print(".");
        delay(500);
    }
    timeClient.begin();
    timeClient.update();
    timer = new Timer(1000);
    tInd = new Timer(10000);
    tHeard = new Timer(500);
    timer53L1 = new Timer(500);
    // tft.drawRect(0,0,319,239,ILI9341_BLUE); // draw white frame line
    tft.fillRect(0,0,319,239,TFT_BLACK);
    // tft.drawRect(1,1,237,317,ILI9341_RED); // and second frame line
    // tft.setTextSize(4); // set text size
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    reconnect_mqtt();
    //---------------------------
    if (lightMeter.begin()) {//BH1750::CONTINUOUS_HIGH_RES_MODE)) {
      Serial.println(F("BH1750 Advanced begin"));
    }
    else {
      Serial.println(F("Error initialising BH1750"));
    }

    attachInterrupt(digitalPinToInterrupt(pinMove), moving, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinBut1), button_interr_1, FALLING);
    attachInterrupt(digitalPinToInterrupt(pinBut2), button_interr_2, FALLING);
}
//------------------------------------------------------------------------
void outTime(NTPClient *tk){
    tk->update();
    tft.setTextSize(2); // set text size
    int16_t c = oldTime.compare(tk);
    // tft.setTextColor(oldTime.getColorTime(tk));
    tft.setTextColor(oldTime.getColorTime(tk));

    String strM = tk->getFormattedM();
    String strH = tk->getFormattedH();
    String strSec = tk->getFormattedSec();
    // tft.setTextColor(ILI9341_WHITE);
    int16_t w = (c - 1) * (wSigns + dt);
    tft.setFont(&FreeMono24pt7b);

    switch(c){
      case 1: tft.setCursor(cursPosX, cursPosY);
        tft.fillRect(cursPosX, 4, wSigns + 10, hSigns + 1, TFT_BLACK);
        tft.print(strH);
      case 2: tft.setCursor(cursPosX + wSigns + dt, cursPosY);
        tft.fillRect(cursPosX + wSigns + dt, 4, wSigns + 10, hSigns + 1, TFT_BLACK);
        tft.print(strM);
      case 3: tft.setCursor(cursPosX + (wSigns + dt) * 2, cursPosY);
        tft.fillRect(cursPosX + (wSigns + dt) * 2, 32, wSigns - 40, hSigns - 28, TFT_BLACK);
        tft.setTextSize(1); // set text size
        tft.println(strSec);
        break;
    }

    tft.setFont();
    // tft.drawCircle( 130, 50, 5, TFT_WHITE);
    tft.fillCircle( pointHM, cursPosY, 3, colors[nColor++]);
    if(nColor == sizeof(colors)/sizeof(colors[0]))
      nColor = 0;
    tHeard->setTimer();
    bHeard = true; 

    if(oldTime.set(tk)){
        String strD = tk->getFormattedDate();
        tft.setCursor(cursPosX, cursDatePosY);
        tft.setTextSize(3); // set text size
        tft.setTextColor(oldTime.getDay()? TFT_GRAY: TFT_CYAN);
        tft.fillRect(cursPosX, cursDatePosY, 320, 30, ILI9341_BLACK);
        tft.println(strD);
    }
}
//------------------------------------------------------------------------
void outLidar(Adafruit_ILI9341& tft, int16_t mm){
    tft.setCursor(cursPosX, lidarPosY);
    tft.setTextSize(2); // set text size
    tft.setTextColor(TFT_TURQUOISE);
    tft.fillRect(cursPosX, lidarPosY, 100, 30, TFT_BLACK);
    tft.print(String(mm) + " mm");
    tft.setCursor(cursPosX + pressDPosX, lidarPosY);
    tft.fillRect(cursPosX + pressDPosX, lidarPosY, 120, 30, TFT_BLACK);
    tft.print(lux);
}
//------------------------------------------------------------------------
int16_t motion_func(){
  bool res{false};
  if(motion){
    res = true;
    motion = false;
    int p = digitalRead(pinMove);
    light_1.setMotion(p);
  }
  return res;
}
//------------------------------------------------------------------------
void loop()
{
    // tft.print(timeClient.getFormattedTime());
    if(timer->getTimer()){  //вывод времени
      timer->setTimer();
      outTime(&timeClient);
      reconnect_mqtt();
      if(tInd->getTimer())
        tft.fillRect( cursPosX + sdvX, cursPosY + sdvY, 20,30, ILI9341_BLACK );
    }
    client.loop();  //mqtt
    if(bHeard && tHeard->getTimer()){ //выключена мигающая точка
      bHeard = false;
      tft.fillCircle( pointHM, cursPosY, 4, TFT_BLACK);
    }

    if(timer53L1->getTimer()){  //дальномер
      sensor.read();
      timer53L1->setTimer();
      if(sensor.ranging_data.range_status == VL53L1X::RangeStatus::RangeValid){
          outLidar(tft, sensor.ranging_data.range_mm);//sensor.ranging_data.range_mm);
          light_1.setLidar(sensor.ranging_data.range_mm);
          // Serial.print("range: ");
          // Serial.print(sensor.ranging_data.range_mm);
          // Serial.print("\tstatus: ");
          // Serial.print(VL53L1X::rangeStatusToString(sensor.ranging_data.range_status));
          // Serial.print("\tpeak signal: ");
          // Serial.print(sensor.ranging_data.peak_signal_count_rate_MCPS);
          // Serial.print("\tambient: ");
          // Serial.print(sensor.ranging_data.ambient_count_rate_MCPS);
          // Serial.println();
      }
    }
    //....... измерение освещённости
    if (tLux.getTimer() && lightMeter.measurementReady()) {
      tLux.setTimer();
      lux = lightMeter.readLightLevel();
      light_1.setLux(lux);
    }
    
    motion_func();  //проверка движения

  //-------------------------------------------------------------------------------- BUTTONS
  if(ft_1 == 3){
    tButt_1.setTimer();
    ft_1 = 2;
  }
  if( ft_1_dr == 2 ){
    tDrebezg_1.setTimer();
    ft_1_dr = 1;
  }
  if( ( ft_1_dr == 1 ) && tDrebezg_1.getTimer() ){
    ft_1_dr = 0;
    attachInterrupt(digitalPinToInterrupt(pinBut1), button_interr_1, FALLING);
  }
  if( ( ft_1_pause == 1 ) && tPause_1.getTimer() ){
    ft_1_pause = 0;
    attachInterrupt(digitalPinToInterrupt(pinBut1), button_interr_1, FALLING);
  }

  if(ft_1 == 2 && tButt_1.getTimer()){
    int16_t i = digitalRead(pinBut1);
    // Serial.print(i == 0? "Long ": "Short ");
    // Serial.print("nCount = ");
    // Serial.println(nClickBut_1);
    ft_1 = 0;
    ft_1_pause = 1;
    tPause_1.setTimer();
    detachInterrupt(pinBut1);
    light_1.clickBut(0, i, nClickBut_1);
  }

  if(light_1.getChangesMade())
    indMode.outMode(light_1.getModeName());
  //---------------------
  if(ft_2 == 3){
    tButt_2.setTimer();
    ft_2 = 2;
  }
  if( ft_2_dr == 2 ){
    tDrebezg_2.setTimer();
    ft_2_dr = 1;
  }
  if( ( ft_2_dr == 1 ) && tDrebezg_2.getTimer() ){
    ft_2_dr = 0;
    attachInterrupt(digitalPinToInterrupt(pinBut2), button_interr_2, FALLING);
  }

  if(ft_2 == 2 && tButt_2.getTimer()){
    int16_t i = digitalRead(pinBut2);
    // Serial.print("Button 2:  ");
    // Serial.print(i == 0? "Long ": "Short ");
    // Serial.print("nCount = ");
    // Serial.println(nClickBut_2);
    ft_2 = 0;
    ft_2_dr = 1;
    tDrebezg_2.setTimer();
    detachInterrupt(pinBut2);
  }

  if(light_1.cycle()){
    indMsg.set();
  }
  indMsg.cycle();
}