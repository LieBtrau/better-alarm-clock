/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */

#include "stm32rtcwrapper.h"
#include "alarmcalendar.h"
#include "robustDcf.h"
#include "tm1637clock.h"

Stm32RtcWrapper stmRtc;
AlarmCalendar syncCalendar(5);
HardwareSerial* ser1=&Serial;
RobustDcf rd(PB6);
tm1637clock dispClock(PB12, PB13);
volatile bool syncOngoing=false;

bool getDcfTime(Chronos::EpochTime& epoch)
{
    if(!rd.update(epoch))
    {
        dispClock.setColon(false);
        return false;
    }
    dispClock.setColon(true);
    ser1->println("Time updated.");
    if(stmRtc.setEpoch(epoch))
    {
        setTime(epoch);//Force set time, otherwise it will only be set after 300s
        Chronos::DateTime::now().printTo(*ser1);
    }
    return true;
}

time_t getRtcTime()
{
    return stmRtc.get();
}

void updateDcf(bool start)
{
    syncOngoing=start;
}

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    rd.init();
    stmRtc.begin();
    dispClock.init();
    setSyncProvider(getRtcTime); // the function to get the time from the RTC
    delay(2000);
    if (timeStatus() != timeSet)
        ser1->println("Unable to sync with the RTC");
    else
        ser1->println("RTC has set the system time");

    syncCalendar.setDailyAlarm(2, 0);
    syncCalendar.setAlarmCallBack(updateDcf);
}

void loop()
{
    if (timeStatus() != timeSet)
    {
        syncOngoing=true;
    }
    else
    {
        Chronos::DateTime curDateTime = Chronos::DateTime::now();
        if(dispClock.getMinutes()!=curDateTime.minute())
        {
            dispClock.showTime(curDateTime.hour(), curDateTime.minute());
        }
        syncCalendar.loop();
    }
    if(syncOngoing)
    {
        Chronos::EpochTime epoch;
        syncOngoing = !getDcfTime(epoch);
    }
}
