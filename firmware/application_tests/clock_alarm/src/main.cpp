/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */
#include "Arduino.h"
#include "TimeSync.h"
#include "VisualElements.h"
#include "alarmcalendar.h"
#include "DisplayOnOff.h"

AlarmCalendar ac1(2);
HardwareSerial *ser1 = &Serial;
byte alarmHours = 13, alarmMinutes = 25;
WEEKDAYS wd = (WEEKDAYS)(WD_MONDAY | WD_SUNDAY);

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__);
    initClockSource();
    if (!initVisualElements() || !initDisplayOnOffControl())
    {
        ser1->println("Failed to initialize.");
        while (1)
            ; // If we fail to communicate, loop forever.
    }

    ac1.setAlarmTime(alarmHours, alarmMinutes);
    ac1.setWeekdays(wd);
}

bool isAlarmBusy()
{
    time_t localTime;
    return getLocalTimeSeconds(localTime) && ac1.isUnacknowledgedAlarmOnGoing(localTime);
}

void loop()
{
    byte hours, minutes;
    time_t localTime;
    bool buttonPressed = false;
    DISPLAY_STATE ds = getDisplayState(buttonPressed, isAlarmBusy());

    if (getLocalTimeSeconds(localTime))
    {
        //Redraw LED array
        if (ds == DISPLAY_TURNED_ON || isNewMinuteStarted(localTime, hours, minutes))
        {
            showClockTime(hours, minutes, isStillSynced(), ac1.isUnacknowledgedAlarmOnGoing(localTime));
            Chronos::DateTime::now().printTo(*ser1);
            Serial.println();
        }
        //Redraw alarm LCD
        if ((ds == DISPLAY_TURNED_ON) || (ds == DISPLAY_ON))
        {
            showAlarmDisplay(ac1.isAlarmIn24Hours(localTime) ? AL_BOTH_ON : AL_BOTH_OFF);
        }
        if (ac1.isAlarmIn24Hours(localTime))
        {
            showAlarmTime(alarmHours, alarmMinutes);
        }
        //Redraw Weekday LEDs
        showWeekDay(wd);
    }

    //Brightness control for all elements
    switch (ds)
    {
    case DISPLAY_TURNED_ON:
        setVisible(true);
        break;
    case DISPLAY_ON:
        setBrightness(getDisplayBrightness(isAlarmBusy()));
        break;
    case DISPLAY_TURNED_OFF:
        setVisible(false);
        break;
    case DISPLAY_OFF:
    default:
        break;
    }

    redraw();

    delay(100);
}