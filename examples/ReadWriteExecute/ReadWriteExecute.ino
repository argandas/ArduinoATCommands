#include <SerialCommand.h>

SerialCommand mySerialCMD;

void setup()
{
    /* Setup SerialCommand port */
    Serial.begin(9600);

    /* 
     * Setup callbacks for SerialCommand commands 
     * AT=? -> Test handler
     * AT? -> Read handler
     * AT=X,Y,X -> Write handler
     * AT -> Execute handler
     */
    mySerialCMD.addCommand((char *)"AT", testHandler, readHandler, writeHandler, executeHandler);
}

void loop()
{
    mySerialCMD.loop(); /* Process data from serial port each iteration */
}

void testHandler()
{
    sendOK(); /* Send "OK" message */
}

void readHandler()
{
    Serial.print(__FUNCTION__);
    char *arg = mySerialCMD.next(); /* Get the next argument from the SerialCommand object buffer */
    int i = 0;
    while (arg != NULL)
    {
        Serial.print("argument #");
        Serial.print(++i);
        Serial.print(": ");
        Serial.println(arg);
        arg = mySerialCMD.next(); /* Get the next argument from the SerialCommand object buffer */
    }

    sendOK(); /* Send "OK" message */
}

void writeHandler()
{
    Serial.print(__FUNCTION__);
    sendOK(); /* Send "OK" message */
}

void executeHandler()
{
    Serial.print(__FUNCTION__);
    sendOK(); /* Send "OK" message */
}

void sendOK()
{
    Serial.println("OK"); /* Send "OK" message */
}