#ifndef MANAGERLED_H
#define MANAGERLED_H
#include "OneLed.h"

// enum class Status { OFF, ON };

class ManagerLed: public OneLed {
    // Status stat{Status::OFF};
    // int16_t dim{0};
    int16_t lux{0};         //освещённость
    bool movement{false};   //движение

public:
    ManagerLed(int p):OneLed(p){};
	ManagerLed(int p, int ch): OneLed(p, ch){};
	ManagerLed(int p, int ch, int medium): OneLed(p, ch, medium){};
    int16_t clickBut(int16_t nBut, bool longClick, int16_t nClick); //обработка события click
};

#endif // MANAGERLED_H