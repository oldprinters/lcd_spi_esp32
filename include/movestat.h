#ifndef MOVESTAT_H
#define MOVESTAT_H
#include "Timer.h"
const int16_t FOLLOW_ME_TIME = 10000;
const int16_t MAX_LENGTH {1700};

class MoveStat:Timer {
    bool moving{0};
    bool stat{0};
    bool statOld{0};
    bool wait{0};   //ожидание перед выключением
    uint16_t maxLength{MAX_LENGTH};
  public:
    MoveStat():Timer(FOLLOW_ME_TIME){};
    MoveStat(uint16_t ml):Timer(FOLLOW_ME_TIME), maxLength(ml){};
    MoveStat(uint32_t dt, uint16_t ml):Timer(dt), maxLength(ml){};
    bool getStat(){return stat;}
    void setML(uint16_t ml){maxLength = ml;}
    void setMotion(bool st);
    int16_t cycle();
    void setDt(uint32_t dt) {Timer::setDt(dt);}
};

#endif