# BaSta-LedControl

BaSta-LedControl is an Arduino program used to control the red and yellow basket LED strips from an in coming Stramatel protocol. The red strips are enabled when the game clock runs to zero and the yellow strips are enabled when the shot clock runs to zero.

## Usage

BaSta-LedControl is intended to be used on an Arduino Uno board. The Stramatel basketball protocol is received through serial input. Two digital output pins control the LED state e.g. via switch relays.

To keep the USB serial interface enabled, the protocol is received through a separate Rx-pin and the SoftwareSerial plugin. Following is the pin usage for an Arduino Uno board:

- 2 - Rx serial input (SoftwareSerial)
- 3 - gpio output #1 (red stripe)
- 4 - gpio output #2 (yellow stripe)
- 5 - Tx serial input (SoftwareSerial - not used)