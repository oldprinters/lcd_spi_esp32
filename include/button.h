#ifndef BUTTON_H
#define BUTTON_H
#include "Timer.h"
enum class PRESS_TYPE { NONE, ONE, DOUBLE, LONG };

class Button : public Timer {
  int pin;
  uint32_t tTime;
  uint32_t longPress{2000};
  Timer postPress;
  bool firstPress{};  //первое нажатие
  void (*fDouble)();
  void (*fLong)();
  void (*fShort)();
public:
  Button(int p, void (*f)(), void (*f1)(), void (*f2)());
  void press_button();
  void cycle();
};
#endif
