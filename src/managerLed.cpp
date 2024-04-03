#include "managerLed.h"

//-------------------------------------------
int16_t ManagerLed::triggerAuto(){
    switch(stat){
        case Status::OFF: stat = Status::AUTO;
            this->setStat(StatLed::ON);
            this->setMediumLevel();
            break;
        case Status::ON: break;
        case Status::AUTO: break;
    }
}
//-------------------------------------------
int16_t ManagerLed::clickBut(int16_t nBut, bool longClick, int16_t nClick){
    if(nBut == 0){
        if(!longClick){
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