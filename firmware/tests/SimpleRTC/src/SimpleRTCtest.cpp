//More info : http://embedded-lab.com/blog/stm32s-internal-rtc/
#include "Chronos.h"
#include <Timezone.h> // https://github.com/JChristensen/Timezone
#include <utility/rtc_util.h>

void printDateTime();
void alarmMatch();

unsigned long startUtc = 0;

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
    Serial1.begin(115200);
    while (!Serial1)
        ;
    rtc_init(RTCSEL_LSE);
    rtc_set_prescaler_load(0x7fff);
    //End of October switch from DST to STD
    //1540686780    Sunday, October 28, 2018 2:33:00 AM GMT+02:00
    //1540690380    Sunday, October 28, 2018 2:33:00 AM GMT+01:00
    //1540693980    Sunday, October 28, 2018 3:33:00 AM GMT+01:00
    //End of March switch from STD to DST
    //1553992200    Sunday, March 31, 2019 1:30:00 AM GMT+01:00
    //1553995800    Sunday, March 31, 2019 3:30:00 AM GMT+02:00
    printDateTime();
    Serial1.println("Ready to receive UTC");
}

void loop()
{
    // while (Serial1.available() > 0)
    // {
    //     unsigned long newutc = Serial1.parseInt();
    //     if (newutc)
    //     {
    //         if (!startUtc)
    //         {
    //             startUtc = newutc;
    //             rtc_set_count(newutc);
    //             printDateTime();
    //             rtc_set_alarm(rtc_get_count() + 10);
    //             rtc_attach_interrupt(RTC_ALARM_SPECIFIC_INTERRUPT, alarmMatch);
    //         }
    //         else
    //         {
    //             Serial1.print("PC epoch - RTC epoch = ");
    //             Serial1.println(newutc - rtc_get_count());
    //             Serial1.print("Number of seconds RTC is running: ");
    //             Serial1.println(newutc - startUtc);
    //         }
    //     }
    // }
}

void printDateTime()
{
    Serial1.print(rtc_get_count());
    Serial1.print("\t");
    // Print date...
    Chronos::DateTime utc(rtc_get_count());
    Chronos::DateTime localTime = myTZ.toLocal(utc.asEpoch());
    localTime.printTo(Serial1);
    Serial1.println();
}

void alarmMatch()
{
    Serial1.println("Alarm Match!");
}
