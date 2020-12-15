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

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__);
    initClock();
    if (!initVisualElements() || !initDisplayOnOffControl())
    {
        ser1->println("Failed to initialize.");
        while (1)
            ; // If we fail to communicate, loop forever.
    }

    ac1.setAlarm(21, 53);
    ac1.enableWeekday(Chronos::Weekday::Tuesday);
}

void loop()
{
    byte hours, minutes;
    time_t localTime;
    bool buttonPressed = false;

    //Redraw LED array
    if (getLocalTimeSeconds(localTime) && (splitTime(localTime, hours, minutes) || displayTurnedOn(buttonPressed) || ac1.isUnacknowledgedAlarmOnGoing(localTime)))
    {
        showTime(hours, minutes, isStillSynced(), ac1.isUnacknowledgedAlarmOnGoing(localTime));
        ser1->printf("%02d%s%02d\r\n", hours, isStillSynced() ? ":" : "v", minutes);
    }
    if (ac1.isUnacknowledgedAlarmOnGoing(localTime))
    {
        setBrightness(15);
        setVisible(true);
    }
    else
    {
        if (isDisplayOn(buttonPressed))
        {
            byte brightness;
            if (getDisplayBrightness(brightness))
            {
                Serial.printf("New brightness: %d\r\n", brightness);
                setBrightness(brightness);
            }
            setVisible(true);
        }
        else
        {
            setVisible(false);
        }
    }

    redraw();

    delay(100);
}