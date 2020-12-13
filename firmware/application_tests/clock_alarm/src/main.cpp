/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */
#include "Arduino.h"
#include "TimeSync.h"
#include "VisualElements.h"
#include "alarmcalendar.h"

AlarmCalendar ac1(2);
HardwareSerial *ser1 = &Serial;
bool bStarted = false;

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__);
    initClock();
    if (!initVisualElements())
    {
        ser1->println("Failed to initialize visual elements.");
        while (1)
            ; // If we fail to communicate, loop forever.
    }
    setBrightness(15);
    ac1.setAlarm(11, 35);
    ac1.enableWeekday(Chronos::Weekday::Sunday);
}

void loop()
{
    byte hours, minutes;
    time_t localTime;
    if (getLocalTimeSeconds(localTime) && splitTime(localTime, hours, minutes))
    {
        showTime(hours, minutes, isStillSynced());
        ser1->printf("%02d%s%02d\r\n", hours, isStillSynced() ? ":" : "v", minutes);
        if (ac1.isAlarmOnGoing(localTime))
        {
            if (!bStarted)
            {
                bStarted = true;
                Serial.println("start");
            }
        }
        else
        {
            if (bStarted)
            {
                bStarted = false;
                Serial.println("stop");
            }
        }
    }
    else
    {
        ser1->print(".");
    }
    redraw();
    delay(100);
}