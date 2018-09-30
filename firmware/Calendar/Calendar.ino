/*
 * Calendar.ino  -- scheduling and event manipulation example.
 *  Created on: Jan 2, 2016
 *      Author: Pat Deegan
 *      Part of the Chronos library project
 *      Copyright (C) 2016 Pat Deegan, http://psychogenic.com
 *  Modified by Christoph Tack on, Sep 30, 2018
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

#include "stm32rtcwrapper.h"
#include "alarmcalendar.h"

// the serial device specifics
#define SERIAL_BAUDRATE    9600
#define SERIAL_DEVICE     Serial

Stm32RtcWrapper stmRtc;
AlarmCalendar ac;

#define PRINTLN(...)  SERIAL_DEVICE.println(__VA_ARGS__)

void setup() {
    while (!SERIAL_DEVICE);
    SERIAL_DEVICE.begin(SERIAL_BAUDRATE);

    stmRtc.begin();
    setSyncProvider(stmRtc.get);   // the function to get the time from the RTC
    if(timeStatus()!= timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");

    // printTo is a convenience method useful for debugging
    // in real life, you'd use accessors and format it however you like.
    Chronos::DateTime::now().printTo(SERIAL_DEVICE);//Saturday, Jan 1 2000

    ac.addDailyEvent(AlarmCalendar::ALARM1, 9,0);
    Chronos::DateTime nextEventStart;
    if(ac.getStartOfNextEvent(nextEventStart))
    {
        PRINTLN();
        nextEventStart.printTo(SERIAL_DEVICE);
    }
    ac.addOnceOnlyEvent(AlarmCalendar::ALARM1, Chronos::Weekday::Thursday, 10,0);
    nextEventStart;
    if(ac.getStartOfNextEvent(nextEventStart))
    {
        PRINTLN();
        nextEventStart.printTo(SERIAL_DEVICE);
    }
    ac.addWeeklyEvent(AlarmCalendar::ALARM1, Chronos::Weekday::Thursday, 11,0);
    nextEventStart;
    if(ac.getStartOfNextEvent(nextEventStart))
    {
        PRINTLN();
        nextEventStart.printTo(SERIAL_DEVICE);
    }
}


void loop()
{
}
