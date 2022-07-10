#include <SoftwareSerial.h>
#include "Troolean.h"

const int YellowLedOutputPin = 2;
const int RedLedOutputPin = 3;
const int ReceiverOutputPin = 4;
const int ReceiverEnablePin = 5;
const int DriverEnablePin = 6;
const int DriverInputPin = 7;
const int MessageSize = 54;
const unsigned long flashDuration = 3000;

SoftwareSerial serialInput(ReceiverOutputPin, DriverInputPin);
byte message[MessageSize];
int messagePointer = 0;
bool isInitialized = false;

Troolean gameClockZeroState(unknown);
Troolean gameClockZeroStateReceived(unknown);
unsigned long gameClockTimer = 0;
bool gameClockLedState = false;
Troolean shotClockZeroState(unknown);
Troolean shotClockZeroStateReceived(unknown);
unsigned long shotClockTimer = 0;
bool shotClockLedState = false;

//////////////////////////////////////////////////
////////// SETUP & MAIN LOOP

void setup()
{
  // Configure GPIOs as outputs
  pinMode(YellowLedOutputPin, OUTPUT);
  pinMode(RedLedOutputPin, OUTPUT);

  // Configure the MAX485 DE & RE GPIO pins as LOW to only enable receiving
  pinMode(ReceiverEnablePin, OUTPUT);
  digitalWrite(ReceiverEnablePin, LOW);
  pinMode(DriverEnablePin, OUTPUT);
  digitalWrite(DriverEnablePin, LOW);

  // Configure serial receive (through SerialInput)
  serialInput.begin(19200);

  Serial.begin(19200);
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

    Serial.write(value);

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

    // 0x31 (1) to 0x39 (9) and 0x41 (.1) to 0x49 (.9) mean value other than zero
    if ((value > 0x30 && value < 0x3A) || (value > 0x40 && value < 0x4A))
    {
      gameClockZeroStateReceived = false;
      break;
    }
    
    // Other values mean unknown state
    gameClockZeroStateReceived = unknown;
    break;
  }

  // Parse shot clock (bytes 48 to 49)
  shotClockZeroStateReceived = unknown;
  for (int i = 48; i < 50; i++)
  {
    byte value = message[i];

    // Ignore spaces (0x20) and NUL bytes (0x00)
    if (value == 0x20 || value == 0x00)
      continue;

    // 0x30 (0) and 0x40 (.0) are allowed
    if (value == 0x30 || value == 0x40)
    {
      shotClockZeroStateReceived = true;
      continue;
    }

    // 0x31 (1) to 0x39 (9) and 0x41 (.1) to 0x49 (.9) mean value other than zero
    if ((value > 0x30 && value < 0x3A) || (value > 0x40 && value < 0x4A))
    {
      shotClockZeroStateReceived = false;
      break;
    }
    
    // Other values mean unknown state
    shotClockZeroStateReceived = unknown;
    break;
  }
}

//////////////////////////////////////////////////
////////// CONTROL

void control()
{
  // If we transition from non-zero to zero game clock unset yellow and set red LED
  if (gameClockZeroState == false && gameClockZeroStateReceived == true)
  {
    setYellowLed(false);
    shotClockTimer = 0;
    shotClockZeroState = shotClockZeroStateReceived;
    
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

  // Make sure that the yellow LED is not controlled while the red one is active
  if (gameClockLedState == true)
  {
    return;
  }

  // If we transition from non-zero to zero shot clock set yellow LED
  if (shotClockZeroState == false && shotClockZeroStateReceived == true)
  {
    setYellowLed(true);
    shotClockTimer = millis();
  }

  // If the yellow LED was active for the designated time, disable it
  if (shotClockTimer != 0 && millis() - shotClockTimer >= flashDuration)
  {
    setYellowLed(false);
    shotClockTimer = 0;
  }

  // Remember the received state
  shotClockZeroState = shotClockZeroStateReceived;
}

//////////////////////////////////////////////////
////////// HELPER

void setRedLed(bool state)
{
  gameClockLedState = state;
  setLed(RedLedOutputPin, state);
}

void setYellowLed(bool state)
{
  shotClockLedState = state;
  setLed(YellowLedOutputPin, state);
}

void setLed(int pin, bool state)
{
  digitalWrite(pin, state ? LOW : HIGH);
}
