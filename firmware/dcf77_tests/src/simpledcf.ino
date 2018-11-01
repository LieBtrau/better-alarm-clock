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
unsigned long ultimer=0;


void secondsTick(const bool isSyncMark, const bool isLongPulse)
{
    sd.updateSeconds(isSyncMark, isLongPulse);
    Serial1.print(millis() - ultimer);
    Serial1.print(",");
    Serial1.print(sd.getSecond());
    Serial1.print(",");
    Serial1.println(isLongPulse);
    ultimer=millis();
}
 
void setup() {
    Serial1.begin(115200);
    Serial1.println();
    pd.init();
    pd.attachSecondEventHandler(secondsTick);
}
 
void loop() {
}

