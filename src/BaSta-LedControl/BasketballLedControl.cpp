#include "BasketballLedControl.h"

BasketballLedControl::BasketballLedControl(int controlPin, long timeout)
{
	_pin = controlPin;
  _timeout = timeout;

  pinMode(_pin, OUTPUT);
  setState(false);
};

bool BasketballLedControl::getState()
{
  return _state;
}

void BasketballLedControl::setState(bool state)
{
  _state = state;
  digitalWrite(_pin, _state ? LOW : HIGH);
  _stamp = _state ? millis() : 0;
}

void BasketballLedControl::checkTimeout()
{
  // If the LED was active for the designated time, disable it
  if (_stamp != 0 && millis() - _stamp >= _timeout)
    setState(false);
}