#include "phaseDetector.h"
#include "secondsDecoder.h"
#include "binner.h"
#include <Timezone.h> // https://github.com/JChristensen/Timezone

PhaseDetector pd(PB6, PC13);
SecondsDecoder sd;
volatile bool secondTicked = false;
Binner minutes(21, 7, true, 0, 59, 4);
Binner hours(29, 6, true, 0, 23, 3);
Binner days(36, 6, false, 1, 31, 3);
Binner months(45, 5, false, 1, 12, 2);
Binner years(50, 8, false, 0, 99, 4);
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

void loop()
{
    if (secondTicked)
    {
        secondTicked = false;
        sd.updateSeconds(syncMark, longPulse);
        uint8_t second;
        if (sd.getSecond(second) && second == 59)
        {
            SecondsDecoder::BITDATA data;
            sd.getTimeData(&data);
            Serial1.print(data.bitShifter, HEX);
            minutes.update(&data);
            hours.update(&data);
            days.update(&data);
            months.update(&data);
            years.update(&data);
            getDcfTime();
           minutes.advanceTick();
        }
    }
}

void getDcfTime()
{
    uint8_t minute, hour, day, month, year;
    minutes.getTime(minute);
    hours.getTime(hour);
    days.getTime(day);
    months.getTime(month);
    years.getTime(year);
    char stime[100];
    sprintf(stime, "\t%d-%d-%d %d:%d", year, month, day, hour, minute);
    Serial1.println(stime);
}

/*Chronos::DateTime::setTime(2015, 12, 21, 17, 30, 0); //DateTime(YYYY, MM, DD, HH, mm, SS)
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