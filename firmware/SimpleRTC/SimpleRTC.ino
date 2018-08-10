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

/* Changes by Christoph Tack: adding serial interface, so that utc time can be sent to stm32 and time can be asked back.
 *  Linux command line:
    Configure serial port : stty -F /dev/ttyACM0 speed 9600 cs8 -cstopb -parenb raw
    To send UTC to the STM32, open a 2nd terminal and execute: echo -ne $(date +%s) > /dev/ttyACM0
*/

/* Get the rtc object */
STM32RTC& rtc = STM32RTC::getInstance();
long startUtc=0;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  rtc.setClockSource(STM32RTC::RTC_LSE_CLOCK);
  rtc.begin(); // initialize RTC 24H format
  Serial.println("Ready to receive UTC");
}

void loop()
{
  while (Serial.available() > 0)
  {
    long newutc = Serial.parseInt();
    if (newutc)
    {
      if (!startUtc)
      {
        startUtc=newutc;
        rtc.setEpoch(newutc);
        printDateTime();
      }
      else
      {
        Serial.print("PC epoch - RTC epoch = "); Serial.println((long)(newutc - rtc.getEpoch()));
        Serial.print("Number of seconds RTC is running: "); Serial.println(newutc - startUtc);
      }
    }
  }
  //  delay(1000);

}

void printDateTime()
{
  // Print date...
  print2digits(rtc.getDay());
  Serial.print("/");
  print2digits(rtc.getMonth());
  Serial.print("/");
  print2digits(rtc.getYear());
  Serial.print(" ");

  // ...and time
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());

  Serial.println();
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0"); // print a 0 before if the number is < than 10
  }
  Serial.print(number);
}
