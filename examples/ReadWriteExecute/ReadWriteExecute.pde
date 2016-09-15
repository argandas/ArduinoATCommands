#include <SerialCommand.h>

SerialCommand mySerialCMD;

void setup()
{
  /* Setup SerialCommand port */
  mySerialCMD.begin(Serial, 9600);
  
  /* Setup callbacks for SerialCommand commands */
  mySerialCMD.addCommand("AT", NULL, readHandler, writeHandler, executeHandler);
}

void loop()
{
  mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void readHandler()
{
  mySerialCMD.print(__FUNCTION__);
  mySerialCMD.sendOK(); /* Send "OK" message */
}

void writeHandler()
{
  mySerialCMD.print(__FUNCTION__);
  mySerialCMD.sendOK(); /* Send "OK" message */
}

void executeHandler()
{
  mySerialCMD.print(__FUNCTION__);
  mySerialCMD.sendOK(); /* Send "OK" message */
}