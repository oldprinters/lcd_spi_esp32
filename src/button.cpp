#include "button.h"

Button::Button(int p, void (*f)(), void (*f1)(), void (*f2)()): Timer(1500), pin(p), fDouble(f), fLong(f1), fShort(f2){
  pinMode(pin, INPUT);
  postPress.setDt(longPress);
}
//************************************************
void Button::press_button(){
  if(!firstPress && postPress.getTimer()){
    setTimer(1000);
    tTime = millis();
    firstPress = true;
  } else {
    if((millis() - tTime) < 800){
      fDouble();
      setTimer(-1);                //сброс таймеров
      firstPress = false;
      postPress.setTimer(longPress);
    }
  }
}
//**********************************************
void Button::cycle(){
  if(firstPress && getTimer() && postPress.getTimer()){
    firstPress = false;
    if(digitalRead(pin))
      fLong();
    else
      fShort();
    postPress.setTimer(longPress);
  }
}
