#ifndef StramatelProtocolParser_h
#define StramatelProtocolParser_h

#include "Troolean.h"

const int MessageSize = 54;

class StramatelProtocolParser
{
  public:
    StramatelProtocolParser();
    void push(byte value);
	  bool isValidMessage();
	  Troolean isGameClockZero();	
    Troolean isShotClockZero();	
  private:
    byte _message[MessageSize];
    int _messagePointer = 0;
    Troolean isValueZero(int startIndex, int endIndex);
};

#endif