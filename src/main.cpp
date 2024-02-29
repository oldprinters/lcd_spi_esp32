
#include <Adafruit_ILI9341.h> // include ILI9341 library
#include <WiFi.h> // library to connect to Wi-Fi network
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 3600000);

int tftCS = 5; // screen chip select pin
int tftDC = 26; // data command select pin
int tftRST = 25; // reset pin

const char* ssid = "ivanych";
const char* password = "stroykomitet";
WiFiClient client;

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
    Serial.begin(115200); // Serial Monitor baud rate
    WiFi.begin(ssid, password); // initialise Wi-Fi and wait
    while (WiFi.status() != WL_CONNECTED) delay(500);
    // for Wi-Fi connection
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // Wi-Fi network IP address
    Serial.println("Connecting...");

    timeClient.begin();
    timeClient.update();

    tft.begin(); // initialise screen
    tft.setRotation(2); // portrait, connections at top
    tft.fillScreen(ILI9341_BLACK); // fill screen in black
    tft.drawRect(0,0,239,319,ILI9341_WHITE); // draw white frame line
    tft.drawRect(1,1,237,317,ILI9341_WHITE); // and second frame line
    tft.setTextSize(4); // set text size
}
void loop()
{
    tft.setCursor(15, 25);
    timeClient.update();
    tft.fillRect(15,15,200,100,ILI9341_BLACK);
    tft.print(timeClient.getFormattedTime());
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