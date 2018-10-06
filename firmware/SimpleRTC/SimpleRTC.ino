#include <STM32RTC.h>
/**
  ******************************************************************************
    @file    SimpleRTC.ino
    @author  WI6LABS
    @version V1.0.0
    @date    12-December-2017
    @brief   Simple RTC example.

  ******************************************************************************
    @attention

    <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
      1. Redistributions of source code must retain the above copyright notice,
         this list of conditions and the following disclaimer.
      2. Redistributions in binary form must reproduce the above copyright notice,
         this list of conditions and the following disclaimer in the documentation
         and/or other materials provided with the distribution.
      3. Neither the name of STMicroelectronics nor the names of its contributors
         may be used to endorse or promote products derived from this software
         without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  ******************************************************************************
*/

/* Changes by Christoph Tack: adding Serial interface, so that utc time can be sent to stm32 and time can be asked back.
 *  Linux command line:
 *  Configure Serial port : stty -F /dev/ttyACM0 speed 9600 cs8 -cstopb -parenb raw
 *  To send UTC to the STM32, open a 2nd terminal and execute: echo -ne $(date +%s) > /dev/ttyACM0
 *
 *  Adding local time zone functionality
 */

#include "Chronos.h"
#include <Timezone.h>    // https://github.com/JChristensen/Timezone

/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();
unsigned long startUtc=0;

TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60}; //Last Sunday of October, at 3AM, go to UTC+60min
Timezone myTZ(myDST, mySTD);

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    rtc.setClockSource(STM32RTC::RTC_LSE_CLOCK);
    /*Upon reset, RTC time will also be reset.  The "keep time" functionality doesn't work yet.
     *  in our application this is not a problem, because clock time can be retrieved using DCF.
     * It only takes a minute.
     */
    rtc.begin(STM32RTC::RTC_HOUR_24); // initialize RTC 24H format
    //End of October switch from DST to STD
    //1540686780    Sunday, October 28, 2018 2:33:00 AM GMT+02:00
    //1540690380    Sunday, October 28, 2018 2:33:00 AM GMT+01:00
    //1540693980    Sunday, October 28, 2018 3:33:00 AM GMT+01:00
    //End of March switch from STD to DST
    //1553992200    Sunday, March 31, 2019 1:30:00 AM GMT+01:00
    //1553995800    Sunday, March 31, 2019 3:30:00 AM GMT+02:00
    rtc.setEpoch(1553992200);
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
                startUtc=newutc;
                rtc.setEpoch(newutc);
                printDateTime();
                rtc.setAlarmEpoch( rtc.getEpoch() + 10);
                rtc.attachInterrupt(alarmMatch);
                rtc.enableAlarm(rtc.MATCH_DHHMMSS);
            }
            else
            {
                Serial.print("PC epoch - RTC epoch = "); Serial.println(newutc - rtc.getEpoch());
                Serial.print("Number of seconds RTC is running: "); Serial.println(newutc - startUtc);
            }
        }
    }
}

void printDateTime()
{
    Serial.print(rtc.getEpoch());
    Serial.print("\t");
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
