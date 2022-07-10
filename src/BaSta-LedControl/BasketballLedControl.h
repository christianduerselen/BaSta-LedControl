#ifndef BasketballLedControl_h
#define BasketballLedControl_h

#include "Arduino.h"

class BasketballLedControl
{
  public:
    BasketballLedControl(int controlPin, long timeout);
    bool getState();
    void setState(bool state);
    void checkTimeout();
  private:
    int _pin;
    long _timeout;
    bool _state = false;
    unsigned long _stamp = 0;
};

#endif