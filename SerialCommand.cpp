#include "SerialCommand.h"

static void printHex(Stream &port, uint8_t *data, uint8_t length);
static void printHex(Stream &port, uint16_t *data, uint8_t length);

void printHex(Stream &port, uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
    for (int i = 0; i < length; i++)
    {
        // ort.print("0x");
        if (data[i] < 0x10)
        {
            port.print("0");
        }
        port.print(data[i], HEX);
        port.print(" ");
    }
}

void printHex(Stream &port, uint16_t *data, uint8_t length) // prints 16-bit data in hex with leading zeroes
{
    for (int i = 0; i < length; i++)
    {
        // port.print("0x");
        uint8_t MSB = byte(data[i] >> 8);
        uint8_t LSB = byte(data[i]);
        if (MSB < 0x10)
        {
            port.print("0");
        }
        port.print(MSB, HEX);
        if (LSB < 0x10)
        {
            port.print("0");
        }
        port.print(LSB, HEX);
        port.print(" ");
    }
}

SerialCommand::SerialCommand() :
    commandCount(0), userErrorHandler(NULL)
{
    clear();
}

void SerialCommand::begin(SoftwareSerial &serialPort, uint32_t baud)
{
    /* Save Serial Port configurations */
    _serialPortHandler.isSoftSerial = true;
    _serialPortHandler._soft = &serialPort;
    _serialPortHandler._hard = NULL;
    setup(baud);
}

void SerialCommand::begin(HardwareSerial &serialPort, uint32_t baud)
{
    /* Save Serial Port configurations */
    _serialPortHandler.isSoftSerial = false;
    _serialPortHandler._soft = NULL;
    _serialPortHandler._hard = &serialPort;
    setup(baud);
}

void SerialCommand::setup(uint32_t baud)
{
    /* Begin Serial Port */
    if (_serialPortHandler.isSoftSerial)
    {
        if (NULL != _serialPortHandler._soft)
        {
            _serialPortHandler._soft->begin(baud);
        }
    }
    else
    {
        if (NULL != _serialPortHandler._hard)
        {
            _serialPortHandler._hard->begin(baud);
        }
    }
}

// This checks the Serial stream for characters, and assembles them into a buffer.  
// When the terminator character (defined by EOL constant) is seen, it starts parsing the 
// buffer for a prefix command, and calls handlers setup by addCommand() method
void SerialCommand::loop(void)
{
    char c;
    while (available() > 0)
    {
        c = read();
        bufferHandler(c);
    }
}

/* Clear buffer */
void SerialCommand::clear(void)
{
    memset(buffer, 0, SERIAL_CMD_BUFF_LEN);
    pBuff = buffer;
}

/* 
 * Send error response
 * NOTE: Will execute user defined callback (defined using addDefault method),
 * if no user defined callback it will send the ERROR message (sendERROR method).
 */
void SerialCommand::error(void)
{
    if (NULL != userErrorHandler)
    {
        (*userErrorHandler)();
    }
    else
    {
        sendERROR();
    }
    clear(); /* Clear buffer */
}

// Retrieve the next token ("word" or "argument") from the Command buffer.  
// returns a NULL if no more tokens exist.   
char* SerialCommand::next(void)
{
    return strtok_r(NULL, delimiters, &last);
}

void SerialCommand::bufferHandler(char c)
{
    int len;
    char* lastChars = NULL;

    print(c); /* ECHO received command */

    if ((pBuff - buffer) > (SERIAL_CMD_BUFF_LEN - 2)) /* Check buffer overflow */
    {
        error(); /* Send ERROR, Buffer overflow */
    }

    *pBuff++ = c; /* Put character into buffer */
    *pBuff = '\0'; /* Always null terminate strings */

    if ((pBuff - buffer) > 2) /* Check buffer length */
    {
        /* Get EOL */
        len = strlen(buffer);
        lastChars = buffer + len - 2;

        /* Compare last chars to EOL */
        if (0 == strcmp(lastChars, EOL))
        {

            // *lastChars = '\0'; /* Replace EOL with NULL terminator */

#if (SERIAL_CMD_DBG_EN == 1)
            print("Received: ");
            println(buffer);
#endif

            if (commandHandler())
            {
                clear();
            }
            else
            {
                error();
            }
        }
    }
}

