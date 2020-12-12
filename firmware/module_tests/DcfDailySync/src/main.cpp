/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */
#include "Arduino.h"
#include "TimeSync.h"

HardwareSerial *ser1 = &Serial;

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__);
    initClock();
}

void loop()
{
    byte hours, minutes;
    if (getLocalTime(hours, minutes))
    {
        ser1->printf("%02d%s%02d\r\n", hours, isStillSynced ? ":" : "v", minutes);
    }
    else
    {
        ser1->print(".");
    }
    delay(1000);
}
