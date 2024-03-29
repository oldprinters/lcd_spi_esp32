#include "IndMsg.h"

//--------------------------------------------
void IndMsg::cycle(){
    if(stat && getTimer()){
        stat = 0;
        tftM->fillCircle(sdvX, sdvY, 8, 0x0);
    }
}
//--------------------------------------------
void IndMsg::set(){
    Timer::setTimer(); 
    stat = true;
    tftM->fillCircle(sdvX, sdvY, 8, color);
}
