#pragma once
#include "Arduino.h"
#include "secondsDecoder.h"
#include "bin.h"

class TimeZoneDecoder
{
public:
  TimeZoneDecoder();
  bool update(SecondsDecoder::BITDATA *data);
  bool getSecondsOffset(int16_t &offset, uint8_t& hour, uint8_t minute);

private:
  static const uint8_t NR_OR_TIMEZONES = 2;
  static const uint8_t STARTBIT = 16;
  static const int8_t THRESHOLD = 1;
  static const uint32_t TIMEZONE_CHANGE_BIT = 0x10000;
  static const uint32_t CEST_BIT = 0x20000;
  static const uint32_t CET_BIT = 0x40000;
  static const int16_t ONE_HOUR = 3600;
  static const int16_t TWO_HOURS = 7200;
  uint8_t _timeZoneChangeAnnounced = 0;
  int8_t _isSummerTime = 0;
  bool _isPredictionCEST = false;
};