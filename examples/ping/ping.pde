#include <SerialCommand.h>

SerialCommand mySerialCMD(Serial);

void setup()
{
  /* Setup callbacks for SerialCommand commands */
  mySerialCMD.addCommand("AT", ping);
  
  /* Setup SerialCommand port */
  mySerialCMD.begin(9600);
}

void loop()
{
  mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void ping()
{
  mySerialCMD.sendOK(); /* Send "OK" message */
}