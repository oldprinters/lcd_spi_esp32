#include "mtime.h"

int16_t MTime::compare(NTPClient *tC){
    int16_t th = tC->getHours();
    int16_t tm = tC->getMinutes();
    int16_t ts = tC->getSeconds();
    int16_t rez = 0;
    if(th != h){
        rez = 3;
    }else if(tm != m) {
        rez = 2;
    }else if(ts != s){
        rez = 1;
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
    dateSet = 1;
    return res;
}
