[![Arduino CI](https://github.com/argandas/SerialCommand/workflows/Arduino_CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/argandas/SerialCommand/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/v/release/argandas/serialcommand)](https://github.com/argandas/SerialCommand/releases)

# SerialCommand

Arduino library for Serial Commands over a serial port

--------------------------------------------------------------------------------

# Introduction

This library is a fork from [scogswell's library](https://github.com/scogswell/ArduinoSerialCommand). 

The main intention of this library is to implement a device able to respond to serial Commands, for example AT Commands ("AT", "AT+RST", etc), that kind of commands you see a lot on modems or SoCs like the ESP8266 or the SIM900.

# Usage

## Command Syntax

Type                | Syntax             | Description 
------------------- | :----------------: | :------------------------------------------------------------------------
Test Command        | <cmd>=?            | Call the test callback defined for the command (if any).
Read Command        | <cmd>?             | Call the reaad callback defined for the command (if any).
Write Command       | <cmd>=<p1>,<p2>    | Call the write callback defined for the command (if any).
Execution Command   | <cmd>              | Call the execute callback defined for the command (if any).

## Write Command Example

First, you need to setup the Serial Interface to use by the library, assign the serial port to your SerialCommand class instance, and create a new command (e.g. "LED"):
```c++
SerialCommand mySerial;

void setup() 
{
    Serial.being(9600);
    mySerial.begin(Serial);
    mySerial.addWriteCommand("LED", writeLED);
}
```

This will create the command "LED", and will execute the function called "writeLED" everytime a message with the format "LED=X,Y" is received in the "Serial" port.

**NOTE:** It is expected that every line received in the serial interface contains the End Of Line characters "\r\n" (<CR><LF>).

The function "writeLED" shall be void type and receive no parameters, the write parameters will be obtained trough the "next" method of the SerialCommand class.

**NOTE:** Only the write command use the "next" method, any other command type does not expect parameters.

```c++
void writeLED() 
{
    char *arg;
    arg = mySerialCMD.next(); /* Get the next argument from the SerialCommand object buffer */
    if (*arg == '1')
    {
        digitalWrite(13, HIGH);
        Serial.println("+LED=OFF");
        Serial.println("OK");
    }
    else if (*arg == '0') 
    {
        digitalWrite(13, LOW);
        Serial.println("+LED=ON");    
        Serial.println("OK");
    }
    else
    {
        Serial.println("ERROR");
    }
}
```


## Other examples

For more examples please check the "examples" folder inside this library.

# Serial Interfaces

This library is intended to work with any Stream child class like "HardwareSerial" (Most commonly used serial interface), "SoftwareSerial", and "Serial_" (USB CDC Serial).

# Debug
If you are experimenting issues with the commands and don't know why, you may want to debug this library, to do that just set "SERIAL_CMD_DBG_EN" on "SerialCommand.h" to 1.
