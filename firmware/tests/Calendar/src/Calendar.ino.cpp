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

//Stm32RtcWrapper stmRtc;
AlarmCalendar ac1;
//AlarmCalendar syncCalendar;
HardwareSerial* ser1=&Serial;
//RobustDcf rd(PB6, PC13);
static volatile bool alarmIsActive=false;

// bool getDcfTime(Chronos::EpochTime& epoch)
// {
//     if(!rd.update(epoch))
//     {
//         return false;
//     }
//     ser1->println("Time updated.");
//     if(stmRtc.setEpoch(epoch))
//     {
//         setTime(epoch);//Force set time, otherwise it will only be set after 300s
//         Chronos::DateTime::now().printTo(*ser1);
//     }
//     return true;
// }

// time_t getRtcTime()
// {
//     return stmRtc.get();
// }

// void updateDcf()
// {
//     Chronos::EpochTime epoch;
//     getDcfTime(epoch);
// }

void alarmCallback()
{
    alarmIsActive=true;
}

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    // rd.init();
    // stmRtc.begin();
    // setSyncProvider(getRtcTime); // the function to get the time from the RTC
    // delay(2000);
    // if (timeStatus() != timeSet)
    //     ser1->println("Unable to sync with the RTC");
    // else
    //     ser1->println("RTC has set the system time");
    setTime(1552892378);//Monday, March 18, 2019 6:59:38 AM

    // printTo is a convenience method useful for debugging
    // in real life, you'd use accessors and format it however you like.
    Chronos::DateTime::now().printTo(*ser1); //Jan 1 1970
    Chronos::DateTime nextAlarmTime; 
    ac1.setTime(7,0);
    ac1.enableWeekday(Chronos::Weekday::Monday);
    ac1.setAlarmCallBack(alarmCallback);
    if(ac1.getStartOfNextEvent(&nextAlarmTime))
    {
        nextAlarmTime.printTo(*ser1);
    }
    // syncCalendar.setDailyAlarm(2, 0);
    // syncCalendar.setAlarmCallBack(updateDcf);
}

void loop()
{
    static bool alarmWasOn=false;
    bool alarmIsOn = ac1.isAlarmOnGoing();
    if(alarmIsOn && !alarmWasOn)
    {
        Chronos::DateTime::now().printTo(*ser1);
        ser1->println(": Start of alarm");
    }
    if(!alarmIsOn && alarmWasOn)
    {
        Chronos::DateTime::now().printTo(*ser1);
        ser1->println(": End of alarm");
    }
    alarmWasOn = alarmIsOn;

    // if (timeStatus() != timeSet)
    // {
    //     Chronos::EpochTime epoch;
    //     getDcfTime(epoch);
    // }
    // else
    // {
    //     syncCalendar.loop();
    // }
}
