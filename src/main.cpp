
#include <Adafruit_ILI9341.h> // include ILI9341 library
#include <WiFi.h> // library to connect to Wi-Fi network
#include "NTPClient.h"
#include <WiFiUdp.h>
#include "mtime.h"
#include "Timer.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 3600000);

int tftCS = 5; // screen chip select pin
int tftDC = 26; // data command select pin
int tftRST = 25; // reset pin

MTime oldTime;
Timer* timer;
//---------------------------------------------
const char* ssid = "ivanych";
const char* password = "stroykomitet";
WiFiClient client;
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
    // tft.drawRect(0,0,319,239,ILI9341_BLUE); // draw white frame line
    tft.fillRect(0,0,319,239,ILI9341_BLACK);
    // tft.drawRect(1,1,237,317,ILI9341_RED); // and second frame line
    tft.setTextSize(4); // set text size
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
    tft.fillRect(cursPosX + w, cursPosY , c * 70 - 20, 30, ILI9341_BLACK);
    tft.println(str);
    if(oldTime.set(tk)){
        String strD = tk->getFormattedDate();
        tft.setCursor(cursPosX, cursPosY + 35);
        tft.setTextSize(2); // set text size
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
    }
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