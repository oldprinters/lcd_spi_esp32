#include "indMode.h"

//---------------------------------------------------
void IndMode::outMode(char const* str){
    tftM->fillRect( sdvX, sdvY, width, height, 0 );
    tftM->setCursor(sdvX, sdvY);
    tftM->setTextSize(2);
    tftM->setTextColor(TFT_RED);
    tftM->print(str);
}
