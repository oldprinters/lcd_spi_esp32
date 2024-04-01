#include "managerLed.h"

//-------------------------------------------
int16_t ManagerLed::clickBut(int16_t nBut, bool longClick, int16_t nClick){
    if(nBut == 0){
        if(!longClick){
            toggleMax();
        } else {
    Serial.print("nBut = ");
    Serial.print(nBut);
    Serial.print(", longClick = ");
    Serial.print(longClick);
    Serial.print(", nClick = ");
    Serial.println(nClick);
            switch(nClick){
                case 1: trigger();break;
                case 2: setOff(); break;
                case 3: break;
            }
        }
    } else {
        //пока мыслей нет
    }
    return 1;//this->getStat() == 1;
}