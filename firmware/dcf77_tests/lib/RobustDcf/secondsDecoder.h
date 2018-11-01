#pragma once
#include "Arduino.h"
class SecondsDecoder
{
public:
  SecondsDecoder();
  void updateSeconds(const bool isSyncMark, const bool isLongPulse);
  uint8_t getSecond();

private:
  static const uint8_t SECONDS_PER_MINUTE = 60;
  void bounded_increment(int8_t &value, int8_t N);
  bool dataValid(uint64_t data);
  int8_t _bins[SECONDS_PER_MINUTE];
  uint64_t _bitShifter = 0;
  uint8_t _activeBin = 0;
  uint8_t _minuteStartBin = 0;
};