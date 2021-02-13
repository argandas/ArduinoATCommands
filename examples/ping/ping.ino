#include <SerialCommand.h>

SerialCommand mySerialCMD;

void setup()
{
    /* Setup SerialCommand port */
    Serial.begin(9600);

    /* Setup callbacks for SerialCommand commands */
    mySerialCMD.addExecuteCommand((char*)"AT", ping);
}

void loop()
{
    mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void ping()
{
    Serial.println("Hello world!");
    Serial.println("OK"); /* Send "OK" message */
}