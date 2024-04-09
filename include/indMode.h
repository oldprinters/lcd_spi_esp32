#ifndef INDMODE_H
#define INDMODE_H
#include "Timer.h"
#include <Adafruit_ILI9341.h> // include ILI9341 library
#define TFT_RED 0x01F
// задержка выключения на заданное время

#define T_DELAY_MODE 5000

class IndMode: Timer {
    uint16_t sdvX{260};
    uint16_t sdvY{5};
    uint16_t width{60};
    uint16_t height{20};
    uint16_t color{TFT_RED};
    Adafruit_ILI9341 *tftM;
public:
    IndMode(Adafruit_ILI9341 *tft):Timer(T_DELAY_MODE), tftM(tft){}
    void outMode(char const* str);
};

#endif