#include <SerialCommand.h>

SerialCommand mySerialCMD;

void setup()
{
    /* Setup SerialCommand port */
    mySerialCMD.begin(Serial, 9600);

    /* Setup callbacks for SerialCommand commands */
    mySerialCMD.addExecuteCommand("AT", ping);
}

void loop()
{
    mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void ping()
{
    mySerialCMD.print("Hello world!");
    mySerialCMD.sendOK(); /* Send "OK" message */
}