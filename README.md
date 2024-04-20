# BaSta-LedControl

BaSta-LedControl is an Arduino program used to control the red and yellow basket LED strips from an in coming Stramatel protocol. The red strips are enabled when the game clock runs to zero and the yellow strips are enabled when the shot clock runs to zero.

## Usage

BaSta-LedControl is intended to be used on an Arduino Uno board. The Stramatel basketball protocol is received through serial input from a MAX485 board. Two digital output pins control the LED state e.g. via switch relays.

To keep the USB serial interface enabled, the protocol is received through a separate Rx-pin and the SoftwareSerial plugin. Following is the pin usage for an Arduino Uno board:

- 2 - gpio output #1 (yellow stripe)
- 3 - gpio output #2 (red stripe)
- 4 - Receiver Output (RO) pin of MAX485 (used as RX-pin for SoftwareSerial)
- 5 - Receiver Enable (RE) pin of MAX485 (set to LOW to only enable receiving)
- 6 - Driver Enable (DE) pin of MAX485 (set to LOW to only enable receiving)
- 7 - Driver Inpit (DI) pin of MAX485 (used as TX-pin for SoftwareSerial)
