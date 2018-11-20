#include "robustDcf.h"

RobustDcf rd(PB6, PC13);

void setup()
{
    Serial1.begin(115200);
    Serial1.println("Hello, I'm awake!");
    // rd.init();
}

void loop()
{
    test();
    // Chronos::EpochTime unixEpoch;
    // if (rd.update(unixEpoch))
    // {
    //     Serial1.println(unixEpoch);
    //     TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
    //     TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
    //     Timezone myTZ(myDST, mySTD);
    //     Chronos::DateTime localTime = myTZ.toLocal(unixEpoch);
    //     localTime.printTo(Serial1);
    //     Serial1.println();
    // }
}

void test()
{
    SecondsDecoder::BITDATA bd;
    bd.validBitCtr = 60;
    Chronos::EpochTime epoch;
    while (Serial1.available() > 0)
    {
        String str1 = Serial1.readStringUntil('Z');
        uint64_t data = hexStringToUint64(str1);
        if (data)
        {
            bd.bitShifter = data;
            rd.updateClock(&bd, &epoch);
            Serial1.println(epoch);
        }
    }
}

uint64_t hexStringToUint64(String str1)
{
    str1.toUpperCase();
    if (str1.startsWith("0x"))
    {
        str1 = str1.substring(2);
    }
    uint64_t data = 0;
    for (uint8_t i = 0; i < str1.length(); i++)
    {
        char c = str1.charAt(i);
        if (c >= '0' && c <= '9')
        {
            data <<= 4;
            data += c - '0';
        }
        if (c >= 'A' && c <= 'F')
        {
            data <<= 4;
            data += c + 10 - 'A';
        }
    }
    return data;
}