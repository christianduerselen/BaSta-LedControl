#include "StramatelProtocolParser.h"

StramatelProtocolParser::StramatelProtocolParser()
{
};

void StramatelProtocolParser::push(byte value)
{
  // Reset pointer if overflow would happen
  if (_messagePointer >= MessageSize)
    _messagePointer = 0;

  // 0xF8 is the start code if a new message, therefore reset message pointer
  if (value == 0xF8)
    _messagePointer = 0;

  // Write value and advance pointer
  _message[_messagePointer] = value;
  _messagePointer++;
}

bool StramatelProtocolParser::isValidMessage()
{
  // A complete message must have been received
  if (_messagePointer != MessageSize)
    return false;

  // A message must start with 0xF8
  if (_message[0] != 0xF8)
    return false;

  // A message must follow with 0x33, 0x37 or 0x38
  if (_message[1] != 0x33 && _message[1] != 0x37 && _message[1] != 0x38)
    return false;

  return true;	
}

Troolean StramatelProtocolParser::isGameClockZero()
{
  return isValueZero(4, 7);
}

Troolean StramatelProtocolParser::isShotClockZero()
{
  return isValueZero(48, 49);
}

Troolean StramatelProtocolParser::isValueZero(int startIndex, int endIndex)
{
  bool zeroAvailable = false;

  for (int i = startIndex; i <= endIndex; i++)
  {
    byte value = _message[i];

    // Ignore spaces (0x20) and NUL bytes (0x00)
    if (value == 0x20 || value == 0x00)
      continue;

    // 0x30 (0) and 0x40 (.0) are allowed
    if (value == 0x30 || value == 0x40)
    {
      zeroAvailable = true;
      continue;
    }

    // 0x31 (1) to 0x39 (9) and 0x41 (.1) to 0x49 (.9) mean value other than zero
    if ((value > 0x30 && value < 0x3A) || (value > 0x40 && value < 0x4A))
      return Troolean(false);
    
    // Other values mean unknown state
    return Troolean(unknown);    
  }

  return Troolean(zeroAvailable ? true : unknown);
}