#include <source/robustDcf.h>
#include <TM1637Display.h>

RobustDcf rd(PB6, PC13);
TM1637Display display(PB12, PB13);
void runClock();
void serialParseLoggingTest();
uint64_t hexStringToUint64(String str1);
uint8_t data[] = {SEG_G, SEG_G | SEG_DP, SEG_G, SEG_G};

void setup()
{
    const char compile_date[40] = "Compile time : " __DATE__ " " __TIME__;
    //const char compile_date[] = "hallo";
    Serial.begin(115200);
    Serial.println(compile_date);
    display.setBrightness(1);
    display.setSegments(data);
    rd.init();
}

void loop()
{
    //serialParseLoggingTest();
    runClock();
}

void runClock()
{
    Chronos::EpochTime unixEpoch;
    if (rd.update(unixEpoch))
    {
        Serial.println(unixEpoch);
        TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
        TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
        Timezone myTZ(myDST, mySTD);
        Chronos::DateTime localTime = myTZ.toLocal(unixEpoch);
        localTime.printTo(Serial);
        Serial.println();
        data[0] = display.encodeDigit(localTime.hour() / 10);
        data[1] = display.encodeDigit(localTime.hour() % 10);
        data[1] |= SEG_DP;  //show colon
        data[2] = display.encodeDigit(localTime.minute() / 10);
        data[3] = display.encodeDigit(localTime.minute() % 10);
        display.setSegments(data);
    }
}

/* Run "offline-dcf.py" on your PC and run the following code on the embedded platform to test the functionality of the DCF-decoder
 */
void serialParseLoggingTest()
{
    SecondsDecoder::BITDATA bd;
    bd.validBitCtr = 60;
    Chronos::EpochTime epoch;
    while (Serial.available() > 0)
    {
        String str1 = Serial.readStringUntil('Z');
        uint64_t data = hexStringToUint64(str1);
        if (data)
        {
            bd.bitShifter = data;
            rd.updateClock(&bd, &epoch);
            Serial.println(epoch);
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