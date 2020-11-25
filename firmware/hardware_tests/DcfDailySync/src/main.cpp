/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */

#include "dcfUtcClock.h"
HardwareSerial *ser1 = &Serial;

DcfUtcClock dcfclock(PA8, true);
const byte pinDisableDCF = PB1;
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
Timezone CE(CEST, CET);
byte lastMinute = 0;

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__);
    pinMode(pinDisableDCF, OUTPUT);
    digitalWrite(pinDisableDCF, LOW);
    dcfclock.init();
}

void loop()
{
    if (dcfclock.update())
    {
        time_t utc = now();
        TimeChangeRule *tcr; // pointer to the time change rule, use to get the TZ abbrev
        time_t t = CE.toLocal(utc, &tcr);
        if (lastMinute != minute(t))
        {
            lastMinute = minute(t);
            ser1->printf("%02d:%02d\r\n", hour(t), lastMinute);
        }
    }
    else
    {
        ser1->print(".");
        delay(1000);
    }
}
