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

#include "stm32rtcwrapper.h"
#include "alarmcalendar.h"
#include "source/robustDcf.h"

Stm32RtcWrapper stmRtc;
AlarmCalendar ac;
HardwareSerial* ser1=&Serial1;
RobustDcf rd(PB6, PC13);

bool getDcfTime(Chronos::EpochTime& epoch)
{
    return rd.update(epoch);
}

time_t getRtcTime()
{
    return stmRtc.get();
}

void alarmCallback(ALARMNR nr)
{
    ser1->println("Alarm is active");
}

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    rd.init();
    stmRtc.begin();
    stmRtc.setSyncProvider(getDcfTime);
    setSyncProvider(getRtcTime); // the function to get the time from the RTC
    if (timeStatus() != timeSet)
        ser1->println("Unable to sync with the RTC");
    else
        ser1->println("RTC has set the system time");

    // printTo is a convenience method useful for debugging
    // in real life, you'd use accessors and format it however you like.
    Chronos::DateTime::now().printTo(*ser1); //Saturday, Jan 1 2000

    ac.addDailyEvent(ALARM1, 9, 0);
    Chronos::DateTime nextEventStart;
    if (ac.getStartOfNextEvent(nextEventStart))
    {
        ser1->println();
        nextEventStart.printTo(*ser1);
        ser1->print('\t');
        ser1->println(nextEventStart.asEpoch());
    }
    ac.addOnceOnlyEvent(ALARM1, Chronos::Weekday::Thursday, 10, 0);
    if (ac.getStartOfNextEvent(nextEventStart))
    {
        ser1->println();
        nextEventStart.printTo(*ser1);
        ser1->print('\t');
        ser1->println(nextEventStart.asEpoch());
    }
    ac.addWeeklyEvent(ALARM1, Chronos::Weekday::Thursday, 11, 0);
    if (ac.getStartOfNextEvent(nextEventStart))
    {
        ser1->println();
        nextEventStart.printTo(*ser1);
        ser1->print('\t');
        ser1->println(nextEventStart.asEpoch());
    }
    ac.setAlarmCallBack(alarmCallback);
    ac.setTimeSync(2, getDcfTime);
}

void loop()
{
    if (timeStatus() != timeSet)
    {
        Chronos::EpochTime epoch;
        rd.update(epoch);
    }
    ac.checkForOngoingEvents();
}
