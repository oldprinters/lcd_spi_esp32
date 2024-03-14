
#include <Adafruit_ILI9341.h> // include ILI9341 library
#include <WiFi.h> // library to connect to Wi-Fi network
#include "NTPClient.h"
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include "mtime.h"
#include "Timer.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 3600000);

int tftCS = 5; // screen chip select pin
int tftDC = 26; // data command select pin
int tftRST = 25; // reset pin

MTime oldTime;
Timer* timer;
Timer* tInd;
//---------------------------------------------
const char* ssid = "ivanych";
const char* password = "stroykomitet";
WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "192.168.1.34";
const char* msgMotion = "aisle/motion";
const char* msgHSMotion = "hall_small/motion";
//---------------------------------------------
//положение часов
int cursPosX = 30;
int cursPosY = 50;
//---------------------------------------------
// associate tft with ILI9341 lib
Adafruit_ILI9341 tft = Adafruit_ILI9341(tftCS, tftDC, tftRST);
String texts[] = // color names
{"BLUE","RED","GREEN","CYAN","MAGENTA","YELLOW","WHITE","GREY"};
unsigned int colors[ ] = // color codes
{ILI9341_BLUE, ILI9341_RED, ILI9341_GREEN, ILI9341_CYAN,
ILI9341_MAGENTA, ILI9341_YELLOW, ILI9341_WHITE,
ILI9341_LIGHTGREY};
String text;
unsigned int color, chkTime;
//--------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String str = {};
  String strTopic = topic;
  String sOut{};

  for (int i = 0; i < length; i++) {
    str += (char)payload[i];
  }

  if(strTopic == msgMotion){
      tft.setCursor(cursPosX, cursPosY + 80);
      if(str == "1") {
        tft.print("*");
      } else {
        tft.fillRect( cursPosX, cursPosY + 80, 27,37, ILI9341_BLACK );  
      }
  } else if(strTopic == msgHSMotion){
      tft.setCursor(cursPosX + 50, cursPosY + 80);
      tft.print("*");
      tInd->setTimer();
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
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}
//******************************
void setup()
{
    tft.begin(); // initialise screen
    tft.setRotation(1); // portrait, connections at top
    // tft.fillScreen(ILI9341_BLACK);
    tft.fillRect(0,0, 319, 239, ILI9341_BLACK);

    // tft.fillScreen(ILI9341_BLACK); // fill screen in black
    Serial.begin(115200); // Serial Monitor baud rate
    WiFi.begin(ssid, password); // initialise Wi-Fi and wait
    tft.print("Connecting...");
    while (WiFi.status() != WL_CONNECTED){
        tft.print(".");
        delay(500);
    }
    timeClient.begin();
    timeClient.update();
    timer = new Timer(1000);
    tInd = new Timer(5000);
    // tft.drawRect(0,0,319,239,ILI9341_BLUE); // draw white frame line
    tft.fillRect(0,0,319,239,ILI9341_BLACK);
    // tft.drawRect(1,1,237,317,ILI9341_RED); // and second frame line
    tft.setTextSize(4); // set text size
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    reconnect_mqtt();
}
//------------------------------------------------------------------------
void outTime(NTPClient *tk){
    tft.setCursor(cursPosX, cursPosY);
    tk->update();
    tft.setTextSize(4); // set text size
    int16_t c = oldTime.compare(tk);
    int16_t w = (3 - c) * 70;
    // tft.fillRect(cursPosX, cursPosY , 190, 40, ILI9341_BLUE);
    String str = tk->getFormattedTime();
    // tft.setTextColor(ILI9341_WHITE);
    tft.fillRect(cursPosX + w, cursPosY , c * 70 - 20, 30, ILI9341_BLACK);
    tft.println(str);
    if(oldTime.set(tk)){
        String strD = tk->getFormattedDate();
        tft.setCursor(cursPosX, cursPosY + 35);
        tft.setTextSize(2); // set text size
        tft.setTextColor(oldTime.getColorTime());
        tft.fillRect(cursPosX, cursPosY + 35, 240, 20, ILI9341_BLACK);
        tft.print(strD);
    }
}
//------------------------------------------------------------------------
void loop()
{
    // tft.print(timeClient.getFormattedTime());
    if(timer->getTimer()){
        timer->setTimer();
        outTime(&timeClient);
        reconnect_mqtt();
        if(tInd->getTimer())
          tft.fillRect( cursPosX + 50, cursPosY + 80, 27,37, ILI9341_BLACK );
    }
    client.loop();
    // delay(1000);
    //  // clear screen apart from frame
    // tft.fillRect(2,2,235,314,ILI9341_BLACK);
    // for (int i=0; i<8; i++) // for each color
    // {
    //     color = colors[i]; // set color
    //     text = texts[i]; // set text for color
    //     tft.setTextColor(color); // set text color
    //     tft.setCursor(20,40*i+2); // position cursor
    //     tft.print(text); // print color text (name)
    //     delay(250); // delay 250ms between colors
    // }
    // for (int i=0; i<8; i++) // for each color
    // {
    //     color = colors[i];
    //     text = texts[i];
    //     tft.fillRect(2,2,235,314,ILI9341_BLACK);
    //     tft.setCursor(20,25); // cursor to position (20, 25)
    //     tft.setTextColor(color);
    //     tft.print(text); // draw filled-in
    //     // triangle
    //     if ((i+1)%3==0) tft.fillTriangle(20,134,64,55,107,134,color);
    //     // draw open rectangle
    //     else if ((i+1)%2==0) tft.drawRect(20,55,88,80,color);
    //     else tft.fillCircle(64,95,39,color); // draw filled-in circle
    //     delay(250);
    // }
    // tft.fillRect(2,2,235,314,ILI9341_BLACK);
    // tft.drawLine(2,158,236,158,ILI9341_RED );
    // // draw horizontal RED line
    // delay(250);
}