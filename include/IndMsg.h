#ifndef INDMSG_H
#define INDMSG_H
#include "Timer.h"
#define T_DELAY 1000
#include <Adafruit_ILI9341.h> // include ILI9341 library
// задержка выключения на заданное время
class IndMsg : Timer {
    uint16_t sdvX{230};
    uint16_t sdvY{130};
    uint16_t color{0xffff};
    bool stat{false};
    Adafruit_ILI9341 *tftM;
public:
    IndMsg(Adafruit_ILI9341 *tft):Timer(T_DELAY), tftM(tft){}
    IndMsg(Adafruit_ILI9341 *tft, uint32_t dt):Timer(dt), tftM(tft){}
    void setSdvXY(uint16_t x, uint16_t y){sdvX = x; sdvY = y;}
    void setSdvX(uint16_t x){sdvX = x;}
    void setSdvY(uint16_t y){sdvY = y;}
    void setColor(uint16_t col){color = col;}
    void set();
    void cycle();
};

#endif