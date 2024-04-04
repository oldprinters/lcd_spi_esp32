#ifndef MOVESTAT_H
#define MOVESTAT_H
#include "Timer.h"
#define FOLLOW_ME_TIME 10000

class MoveStat:Timer {
    bool moving{0};
    bool stat{0};
    bool statOld{0};
    bool wait{0};   //ожидание перед выключением
    uint16_t maxLength{1800};
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