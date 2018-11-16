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

Chronos::EpochTime getUnixEpochTime()
{
    uint8_t minute, hour, day, month, year;
    int16_t secondsOffset;
    minutes.getTime(minute);
    hours.getTime(hour);
    days.getTime(day);
    months.getTime(month);
    years.getTime(year);
    Chronos::DateTime localtime(tmYearToCalendar(y2kYearToTm(year)), month, day, hour, minute);
    tzd.getSecondsOffset(secondsOffset);
    Chronos::EpochTime epoch = localtime.asEpoch() - secondsOffset;
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
            bool bSuccess = sd.getTimeData(&data);
            bSuccess &= minutes.update(&data);
            bSuccess &= hours.update(&data);
            bSuccess &= days.update(&data);
            bSuccess &= months.update(&data);
            bSuccess &= years.update(&data);
            bSuccess &= tzd.update(&data);
            if (bSuccess)
            {
                Chronos::DateTime localTime = myTZ.toLocal(getUnixEpochTime());
                localTime.printTo(Serial1);
                Serial1.println();
                //set prediction for next minute
                Chronos::DateTime predictionTime = localTime + Chronos::Span::Minutes(1);
                minutes.setPrediction(predictionTime.minute());
                hours.setPrediction(predictionTime.hour());
                days.setPrediction(predictionTime.day());
                months.setPrediction(predictionTime.month());
                years.setPrediction(tmYearToY2k(CalendarYrToTm(predictionTime.year())));
                tzd.setPrediction(myTZ.locIsDST(predictionTime.asEpoch()));
            }
        }
    }
}