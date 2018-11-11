#include "phaseDetector.h"
#include "secondsDecoder.h"
#include "binner.h"

PhaseDetector pd(PB6, PC13);
SecondsDecoder sd;
volatile bool secondTicked = false;
Binner minutes(21, 7, true, 0, 59, 4);
bool syncMark, longPulse;

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
            uint64_t data = 0;
            uint8_t minute = 0;
            sd.getTimeData(data);
            Serial1.println();
            Serial1.println(data, BIN);
            minutes.update(data);
            if (minutes.getTime(minute))
            {
                Serial1.println(minute);
            }
            minutes.advanceTick();
        }
    }
}
