
#include <Adafruit_ILI9341.h> // include ILI9341 library
#include <WiFi.h> // library to connect to Wi-Fi network
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 3600000);

int tftCS = 5; // screen chip select pin
int tftDC = 26; // data command select pin
int tftRST = 25; // reset pin
class MTime{
    int16_t h;
    int16_t m;
    int16_t s;
    public:
    MTime(){
        h = 0;
        m = 0;
        s = 0;
    };
    //-----------------------
    int16_t compare(NTPClient *tC){
        int16_t th = tC->getHours();
        int16_t tm = tC->getMinutes();
        int16_t ts = tC->getSeconds();
        int16_t rez = 0;
        if(th != h){
            rez = 7;
        }else if(tm != m) {
            rez = 3;
        }else if(ts != s){
            rez = 1;
        }
        return rez;
    }
    //---------------------------
    void set(NTPClient *tC){
        h = tC->getHours();
        m = tC->getMinutes();
        s = tC->getSeconds();
    }
};
MTime oldTime;
//---------------------------------------------
const char* ssid = "ivanych";
const char* password = "stroykomitet";
WiFiClient client;
//---------------------------------------------
//положение часов
int cursPosX = 30;
int cursPosY = 10;
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

    tft.drawRect(0,0,319,239,ILI9341_BLUE); // draw white frame line
    tft.drawRect(1,1,237,317,ILI9341_RED); // and second frame line
    tft.setTextSize(4); // set text size
}
//------------------------------------------------------------------------
void outTime(NTPClient *tk){
    tft.setCursor(cursPosX, cursPosY);
    tk->update();
    tft.setTextSize(4); // set text size
    int16_t c = oldTime.compare(tk);
    tft.fillRect(cursPosX + (3 - c) * 60, cursPosY ,cursPosX + 180,cursPosY + 150,ILI9341_BLACK);
    tft.println(tk->getFormattedTime());
    tft.setTextSize(2); // set text size
    tft.print("          ");
    tft.print(oldTime.compare(tk));
    oldTime.set(tk);
}
//------------------------------------------------------------------------
void loop()
{
    // tft.print(timeClient.getFormattedTime());
    outTime(&timeClient);
    delay(1000);
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