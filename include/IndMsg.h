#ifndef INDMSG_H
#define INDMSG_H
#include "Timer.h"

class IndMsg : Timer {
    char sign{'*'};
    bool stat{false};
public:
    IndMsg():Timer(4000){}
    IndMsg(uint32_t dt):Timer(dt){}
    IndMsg(char c, uint32_t dt = 4000):Timer(dt){sign = c;}
    void set(){this->setTimer();}
};

#endif