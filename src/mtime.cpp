#include "mtime.h"

int16_t MTime::compare(NTPClient *tC){
    int16_t th = tC->getHours();
    int16_t tm = tC->getMinutes();
    int16_t ts = tC->getSeconds();
    int16_t rez = 0;
    if(th != h){
        rez = 1;
    }else if(tm != m) {
        rez = 2;
    }else if(ts != s){
        rez = 3;
    }
    return rez;
}
//---------------------------
bool MTime::set(NTPClient *tC){
    bool res = false;
    h = tC->getHours();
    m = tC->getMinutes();
    s = tC->getSeconds();

    res = ((h + m + s) == 0) || (!dateSet);
    
    if(res){
        d = tC -> getDay();
        d = ( d == 0? 7: d);
    }
    dateSet = 1;
    return res;
}
//---------------------------
uint16_t MTime::getColorTime(NTPClient *tk){
    uint16_t res = 0x7BEF;
    h = tk->getHours();
    m = tk->getMinutes();
    s = tk->getSeconds();
    
    if((h > 5) && (h < 23)){
        res = 0xFFFF;
    } 
    return res;
}//---------------------------
uint16_t MTime::getColorDate(){
    uint16_t res = 0xFFFF;
    if(d == 0 || d == 6){
        res = 0xFFFF;
    }
    return res;
}