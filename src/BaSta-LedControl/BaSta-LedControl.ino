#include <SoftwareSerial.h>
#include "Troolean.h"

const int SerialInputPin = 2;
const int SerialOutputPin = 5;
const int GpioOutput1Pin = 3;
const int GpioOutput2Pin = 4;
const int MessageSize = 54;
const unsigned long flashDuration = 3000;

SoftwareSerial serialInput(SerialInputPin, SerialOutputPin);
byte message[MessageSize];
int messagePointer = 0;
bool isInitialized = false;

Troolean gameClockZeroState(unknown);
Troolean gameClockZeroStateReceived(unknown);
unsigned long gameClockTimer = 0;
bool shotClockHornState = false;

//////////////////////////////////////////////////
////////// SETUP & MAIN LOOP

void setup()
{
  // Configure GPIOs as outputs
  pinMode(GpioOutput1Pin, OUTPUT);
  pinMode(GpioOutput2Pin, OUTPUT);

  // Configure serial receive (through SerialInput)
  serialInput.begin(19200);
}

void loop()
{
  // Initialization method will only be performed once
  initialize();

  // Receive method receives data from serial input
  receive();

  // Parse method checks the received data
  parse();

  // Control method checks the current state and controls the LEDs accordingly
  control();
}

//////////////////////////////////////////////////
////////// INITIALIZE

void initialize()
{
  if (isInitialized)
    return;
  
  isInitialized = true;
  setRedLed(false);
  setYellowLed(false);

  delay(1000);
  
  setRedLed(true);
  setYellowLed(false);
  
  delay(2000);
  
  setRedLed(false);
  setYellowLed(true);
  
  delay(2000);
  
  setRedLed(false);
  setYellowLed(false);
}

//////////////////////////////////////////////////
////////// RECEIVE

void receive()
{
  while (serialInput.available() > 0)
  {
    // Read next byte from input
    byte value = serialInput.read();

    // Reset pointer if overflow would happen
    if (messagePointer >= MessageSize)
      messagePointer = 0;

    // 0xF8 is the start code if a new message, therefore reset message pointer
    if (value == 0xF8)
      messagePointer = 0;

    // Write value and advance pointer
    message[messagePointer] = value;
    messagePointer++;
  }
}

//////////////////////////////////////////////////
////////// PARSE

void parse()
{
  // A complete message must have been received
  if (messagePointer != MessageSize)
    return;

  // A message must start with 0xF8
  if (message[0] != 0xF8)
    return;

  // A message must follow with 0x33, 0x37 or 0x38
  if (message[1] != 0x33 && message[1] != 0x37 && message[1] != 0x38)
    return;


  // Parse game clock (bytes 4 to 7)
  gameClockZeroStateReceived = unknown;
  for (int i = 4; i < 8; i++)
  {
    byte value = message[i];

    // Ignore spaces (0x20) and NUL bytes (0x00)
    if (value == 0x20 || value == 0x00)
      continue;

    // 0x30 (0) and 0x40 (.0) are allowed
    if (value == 0x30 || value == 0x40)
    {
      gameClockZeroStateReceived = true;
      continue;
    }

    // Other values mean non-zero value
    gameClockZeroStateReceived = false;
    break;
  }


  // Parse shot clock horn (byte 50)
  shotClockHornState = message[50] == 0x31;
}

//////////////////////////////////////////////////
////////// CONTROL

void control()
{
  // If we transition from non-zero to zero game clock with horn set red LED
  if (gameClockZeroState == false && gameClockZeroStateReceived == true)
  {
    setRedLed(true);
    gameClockTimer = millis();
  }

  // If the red LED was active for the designated time, disable it
  if (gameClockTimer != 0 && millis() - gameClockTimer >= flashDuration)
  {
    setRedLed(false);
    gameClockTimer = 0;
  }

  // Remember the received state
  gameClockZeroState = gameClockZeroStateReceived;


  // The yellow shot clock LED is directly controlled by the horn state
  setYellowLed(shotClockHornState);
}

//////////////////////////////////////////////////
////////// HELPER

void setRedLed(bool state)
{
  setLed(GpioOutput1Pin, state);
}

void setYellowLed(bool state)
{
  setLed(GpioOutput2Pin, state);
}

void setLed(int pin, bool state)
{
  digitalWrite(pin, state ? LOW : HIGH);
}
