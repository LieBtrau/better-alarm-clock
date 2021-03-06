/*
 * Calendar.ino  -- scheduling and event manipulation example.
 *  Created on: Jan 2, 2016
 *      Author: Pat Deegan
 *      Part of the Chronos library project
 *      Copyright (C) 2016 Pat Deegan, http://psychogenic.com
 *  Modified by Christoph Tack, 2018
 *
 *  This file is part of the Chronos embedded datetime/calendar library.
 *
 *     Chronos is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU Lesser Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Chronos is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU Lesser Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser Public License
 *    along with Chronos.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "alarmcalendar.h"

AlarmCalendar ac1(2);
HardwareSerial *ser1 = &Serial;
bool alarmStarted = false;
unsigned long alarmStart = 0;

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__);
    setTime(1552892385); //Monday, March 18, 2019 6:59:45 AM

    ac1.setAlarmTime(7, 0);
    ac1.setWeekdays((WEEKDAYS)(WD_MONDAY | WD_TUESDAY));
}

void loop()
{
    Chronos::DateTime timenow = Chronos::DateTime::now();
    time_t secondsToGo;
    if (ac1.getSecondsToStartOfNextEvent(timenow.asEpoch(), secondsToGo))
    {
        ser1->printf("Next event occurs in %us.\r\n", secondsToGo);
    }
    if (ac1.isUnacknowledgedAlarmOnGoing(Chronos::DateTime::now().asEpoch()))
    {
        ser1->print("a-");
        if (!alarmStarted)
        {
            alarmStart = millis();
        }
        alarmStarted = true;
        if (millis() - alarmStart > 5000)
        {
            ac1.acknowledgeAlarm();
            ser1->println("acknowledge alarm");
        }
    }
    if(ac1.isAlarmIn24Hours(timenow.asEpoch()))
    {
        ser1->println("Alarm within 24h.");
    }
    delay(1000);
}
