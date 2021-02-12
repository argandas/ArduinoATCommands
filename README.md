[![Arduino CI](https://github.com/argandas/SerialCommand/workflows/Arduino_CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/argandas/SerialCommand/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/v/release/argandas/serialcommand.svg?maxAge=3600)](https://github.com/argandas/SerialCommand/releases)

# SerialCommand

Arduino library for Serial Commands over a serial port

--------------------------------------------------------------------------------

# Introduction

This library is a fork from [scogswell's library](https://github.com/scogswell/ArduinoSerialCommand). 

The main intention of this library was to implement a device able to respond to AT Commands, for example "AT", "AT+RST", that kind of commands you see a lot on modems or SoCs like the ESP8266 or the SIM900.

# Types of AT Commands and responses

Type                | Example      | Description 
------------------- | :----------: | :------------------------------------------------------------------------
Test Command        | AT+<x>=?     | Test the existence of a command and checks it's range of subparameter(s).
Read Command        | AT+<x>?      | This command returns the currently set value of the parameters(s).
Write Command       | AT+<x>=<...> | This command sets the user-definable parameter value(s).
Execution Command   | AT+<x>       | This command execute a function.

# Examples

For examples please check the "examples" folder inside this library.

Or open examples directly from Arduino:
  File->Examples->SerialComman

# Usage

**NOTE:** Every command used should end with a carriage return (<CR>, ASCII '\r', HEX 0xD). Be careful since not all serial port consoles send it as default.

## Commands with no parameters:

Adding a new command requires 2 things, a string and a pointer to void function.
The string is used to compare user input against available commands.
The function will be called when the command is received correctly. 
```c++
mySerialCMD.addCommand("AT", ping);
```

Valid user input:
```
AT<CR>
```

This is well explained on: examples/ping/ping.pde

## Commands with parameters:

The declaration for parametric commands is the same as one without parameters
```c++
mySerialCMD.addCommand("AT+LED", ledHandler);
```

When used these commands should include a '=' char for the first parameter and ',' for trailing parameters.

Valid user input:
```
AT+LED=1<CR>
```

In order to get the parameters we use the method "next" which return a pointer to the next parameter.
NOTE: To know if the parameter is valid you should always check if is NULL.

```c++
param = mySerialCMD.next();
if (param != NULL) // Do something
```

This is well explained on: examples/LedHandler/LedHandler.pde

# Configuration

## SoftwareSerial
This library by default only accepts HardwareSerial connections, if you want to use a SoftwareSerial port you only need to change the value of "SERIAL_CMD_SOFT" on "SerialCommand.h" to 1.

## Debugging
If you are experimenting issues with the commands and don't know why, you may want to debug this library, to do that just set "SERIAL_CMD_DBG_EN" on "SerialCommand.h" to 1.
