/*  adding Serial interface, so that utc time can be sent to stm32 and time can be asked back.
 *  Linux command line:
 *  Configure Serial port : stty -F /dev/ttyUSB0 speed 115200 cs8 -cstopb -parenb raw
 *  To send UTC to the STM32, open a 2nd terminal and execute: echo -ne $(date +%s) > /dev/ttyUSB0
 * 
 *  Remark: You replace "/dev/ttyUSB0" with the monitor_port of platformio.ini
 *  Adding local time zone functionality
 * 
 *  Connections:
 *  ------------
 *  A9 : UART TX to RX of USB-UART
 *  A10 : UART RX to TX of USB-UART
 *  5V : to 5V
 *  G : to GND
 *  3VB : VBAT to 3V3
 * 
 *  VBAT
 *  ----
 *  If you remove the 5V connection, while leaving VBAT connected, the RTC should keep running.  When reconnecting the 5V, the MCU
 *  resets and the RTC gets partially re-initialized as well.  The hours are kept, the year/month/day is lost on STM32F1.
 *  The problem is that the RTC-driver converts UTC-seconds to D, M, Y but afterwards has no way of converting that back to UTC to write
 *  to the RTC-registers.
 *  So the VBAT isn't of much use with the current RTC-driver.
 */

#include "Chronos.h"
#include <Timezone.h>
#include <STM32RTC.h>

void printDateTime(Chronos::EpochTime epoch);
void alarmMatch(void *data);

unsigned long startUtc = 0;
STM32RTC &rtc = STM32RTC::getInstance();

TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
Timezone myTZ(myDST, mySTD);
unsigned long starttime = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    delay(500);
    Serial.printf("Build %s\r\n", __TIMESTAMP__);
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    rtc.begin();
    //End of October switch from DST to STD
    //1540686780    Sunday, October 28, 2018 2:33:00 AM GMT+02:00
    //1540690380    Sunday, October 28, 2018 2:33:00 AM GMT+01:00
    //1540693980    Sunday, October 28, 2018 3:33:00 AM GMT+01:00
    //End of March switch from STD to DST
    //1553992200    Sunday, March 31, 2019 1:30:00 AM GMT+01:00
    //1553995800    Sunday, March 31, 2019 3:30:00 AM GMT+02:00
    Serial.print("Time at power up: ");
    printDateTime(rtc.getEpoch());
    Serial.println();
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
                setTime(newutc);
                printDateTime(rtc.getEpoch());
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
    if (millis() > starttime + 1000)
    {
        starttime = millis();
        Serial.print("RTC: ");
        printDateTime(rtc.getEpoch());
        Serial.print("\tMillis: ");
        printDateTime(now());
        Serial.println();
    }
}

void printDateTime(Chronos::EpochTime epoch)
{
    // Print date...
    Chronos::DateTime utc(epoch);
    Chronos::DateTime localTime = myTZ.toLocal(utc.asEpoch());
    localTime.printTo(Serial);
}

void alarmMatch(void *data)
{
    UNUSED(data);
    Serial.println("Alarm Match!");
}
