#include "managerLed.h"

//-------------------------------------------
int16_t ManagerLed::triggerAuto(){
    Serial.println(static_cast<int16_t>(stat));
    switch(stat){
        case Status::ON: 
        case Status::OFF: stat = Status::AUTO;
            OneLed::setStat(StatLed::ON);
            OneLed::setMediumLevel();
            break;
        case Status::AUTO: stat = Status::OFF;
            OneLed::setOff();
            break;
    }
    return static_cast<int16_t>(stat);
}
//-------------------------------------------
int16_t ManagerLed::clickBut(int16_t nBut, bool shortClick, int16_t nClick){
    if(nBut == 0){
        if(!shortClick){
            toggleMax();
        } else {
            switch(nClick){
                case 1: triggerAuto();break;
                case 2: setOff(); break;
                case 3: break;
            }
        }
    } else {
        //пока мыслей нет
    }
    return 1;//this->getStat() == 1;
}