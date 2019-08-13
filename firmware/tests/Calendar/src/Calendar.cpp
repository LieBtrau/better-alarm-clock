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

void alarmCallback(bool bIsAlarmStartNotEnd)
{
    ser1->println();
    Chronos::DateTime::now().printTo(*ser1);
    if (bIsAlarmStartNotEnd)
    {
        ser1->println(": Start of alarm");
    }
    else
    {
        ser1->println(": End of alarm");
    }
}

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    setTime(1552892378); //Monday, March 18, 2019 6:59:38 AM

    // printTo is a convenience method useful for debugging
    // in real life, you'd use accessors and format it however you like.
    Chronos::DateTime timenow = Chronos::DateTime::now();
    timenow.printTo(*ser1); 
    Chronos::DateTime nextAlarmTime;
    ac1.setTime(7, 0);
    ac1.enableWeekday(Chronos::Weekday::Monday);
    ac1.setAlarmCallBack(alarmCallback);
    if (ac1.getStartOfNextEvent(&nextAlarmTime, &timenow))
    {
        nextAlarmTime.printTo(*ser1);
    }
}

void loop()
{
    Chronos::DateTime timenow = Chronos::DateTime::now();
    ac1.loop(&timenow);
}