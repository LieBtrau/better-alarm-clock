//
//  www.blinkenlight.net
//
//  Copyright 2012 Udo Klein
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see http://www.gnu.org/licenses/
#include "phaseDetector.h"
#include "secondsDecoder.h"

PhaseDetector pd(PB6, PC13);
SecondsDecoder sd;
unsigned long ultimer = 0;
volatile bool secondTicked = false;

void secondsTick(const bool isSyncMark, const bool isLongPulse)
{
    sd.updateSeconds(isSyncMark, isLongPulse);
    secondTicked = true;
}

void setup()
{
    Serial1.begin(115200);
    Serial1.println();
    pd.init();
    pd.attachSecondEventHandler(secondsTick);
}

void loop()
{
    if (secondTicked)
    {
        secondTicked=false;
        uint8_t second;
        if (sd.getSecond(second) && second == 59)
        {
            uint64_t data = 0;
            sd.getTimeData(data);
            Serial1.println(data, BIN);
            //website (bit0 is left)    0-00010110010000-000101-00011011-0000011-001000-111-10001-000110000
            //captured (bit0 is right)  00001100-010001-111-000100-1100000-11011000-101000-00001001101000-0
        }
    }
}
