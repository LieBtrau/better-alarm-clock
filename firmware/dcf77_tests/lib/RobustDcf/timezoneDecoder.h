#pragma once
#include "Arduino.h"
#include "secondsDecoder.h"
#include "bin.h"

class TimeZoneDecoder
{
  public:
    TimeZoneDecoder();
    bool update(SecondsDecoder::BITDATA *data);
    bool getSecondsOffset(int16_t &offset);
    void setPrediction(bool isSummerTime);

  private:
    static const uint8_t NR_OR_TIMEZONES=2;
    static const uint8_t STARTBIT = 17;
    static const int8_t THRESHOLD = 1;
    static const uint32_t CEST_BIT = 0x20000;
    static const uint32_t CET_BIT = 0x40000;
    static const int16_t ONE_HOUR=3600;
    static const int16_t TWO_HOURS=7200;
    Bin _bin;
    bool _isPredictionCEST=false;
};