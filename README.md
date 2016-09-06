# dash-kitten
Arduino-powered digital dashboards... For kittens!

This is designed to receive data over CAN BUS from a MegaSquirt and present it
on a Nextion LCD display.

It also allows you to:
* use the Arduino's analog-in pins as extra analog inputs for the MegaSquirt
* Drive warning LEDs / idiot lights from the Arduino

## Requirements
This software is designed for:
* [Arduino Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno)
* [Seeed Studio CAN-BUS shield v1.2](http://www.seeedstudio.com/wiki/CAN-BUS_Shield)
* [Nextion NX4024T032_011R display](http://wiki.iteadstudio.com/NX4024T032)
* [Arduino IDE 1.6.6](https://www.arduino.cc/en/Main/Software)
* [MCP_CAN_lib](https://github.com/coryjfowler/MCP_CAN_lib/tree/d23d62b1ca21b33c6a6b8d0d134a06033b139240)
* [MegaSquirt III, firmware 1.4.0](http://www.msextra.com/)

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

Warning LEDs / idiot lights can be connected to:
* Warning: Digital pin 10
* Error: Digital pin 11

## MegaSquirt setup

To send data from the MegaSquirt to the DashKitten, go to CAN-bus/Testmodes,
then CAN Realtime Data Broadcasting 1/2/3/4.  We're using these settings:

| Frequency | IDs                    |
| --------- | ---------------------- |
| 50Hz      | 00, 01, 02, 03, 18     |
| 20Hz      | 04, 13, 17, 42, 51, 52 |
| Off       | Everything else        |

To send analog measurements from the DashKitten to the MegaSquirt, go to
CAN-bus/Testmodes, then CAN Receiving.  Configure the channels to be:

| Local Variable / Channel | Identifier (decimal) | Offset | Size | Multiply | Divide | Add |
| ------------------------ | -------------------- | ------ | ---- | -------- | ------ | --- |
| CAN ADC01                | 32                   | 0      | B2U  | 1        | 1      | 0   |
| CAN ADC02                | 32                   | 2      | B2U  | 1        | 1      | 0   |
| CAN ADC03                | 32                   | 4      | B2U  | 1        | 1      | 0   |
| CAN ADC04                | 32                   | 6      | B2U  | 1        | 1      | 0   |
| CAN ADC05                | 33                   | 0      | B2U  | 1        | 1      | 0   |
| CAN ADC06                | 33                   | 2      | B2U  | 1        | 1      | 0   |
| CAN ADC07                | 33                   | 4      | B2U  | 1        | 1      | 0   |
| CAN ADC08                | 33                   | 6      | B2U  | 1        | 1      | 0   |

There appears to be a bug in MegaSquirt v1.4.0: If the sensor's value exceeds
3276 it will be corrupted.  This appears to be a bug in the MS firmware where
values are being multiplied by 10 internally (probably fixed-decimal), and
being handled as signed values even though they shouldn't.  The Arduino Uno's
ADC will only output 0-1023 so if you use Multiply=1 Divide=1 it will be fine.
Multiply=3 Divide=1 will still be fine (max 3069), but Multiply=4 Divide=1 will
cause an overflow into negative numbers.  I haven't tried, but Some other
models of Arduino such as the Zero have a 12-bit ADC and may have problems even
with Multiply=1.

Next go to Advanced Engine, then Generic Sensor Inputs, and assign the CAN ADCs
to generic sensors:

| Sensor | Source    | Field Name | Transformation |
| ------ | --------- | ---------- | -------------- |
| 09     | CAN ADC01 | Sensor 09  | Raw            |
| 10     | CAN ADC02 | Sensor 10  | Raw            |
| 11     | CAN ADC03 | Sensor 11  | Raw            |
| 12     | CAN ADC04 | Sensor 12  | Raw            |
| 13     | CAN ADC05 | Sensor 13  | Raw            |
| 14     | CAN ADC06 | Sensor 14  | Raw            |
| 15     | CAN ADC07 | Sensor 15  | Raw            |
| 16     | CAN ADC08 | Sensor 16  | Raw            |

You can configure the Field Name to be anything, such as "Oil Pressure".  This
is what will show up in the logs.

## License
GPL v2
