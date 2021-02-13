#include <SerialCommand.h>

#define pinLED 13 /* Arduino on-board LED */

SerialCommand mySerialCMD;

void setup()
{
    pinMode(pinLED, OUTPUT);   /* Configure the onboard LED as output */
    digitalWrite(pinLED, LOW); /* Default LED OFF */

    /* Setup SerialCommand port */
    Serial.begin(9600);

    /* 
     * Setup callbacks for SerialCommand commands: 
     * PING -> Execute handler
     */
    mySerialCMD.addExecuteCommand((char *)"PING", &ping);

    /* 
     * Setup callbacks for SerialCommand commands: 
     * AT+LED=X -> Write handler
     */
    mySerialCMD.addWriteCommand((char *)"AT+LED", &ledHandler);
}

void loop()
{
    mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void ping()
{
    Serial.println("OK"); /* Send "OK" message */
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
        ok = true;
        switch (*arg)
        {
            case  '1':
                digitalWrite(pinLED, HIGH);
                Serial.println("+LED=ON");
                break;
            case '0':
                digitalWrite(pinLED, LOW);
                Serial.println("+LED=OFF");
                break;
            default:
                // Unexpected value
                ok = false;
        }
    }

    if (ok)
    {
        Serial.println("OK"); /* Send "OK" message */
    }
    else
    {
        Serial.println("\r\nERROR\r\n");
    }
}