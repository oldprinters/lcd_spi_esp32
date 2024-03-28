//movestat
#include "movestat.h"

//-------------------------------------------
void MoveStat::setMotion(bool st){
    if(st){
        stat = 1;
        wait = 0;
    } else {
        wait = 1;
        setTimer();
    }
}
//----------------------------------
int16_t MoveStat::cycle(){
    if(wait && getTimer()){
        wait = 0;
        stat = 0;
    }
    return stat;
}