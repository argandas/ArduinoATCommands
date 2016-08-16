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
    uint8_t commandCount; /* Number of available commands registered by new() */
    SerialCommandCallback  *commandList; /* Actual definition for command/handler array */
    void (*defaultHandler)(); /* Pointer to the default handler function */
};

#endif //SerialCommand_h