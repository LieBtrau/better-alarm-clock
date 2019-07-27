/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */

#include "tm1637clock.h"
#include "dcfUtcClock.h"
HardwareSerial* ser1=&Serial;

tm1637clock dispClock(PB12, PB13);
DcfUtcClock dcfclock(PA8, true);

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    dcfclock.init();
    dispClock.init();
}

void loop()
{
    if(dcfclock.update())
    {
        Chronos::DateTime curDateTime = Chronos::DateTime::now();
        if(dispClock.getMinutes()!=curDateTime.minute())
        {
            dispClock.showTime(curDateTime.hour(), curDateTime.minute());
        }
    }
}
