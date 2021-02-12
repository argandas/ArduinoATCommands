#include <SerialCommand.h>

#define pinLED 13 /* Arduino on-board LED */

SerialCommand mySerialCMD;

void setup()
{
    pinMode(pinLED, OUTPUT);   /* Configure the onboard LED as output */
    digitalWrite(pinLED, LOW); /* Default LED OFF */

    /* Setup SerialCommand port */
    mySerialCMD.begin(Serial, 9600);

    /* Setup callbacks for SerialCommand commands */
    mySerialCMD.addCommand("AT", ping);
    mySerialCMD.addCommand("AT+LED", ledHandler);
}

void loop()
{
    mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void ping()
{
    mySerialCMD.sendOK(); /* Send "OK" message */
}

/* 
  Handler to turn ON and OFF a LED
  Usage examples :
    - AT+LED=1<CR>
    - AT+LED=0<CR>
*/
void ledHandler()
{
    char *arg;
    bool ok = false;
    arg = mySerialCMD.next(); /* Get the next argument from the SerialCommand object buffer */
    if (arg != NULL)          /* Check if argument exists */
    {
        if (*arg == '1')
        {
            digitalWrite(pinLED, HIGH);
            mySerialCMD.println("+LED=ON");
            ok = true;
        }
        else if (*arg == '0')
        {
            digitalWrite(pinLED, LOW);
            mySerialCMD.println("+LED=OFF");
            ok = true;
        }
    }

    if (ok)
    {
        mySerialCMD.sendOK(); /* Acknowledge the command */
    }
    else
    {
        mySerialCMD.sendERROR(); /* Send ERROR message */
    }
}