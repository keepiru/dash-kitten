# dash-kitten
Arduino-powered digital dashboards... For kittens!

## Requirements
This software is designed for:
* [Arduino Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno)
* [Seeed Studio CAN-BUS shield v1.2](http://www.seeedstudio.com/wiki/CAN-BUS_Shield)
* [Nextion NX4024T032_011R display](http://wiki.iteadstudio.com/NX4024T032)
* [Arduino IDE 1.6.6](https://www.arduino.cc/en/Main/Software)
* [MCP_CAN_lib](https://github.com/coryjfowler/MCP_CAN_lib/tree/d23d62b1ca21b33c6a6b8d0d134a06033b139240)

These exact versions are known good, but other variants probably work.

## Software setup
This depends on MCP_CAN_lib to communicate with the CAN BUS chip.  To install
it on Linux:

    cd ~/Arduino
    git clone https://github.com/coryjfowler/MCP_CAN_lib.git
    cd MCP_CAN_lib
    git checkout eecf231fb0b1e594086f6212dfd6b643eca1b8d9

Now start the Arduino IDE and open dash-kitten.ino; click Upload and it should
build and run.

## Nextion Display setup
Plan A:
* Connect the display to a TTL-serial cable
* Open dash-kitten.hmi in the Nextion GUI
* click upload
* If the transfer fails your display may be bricked.  Proceed to Plan B.

Plan B:
* Copy dash-kitten.tft as the only file on a FAT32-formatted micro-SD card
* Click it into the SD slot
* Power on
* Find out that the Nextion display is really picky about which SD cards work
* Find another micro-SD card
* GOTO 10

## Hardware setup
Stack the CAN-BUS shield on the Arduino, and connect the CAN-high and CAN-low
pins to your MS3.

Connect the Nextion display as follows:
* Black - GND
* Red - 5v
* Yellow - Digital pin 4
* Blue - Digital pin 5

## License
GPL v2
