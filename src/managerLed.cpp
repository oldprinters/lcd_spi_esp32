#include "managerLed.h"
//_____-------------------------------------------
ManagerLed::ManagerLed(int8_t p, NTPClient *pNtp):OneLed(p), pNtpClient(pNtp){
    moveStat = new MoveStat();
    moveStat -> setDt(FOLLOW_ME_TIME);
    queue.push_back(3);
}
ManagerLed::ManagerLed(int8_t p, int8_t ch, NTPClient *pNtp): OneLed(p, ch), pNtpClient(pNtp){
    moveStat = new MoveStat();
    moveStat -> setDt(FOLLOW_ME_TIME);
    queue.push_back(3);
}
ManagerLed::ManagerLed(int8_t p, int8_t ch, int8_t medium, NTPClient *pNtp): OneLed(p, ch, medium), pNtpClient(pNtp){
    moveStat = new MoveStat();
    moveStat -> setDt(FOLLOW_ME_TIME);
    queue.push_back(3);
}
//------------------------------------------------
void ManagerLed::linkLidar(VL53L1X* pS){
    if(pS != nullptr)
        pSensor = pS;
}
//-------------------------------------------
char const* ManagerLed::getModeName(){
    char const* pStr{nullptr};
    switch(stat){
        case Status::ON: pStr = "On"; break;
        case Status::OFF: pStr = "Off"; break;
        case Status::AUTO: pStr = "Auto"; break;
        default: pStr = "Def"; break;
    }
    changesMade = false;

    return pStr;
}
//-------------------------------------------
int16_t ManagerLed::triggerAuto(){
    // Serial.println(static_cast<int16_t>(stat));
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
    queue.push_back(1);
    if(nBut == 0){
        if(!shortClick){
            toggleMax();
            stat = Status::ON;
        } else {
            switch(nClick){
                case 1: triggerAuto();break;
                case 2: stat = Status::ON; OneLed::setMediumLevel(); break;
                case 3: stat = Status::ON; OneLed::setNightLevel(); break;
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
        queue.push_back(2);
        presence = pr;
    }
}
//---------------------------------------------
void ManagerLed::setMotion(bool st){
    if(moveStat->setMotion(st))
        queue.push_back(3); //фиксируем движение
}
//-------------------------------------
bool ManagerLed::cycle(){
    OneLed::cycle();    //текущее состояние
    if(moveStat->cycle())
        queue.push_back(3); //фиксируем движение
    bool res = moveStat->getStat();   //наличие движения
    if(queue.size() > 0){
    // Serial.print("size = ");
    // Serial.print(queue.size());
    // Serial.print(", d = ");
    // Serial.print(queue[0]);
    // Serial.print(", res = ");
    // Serial.println(res);
        for(auto d: queue){
            switch(d){
                case 1: break;
                case 2: 
                    if(presence && (stat == Status::AUTO) && !light){
                        OneLed::setStat(StatLed::ON);
                        OneLed::setMediumLevel();   
                    }
                    break;
                case 3:
                    if( res ){
                        if(stat == Status::AUTO && (OneLed::getStat() == StatLed::OFF) && !light){
                            OneLed::setStat(StatLed::ON);
                            if(nowDay())
                                OneLed::setMediumLevel();
                            else
                                OneLed::setNightLevel();
                        }
                    } else {
                        if(((*pSensor).ranging_data.range_status != VL53L1X::RangeStatus::RangeValid)
                            || ((*pSensor).ranging_data.range_mm > MAX_LENGTH))
                            {
                            if(stat == Status::AUTO){
                                OneLed::setStat(StatLed::OFF);
                            }
                        }
                    }
                    break;
                case 4: if(stat == Status::AUTO && (OneLed::getStat() == StatLed::OFF) && 
                            !light && ((*pSensor).ranging_data.range_status == VL53L1X::RangeStatus::RangeValid)){
                        OneLed::setStat(StatLed::ON);
                        OneLed::setMediumLevel();
                    } else if(stat == Status::AUTO && (OneLed::getStat() == StatLed::ON) && light){
                        OneLed::setStat(StatLed::OFF);
                    }
                    break;
            }
        }
        queue.clear();
        changesMade = true;
    }
    return res;
}
//----------------------------------------------------
bool ManagerLed::getStat(){
    bool res = (stat == Status::ON) 
        || ((stat == Status::AUTO) && (OneLed::getStat() == StatLed::ON));
    return res;
}
//--------------------------------------------------------
void ManagerLed::setLux(float l){
    if((lux != l) || (lux == 0 && l == 0)){
        bool d1 = lux > LEVEL_LIGHT;
        bool d2 = l > LEVEL_LIGHT;
        if(d1 ^ d2)queue.push_back(4);
        lux = l; 
        light = lux < LEVEL_LIGHT? false: true;
    }
}
//------------------------------------------------------------------------
bool ManagerLed::nowDay(){
  bool res = false;
  int day = pNtpClient->getDay();
  int hour = pNtpClient->getHours();
  if( (day >= 0) && (day < 5) )
    res = (hour > 5) && (hour < 23);
  else
    res = (hour > 8) && (hour < 24);
  return res;
}
//------------------------------------------------
void  ManagerLed::printQueue(){
    if(queue.size() > 0){
        Serial.print("queue: ");
        for(auto i: queue){
            Serial.println(i);
        }
    }
}