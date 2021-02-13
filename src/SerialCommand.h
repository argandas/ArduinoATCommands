#ifndef SerialCommand_h
#define SerialCommand_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <string.h>

#define SERIAL_CMD_DBG_EN 0    /* Set this value to 1 to enable debugging */
#define SERIAL_CMD_BUFF_LEN 64 /* Max length for each serial command */

/* Data structure to hold Command/Handler function key-value pairs */
typedef struct
{
    char command[SERIAL_CMD_BUFF_LEN];
    void (*test)();
    void (*read)();
    void (*write)();
    void (*execute)();
} serialCommandCallback;

/*
 * Token delimeters (setup '=', query '?', separator ',') 
 */
const char delimiters[] = "=,?\r\n";

/* 
 * End Of Line: <CR><LF>
 * <CR> = <Carriage Return, 0x0D, 13, '\r'>
 * <LF> = <Line Feed, 0x0A, 10, '\n'>
 */
const char EOL[] = "\r\n";

class SerialCommand : public Stream
{
public:
    /**
         * Class constructor
         */
    SerialCommand(); //Constructor

    /**
     * Start connection to serial port
     *
     * @param serialPort - Serial port to listen for commands
     * @param baud - Baud rate
     */
    void begin(Stream &serialPort);

    /**
     * Execute this function inside Arduino's loop function.
     */
    void loop(void);

    /**
     * Add a new command
     *
     * @param cmd - Command to listen
     * @param test - Test command callback
     * @param read - Read command callback
     * @param write - Write command callback
     * @param execute - Execute command callback
     */
    void addCommand(char *cmd, void (*test)(), void (*read)(), void (*write)(), void (*execute)());

    /**
     * Add a read-only command
     *
     * @param cmd - Command to listen
     * @param callback - Read command callback
     */
    void addTestCommand(char *cmd, void (*callback)())
    {
        addCommand(cmd, callback, NULL, NULL, NULL);
    };

    /**
     * Add a read-only command
     *
     * @param cmd - Command to listen
     * @param callback - Read command callback
     */
    void addReadCommand(char *cmd, void (*callback)())
    {
        addCommand(cmd, NULL, callback, NULL, NULL);
    };

    /**
     * Add a write-only command
     *
     * @param cmd - Command to listen
     * @param callback - Write command callback
     */
    void addWriteCommand(char *cmd, void (*callback)())
    {
        addCommand(cmd, NULL, NULL, callback, NULL);
    };

    /**
     * Add a execute-only command
     *
     * @param cmd - Command to listen
     * @param callback - Execute command callback
     */
    void addExecuteCommand(char *cmd, void (*callback)())
    {
        addCommand(cmd, NULL, NULL, NULL, callback);
    };

    /**
     * Default function to execute when no match is found
     *
     * @param callback - Function to execute when command is received
     */
    void addError(void (*callback)());

    /*  Return next argument found in command buffer */
    char* next(void);

    /* 
     * Virtual methods to match Stream class 
     */
    size_t write(uint8_t);
    int available();
    int read();
    int peek();
    void flush();

private:
    /* Setup serial port */
    void setup(unsigned long baud);

    /* Sets the command buffer to all '\0' (nulls) */
    void clear(void);

    /* Send error message and clear buffer */
    void error();

    /* Process buffer */
    void bufferHandler(char c);

    /* Check for command instances and handle callbacks and queries */
    bool commandHandler(void);

    /* User defined error handler */
    void (*userErrorHandler)();

    /* Serial Port handler */
    Stream * _serial;

    /* Actual definition for command/handler array */
    serialCommandCallback *commandList;

    /* Buffer of stored characters while waiting for terminator character */
    char buffer[SERIAL_CMD_BUFF_LEN];

    /* Pointer to buffer, used to store data in the buffer */
    char *pBuff;

    /* State variable used by strtok_r during processing */
    char *last;

    /* Number of available commands registered by new() */
    uint8_t commandCount;
};

#endif //SerialCommand_h
