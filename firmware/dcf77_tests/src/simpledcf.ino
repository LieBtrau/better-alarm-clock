#include "phaseDetector.h"
#include "secondsDecoder.h"
#include "bcdDecoder.h"
#include "timezoneDecoder.h"
#include <Timezone.h> // https://github.com/JChristensen/Timezone
#include <Chronos.h>

PhaseDetector pd(PB6, PC13);
SecondsDecoder sd;
volatile bool secondTicked = false;
BcdDecoder minutes(21, 7, true, 0, 59, 4);
BcdDecoder hours(29, 6, true, 0, 23, 3);
BcdDecoder days(36, 6, false, 1, 31, 3);
BcdDecoder months(45, 5, false, 1, 12, 2);
BcdDecoder years(50, 8, false, 0, 99, 4);
TimeZoneDecoder tzd;
bool syncMark, longPulse;

TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
Timezone myTZ(myDST, mySTD);

//secondsTick is called by an ISR.  It should be kept as short as possible
void secondsTick(const bool isSyncMark, const bool isLongPulse)
{
    syncMark = isSyncMark;
    longPulse = isLongPulse;
    secondTicked = true;
}

void setup()
{
    Serial1.begin(115200);
    Serial1.println("Hello, I'm awake!");
    pd.init(secondsTick);
}


Chronos::EpochTime getEpoch()
{
    uint8_t minute, hour, day, month, year;
    int16_t secondsOffset;
    minutes.getTime(minute);
    hours.getTime(hour);
    days.getTime(day);
    months.getTime(month);
    years.getTime(year);
    year += 2000;
    Chronos::DateTime localtime(year, month, day, hour, minute);
    tzd.getSecondsOffset(secondsOffset);
    Chronos::EpochTime epoch = localtime.asEpoch() - secondsOffset;
    localtime.printTo(Serial1);
    return epoch;
}

void loop()
{
    if (secondTicked)
    {
        secondTicked = false;
        sd.updateSeconds(syncMark, longPulse);
        uint8_t second;
        if (sd.getSecond(second) && second == 59)
        {
            //get current data
            SecondsDecoder::BITDATA data;
            sd.getTimeData(&data);
            Serial1.print(data.bitShifter, HEX);
            minutes.update(&data);
            hours.update(&data);
            days.update(&data);
            months.update(&data);
            years.update(&data);
            tzd.update(&data);
            Serial1.println(getEpoch());
            //set prediction
            minutes.advanceTick();
            tzd.setPrediction(false);
        }
    }
}


/*
Best time to sync the clock is at 01:01 UTC:
    winter to summer time : at 01:00 UTC : 2AM becomes 3AM local time
    summer to winter time : at 01:00 UTC : 3AM becomes 2AM local time
Chronos::DateTime::setTime(2015, 12, 21, 17, 30, 0); //DateTime(YYYY, MM, DD, HH, mm, SS)
Chronos::DateTime inTwoWeeks = nowTime + Chronos::Span::Weeks(2);
PRINT(curDateTime.year());
  PRINT('-');
  PRINT(curDateTime.month());
  PRINT('-');
  PRINT(curDateTime.day());
  PRINT(' ');
  PRINT(curDateTime.hour());
  PRINT(':');
  PRINT(curDateTime.minute());
  PRINT(':');
  PRINT(curDateTime.second());
  PRINTLN('!');
      Chronos::DateTime utc(rtc.getEpoch());
    Chronos::DateTime localTime = myTZ.toLocal(utc.asEpoch());

  */