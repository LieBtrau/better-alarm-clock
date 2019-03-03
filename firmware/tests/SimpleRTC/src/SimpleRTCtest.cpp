#include "Chronos.h"
#include <Timezone.h>
#include <STM32RTC.h>

void printDateTime();
void alarmMatch(void *data);

unsigned long startUtc = 0;
STM32RTC& rtc = STM32RTC::getInstance();

/*  adding Serial interface, so that utc time can be sent to stm32 and time can be asked back.
 *  Linux command line:
 *  Configure Serial port : stty -F /dev/ttyUSB0 speed 115200 cs8 -cstopb -parenb raw
 *  To send UTC to the STM32, open a 2nd terminal and execute: echo -ne $(date +%s) > /dev/ttyUSB0
 *  Adding local time zone functionality
 */

TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
Timezone myTZ(myDST, mySTD);

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    rtc.begin();
    //End of October switch from DST to STD
    //1540686780    Sunday, October 28, 2018 2:33:00 AM GMT+02:00
    //1540690380    Sunday, October 28, 2018 2:33:00 AM GMT+01:00
    //1540693980    Sunday, October 28, 2018 3:33:00 AM GMT+01:00
    //End of March switch from STD to DST
    //1553992200    Sunday, March 31, 2019 1:30:00 AM GMT+01:00
    //1553995800    Sunday, March 31, 2019 3:30:00 AM GMT+02:00
    printDateTime();
    Serial.println("Ready to receive UTC");
}

void loop()
{
    while (Serial.available() > 0)
    {
        unsigned long newutc = Serial.parseInt();
        if (newutc)
        {
            if (!startUtc)
            {
                startUtc = newutc;
                rtc.setEpoch(newutc);
                printDateTime();
                rtc.setAlarmEpoch(rtc.getEpoch() + 10);
                rtc.attachInterrupt(alarmMatch);
            }
            else
            {
                Serial.print("PC epoch - RTC epoch = ");
                Serial.println(newutc - rtc.getEpoch());
                Serial.print("Number of seconds RTC is running: ");
                Serial.println(newutc - startUtc);
            }
        }
    }
}

void printDateTime()
{
    // Print date...
    Chronos::DateTime utc(rtc.getEpoch());
    Chronos::DateTime localTime = myTZ.toLocal(utc.asEpoch());
    localTime.printTo(Serial);
    Serial.println();
}

void alarmMatch(void *data)
{
    UNUSED(data);
    Serial.println("Alarm Match!");
}
