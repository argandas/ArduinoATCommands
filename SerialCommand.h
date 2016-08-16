/******************************************************************************* 
SerialCommand - An Arduino library to tokenize and parse commands received over
a serial port. 
Copyright (C) 2011-2013 Steven Cogswell  <steven.cogswell@gmail.com>
http://awtfy.com

Version 20131021A.   

Version History:
May 11 2011 - Initial version
May 13 2011 - Prevent overwriting bounds of SerialCommandCallback[] array in addCommand()
      defaultHandler() for non-matching commands
Mar 2012 - Some const char * changes to make compiler happier about deprecated warnings.  
           Arduino 1.0 compatibility (Arduino.h header) 
Oct 2013 - SerialCommand object can be created using a SoftwareSerial object, for SoftwareSerial
           support.  Requires #include <SoftwareSerial.h> in your sketch even if you don't use 
           a SoftwareSerial port in the project.  sigh.   See Example Sketch for usage. 
Oct 2013 - Conditional compilation for the SoftwareSerial support, in case you really, really
           hate it and want it removed.  


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
***********************************************************************************/
#ifndef SerialCommand_h
#define SerialCommand_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <string.h>

#define SERIAL_CMD_DBG_EN 0 /* Set this value to 1 to enable debugging */
#define SERIAL_CMD_SOFT   0 /* Set this value to 1 to enable SoftwareSerial usage */

#if (SERIAL_CMD_SOFT == 1)
#include <SoftwareSerial.h>
#endif

#define SERIAL_CMD_BUFF_LEN  16 /* Max length for each serial command */
#define SERIAL_CMD_MAX       10 /* Max number of serial commands */

#define SERIAL_CMD_EOL '\r' /* End Of Line character */
#define SERIAL_CMD_DEL ","  /* Token delimeter string for parameters*/
#define SERIAL_CMD_STR "="  /* Token delimeter string for start of parameters */

class SerialCommand : public Print
{
  public:
#if (SERIAL_CMD_SOFT == 1)
    SerialCommand(SoftwareSerial &port);//Constructor
#else
    SerialCommand(HardwareSerial &port);//Constructor
#endif
    void begin(uint32_t baud); /* Execute this function inside Arduino's setup function */
    void loop(void); /* Execute this function inside Arduino's loop function   */
    void addCommand(const char *, void(*)()); /* Add commands to processing dictionary */
    void addDefault(void (*function)()); /* Default function to execute when no match is found */
    char *next(void); /* Return next argument found in command buffer */
    void sendOK(void); /* Send "OK" message trough the serial port */
    void sendERROR(void); /* Send "ERROR" message trough the serial port */
    virtual size_t write(uint8_t); /* Virtual method to match Print class */

  private:
#if (SERIAL_CMD_SOFT == 1)
    SoftwareSerial *serialPort;
#else
    HardwareSerial *serialPort;
#endif
    void clear(void); /* Sets the command buffer to all '\0' (nulls) */
    char inChar; /* A character read from the serial stream */
    char buffer[SERIAL_CMD_BUFF_LEN]; /* Buffer of stored characters while waiting for terminator character */
    char* pBuff; /* Pointer to buffer, used to store data in the buffer */
    char* token; /* Returned token from the command buffer as returned by strtok_r */
    char* last; /* State variable used by strtok_r during processing */
    typedef struct _callback {
      char command[SERIAL_CMD_BUFF_LEN];
      void (*function)();
    } SerialCommandCallback; /* Data structure to hold Command/Handler function key-value pairs */
    uint8_t numCommand; /* Number of available commands registered by new() */
    SerialCommandCallback CommandList[SERIAL_CMD_MAX]; /* Actual definition for command/handler array */
    void (*defaultHandler)(); /* Pointer to the default handler function */
};

#endif //SerialCommand_h