# MidiBanger
A Raspberry Pico MIDI player that plays physical instruments via servos.

Currently supports both percussive (mallet-type) and pressed (key-type) instruments.

The initial version supports up to 16 directly-connected servos, later versions will
support I2C servo controllers allowing hundreds of servos to be used.

## Hardware Requirements
* Raspberry Pico or compatible
* Servo per note eg. SG90 9g Micro Servo
* Optional - Micro speaker - handy for debugging and setup
* Optional - 5V PSU for servos, if you think you need it

## Configuration
See the comments in `src/config.h` for details.

The critical configuration item is the `NOTEPIN_INITIALISER`.  Leave its first line alone, 
then replace the contents with a maximum of sixteen [MIDI note number] = GPIO_PIN pairs.

If you are connecting up several MidiBangers, I suggest you use a different
number for each one at the end of the `USB_PRODUCT` string.  It will make your
life a lot simpler!

## Building
```
mkdir build
cd build
cmake ..
make
```