/* Return true if match was found */
bool SerialCommand::commandHandler(void)
{
    int i;
    bool ret = false;
    boolean matched = false;
    char* token;

    char userInput[SERIAL_CMD_BUFF_LEN];

    memcpy(userInput, buffer, SERIAL_CMD_BUFF_LEN);

    /* Remove EOL */
    for (int i = 0; i < SERIAL_CMD_BUFF_LEN; i++)
    {
        if ((userInput[i] == '\r') || (userInput[i] == '\n'))
        {
            userInput[i] = '\0';
        }
    }

    /* Search for command at start of buffer */
    token = strtok_r(buffer, delimiters, &last);

    if (NULL != token)
    {

#if (SERIAL_CMD_DBG_EN == 1)
        print("User input: \"");
        print(token);
        print("\" (");
        printHex(Serial, (uint8_t*) token, SERIAL_CMD_BUFF_LEN);
        println(")");
#endif

        for (i = 0; (i < commandCount); i++)
        {

#if (SERIAL_CMD_DBG_EN == 1)
            print("Case: \"");
            print(commandList[i].command);
            print("\" ");
#endif

            /* Compare the token against the list of known commands */
            if (0 == strncmp(token, commandList[i].command, strlen(commandList[i].command)))
            {

#if (SERIAL_CMD_DBG_EN == 1)
                println("- Match Found!");
                print("Next char: \"");
                print(*(char *) (buffer + strlen(token)));
                print("\", ");
                print(*(char *) (buffer + strlen(token)), HEX);
                print(", (");
                printHex(Serial, ((uint8_t *) buffer + strlen(token)), SERIAL_CMD_BUFF_LEN);
                println(")");
#endif

                /* Check for query command */
                // TODO: Fix test command, it jumps to execute if NULL
                if ((0 == strncmp((char *) (buffer + strlen(token)), "=?", 2)) && (NULL != *commandList[i].test))
                {
                    /* Run test callback */
                    (*commandList[i].test)();
                }
                else if (('?' == *(char *) (buffer + strlen(token))) && (NULL != *commandList[i].read))
                {
                    /* Run read callback */
                    (*commandList[i].read)();
                }
                else if (('=' == *(char *) (buffer + strlen(token))) && (NULL != *commandList[i].write))
                {
                    /* Run write callback */
                    (*commandList[i].write)();
                }
                else if (NULL != *commandList[i].execute)
                {
                    /* Run execute callback */
                    (*commandList[i].execute)();
                }
                else
                {
                    println("INVALID");
                    ret = false;
                    break;
                }

                ret = true;
                break;
            }

#if (SERIAL_CMD_DBG_EN == 1)
            else
            {
                println("- Not a match!");
            }
#endif

        }
    }

    return ret;
}

// Adds a "command" and a handler function to the list of available commands.  
// This is used for matching a found token in the buffer, and gives the pointer
// to the handler function to deal with it. 
void SerialCommand::addCommand(const char* cmd, void(*test)(), void(*read)(), void(*write)(), void(*execute)())
{

#if (SERIAL_CMD_DBG_EN == 1)
    print("[");
    print(commandCount);
    print("] New Command: ");
    println(cmd);
#endif

    commandList = (serialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(serialCommandCallback));
    strncpy(commandList[commandCount].command, cmd, SERIAL_CMD_BUFF_LEN);
    commandList[commandCount].test = test;
    commandList[commandCount].read = read;
    commandList[commandCount].write = write;
    commandList[commandCount].execute = execute;
    commandCount++;
}

/* Optional user-defined function to call when an error occurs, default is NULL */
void SerialCommand::addError(void(*callback)())
{
    userErrorHandler = callback;
}

size_t SerialCommand::write(uint8_t character)
{
    if (_serialPortHandler.isSoftSerial)
    {
        if (NULL != _serialPortHandler._soft)
        {
            _serialPortHandler._soft->write(character);
        }
    }
    else
    {
        if (NULL != _serialPortHandler._hard)
        {
            _serialPortHandler._hard->write(character);
        }
    }
}

int SerialCommand::available()
{
    if (_serialPortHandler.isSoftSerial)
    {
        if (NULL != _serialPortHandler._soft)
        {
            return _serialPortHandler._soft->available();
        }
    }
    else
    {
        if (NULL != _serialPortHandler._hard)
        {
            return _serialPortHandler._hard->available();
        }
    }
}

int SerialCommand::read()
{
    if (_serialPortHandler.isSoftSerial)
    {
        if (NULL != _serialPortHandler._soft)
        {
            return _serialPortHandler._soft->read();
        }
    }
    else
    {
        if (NULL != _serialPortHandler._hard)
        {
            return _serialPortHandler._hard->read();
        }
    }
}

int SerialCommand::peek()
{
    if (_serialPortHandler.isSoftSerial)
    {
        if (NULL != _serialPortHandler._soft)
        {
            return _serialPortHandler._soft->peek();
        }
    }
    else
    {
        if (NULL != _serialPortHandler._hard)
        {
            return _serialPortHandler._hard->peek();
        }
    }
}

void SerialCommand::flush()
{
    if (_serialPortHandler.isSoftSerial)
    {
        if (NULL != _serialPortHandler._soft)
        {
            _serialPortHandler._soft->flush();
        }
    }
    else
    {
        if (NULL != _serialPortHandler._hard)
        {
            _serialPortHandler._hard->flush();
        }
    }
}

void SerialCommand::sendOK(void)
{
    println("\r\nOK\r\n");
}

void SerialCommand::sendERROR(void)
{
    println("\r\nERROR\r\n");
}
