#include "managerLed.h"
//------------------------------------------------
MoveStat* newMS(){
    MoveStat* moveStat = new MoveStat();
    moveStat -> setDt(1500);
    return moveStat;
}
//_____-------------------------------------------
ManagerLed::ManagerLed(int8_t p):OneLed(p){
    moveStat = newMS();
}
ManagerLed::ManagerLed(int8_t p, int8_t ch): OneLed(p, ch){
    moveStat = newMS();
}
ManagerLed::ManagerLed(int8_t p, int8_t ch, int8_t medium): OneLed(p, ch, medium){
    moveStat = newMS();
}
//------------------------------------------------
void ManagerLed::linkLidar(VL53L1X* pS){
    if(pS != nullptr)
        pSensor = pS;
}
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
//-------------------------------------------------------
//если появился объект, то в режиме AUTO включается свет
void ManagerLed::setLidar(int16_t mm){
    //presence
    bool pr = mm < MAX_LENGTH? true: false;
    if(pr != presence){
        presence = pr;
        if(presence){
            if(stat == Status::AUTO && !light){
                OneLed::setStat(StatLed::ON);
                OneLed::setMediumLevel();
            }
        } else {
            //???????
        }
    }
}
//---------------------------------------------
void ManagerLed::setMotion(bool st){
    moveStat -> setMotion(st);
    if( moveStat -> getStat() ){
        if(stat == Status::AUTO && !light && (OneLed::getStat() == StatLed::OFF)){
            OneLed::setStat(StatLed::ON);
            OneLed::setMediumLevel();
        }
    } else {
        if((*pSensor).ranging_data.range_status != VL53L1X::RangeStatus::RangeValid){
            if(stat == Status::AUTO){
                OneLed::setStat(StatLed::OFF);
            }
        }
    }
}
//-------------------------------------
bool ManagerLed::cycle(){
    OneLed::cycle();    //текущее состояние
    bool res = moveStat->cycle();   //наличие движения
    if( (res == 0) && (((*pSensor).ranging_data.range_status != VL53L1X::RangeStatus::RangeValid) ||
    ((*pSensor).ranging_data.range_mm > MAX_LENGTH))){
        OneLed::setStat(StatLed::OFF);
    }
    return res;
}
//----------------------------------------------------
bool ManagerLed::getStat(){
    bool res = (stat == Status::ON) 
        || ((stat == Status::AUTO) && (OneLed::getStat() == StatLed::ON));
    return res;
}