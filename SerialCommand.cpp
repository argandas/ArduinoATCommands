#include "SerialCommand.h"

#if (SERIAL_CMD_SOFT == 1)
/* Constructor for SoftwareSerial port */
SerialCommand::SerialCommand(SoftwareSerial &port)
  : serialPort(&port),
    commandCount(0),
    defaultHandler(NULL)
{
  clear();
}
#else
/* Constructor for HardwareSerial port */
SerialCommand::SerialCommand(HardwareSerial &port)
  : serialPort(&port),
    commandCount(0),
    defaultHandler(NULL)
{
  clear();
}
#endif

/* Setup serial port */
void SerialCommand::begin(uint32_t baud)
{
  serialPort->begin(baud);
  serialPort->flush();
  println("READY");
}

/* Clear buffer */
void SerialCommand::clear(void)
{
  pBuff = buffer;
  *pBuff = '\0';
}

// Retrieve the next token ("word" or "argument") from the Command buffer.  
// returns a NULL if no more tokens exist.   
char* SerialCommand::next(void) 
{
	return strtok_r(NULL, SERIAL_CMD_DEL, &last);
}

// This checks the Serial stream for characters, and assembles them into a buffer.  
// When the terminator character (default '\r') is seen, it starts parsing the 
// buffer for a prefix command, and calls handlers setup by addCommand() member
void SerialCommand::loop(void) 
{
  int i; 
  boolean matched = false;

  while (serialPort->available() > 0)
  {
    inChar = serialPort->read(); 

    serialPort->print(inChar); /* ECHO received command */

    if (SERIAL_CMD_EOL == inChar) /* Check for EOL */
    {
      serialPort->println();

#if (SERIAL_CMD_DBG_EN == 1)
      serialPort->print("Received: "); 
      serialPort->println(buffer); 
#endif
     
      token = strtok_r(buffer, SERIAL_CMD_STR, &last); /* Search for command at start of buffer */

      if (NULL == token)
      {
        if (NULL != defaultHandler)
        {
          (*defaultHandler)(); 
        }
        else
        {
          sendERROR();
        }
        return;
      }

      for (i = 0; (i < commandCount); i++) 
      {

#if (SERIAL_CMD_DBG_EN == 1)
        serialPort->print("Comparing ["); 
        serialPort->print(token); 
        serialPort->print("] to [");
        serialPort->print(commandList[i].command);
        serialPort->println("]");
#endif

				/* Compare the found command against the list of known commands for a match */
				if (0 == strncmp(token, commandList[i].command, SERIAL_CMD_BUFF_LEN)) 
				{
#if (SERIAL_CMD_DBG_EN == 1)
          serialPort->print("Matched Command: "); 
          serialPort->println(token);
#endif
					/* Execute the stored handler function for the command */
					(*commandList[i].function)(); 
					clear();
					matched = true; 
					break; 
				}
			}

			if (!matched) /* If no match found */
      {
				if (NULL != defaultHandler)
        {
          (*defaultHandler)(); 
        }
        else
        {
          sendERROR();
        }
				clear(); 
			}

		}
    else if (isprint(inChar)) /* Only printable characters into the buffer */
		{
      /* Check buffer overflow */
      if ((pBuff - buffer) > (SERIAL_CMD_BUFF_LEN - 2))
      {
        clear();              /* Wrap buffer */  
      } 

      *pBuff++ = inChar;  /* Put character into buffer */
      *pBuff = '\0';      /* Always null terminate strings */
		}

	}
}

// Adds a "command" and a handler function to the list of available commands.  
// This is used for matching a found token in the buffer, and gives the pointer
// to the handler function to deal with it. 
void SerialCommand::addCommand(const char *command, void (*function)())
{
#if (SERIAL_CMD_DBG_EN == 1)
  serialPort->print(commandCount); 
  serialPort->print("-"); 
  serialPort->print("Adding command for "); 
  serialPort->println(command); 
#endif

  commandList = (SerialCommandCallback *) realloc(commandList, (commandCount + 1) * sizeof(SerialCommandCallback));
  strncpy(commandList[commandCount].command, command, SERIAL_CMD_BUFF_LEN); 
  commandList[commandCount].function = function; 
  commandCount++;
}

// This sets up a handler to be called in the event that the receveived command string
// isn't in the list of things with handlers.
void SerialCommand::addDefault(void (*function)())
{
  defaultHandler = function;
}

size_t SerialCommand::write(uint8_t character) { /*blahblah is the name of your class*/
  serialPort->write(character);
}

void SerialCommand::sendOK(void)
{
  println("\r\nOK\r\n"); 
}

void SerialCommand::sendERROR(void)
{
  println("\r\nERROR\r\n"); 
}