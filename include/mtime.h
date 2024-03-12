#ifndef MTIME_H
#define MTIME_H

#include <Arduino.h>
#include "NTPClient.h"

class MTime{
    int16_t h;
    int16_t m;
    int16_t s;
    int16_t dateSet{0};
    int16_t d;
    public:
    MTime(){
        h = 0;
        m = 0;
        s = 0;
        d = 0;
    };
    int16_t compare(NTPClient *tC);
    bool set(NTPClient *tC);
    uint16_t getColorTime();
    uint16_t getColorDate();
};


#endif

