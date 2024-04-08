#ifndef MANAGERLED_H
#define MANAGERLED_H
#include "OneLed.h"
#include "movestat.h"
#include <VL53L1X.h>

#define LEVEL_LIGHT 100

enum class Status { OFF, ON, AUTO };

class ManagerLed: OneLed {
    Status stat{Status::OFF};
    // int16_t dim{0};
    int16_t lux{0};         //освещённость
    bool movement{false};   //движение
    bool presence{false};   //присутствие в зоне лидара
    bool light{false};      //светло
    MoveStat* moveStat;
    VL53L1X* pSensor{nullptr};       //указатель на lidar
public:
    ManagerLed(int8_t p);
	ManagerLed(int8_t p, int8_t ch);
	ManagerLed(int8_t p, int8_t ch, int8_t medium);
    ~ManagerLed(){delete moveStat;}
    void linkLidar(VL53L1X* pS);
    int16_t clickBut(int16_t nBut, bool longClick, int16_t nClick); //обработка события click
    bool getStat();
    int16_t triggerAuto();
    void setLux(float l){lux = l; light = lux < LEVEL_LIGHT? false: true;}
    void setLidar(int16_t mm);
    void setMotion(bool st, bool nDay);
    bool cycle();
    // void lidarOn
};

#endif // MANAGERLED_H