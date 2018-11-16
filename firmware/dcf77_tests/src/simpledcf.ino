#include "robustDcf.h"

RobustDcf rd(PB6, PC13);

void setup()
{
    Serial1.begin(115200);
    Serial1.println("Hello, I'm awake!");
    rd.init();
}

void loop()
{
    Chronos::EpochTime unixEpoch;
    if (rd.update(unixEpoch))
    {
        Serial1.println(unixEpoch);
        TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
        TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
        Timezone myTZ(myDST, mySTD);
        Chronos::DateTime localTime = myTZ.toLocal(unixEpoch);
        localTime.printTo(Serial1);
        Serial1.println();
    }
}