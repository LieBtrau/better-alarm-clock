/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */

#include "tm1637clock.h"
#include "dcfUtcClock.h"
HardwareSerial* ser1=&Serial;

tm1637clock dispClock(PB12, PB13);
DcfUtcClock dcfclock(PA8, true);
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);


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
        time_t utc = now();
        TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev
        time_t t = CE.toLocal(utc, &tcr);
        if(dispClock.getMinutes()!=minute(t))
        {
            dispClock.showTime(hour(t),  minute(t));
        }
    }
}
