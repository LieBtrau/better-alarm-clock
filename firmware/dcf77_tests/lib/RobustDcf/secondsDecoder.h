/* The secondsDecoder finds the start of a minute inside the pulse data (long/short/none) that it gets from the phase decoder.
 * It also holds the databits for the next minute and transfers these on the start of each minute to other objects.
 */
#pragma once
#include "Arduino.h"
class SecondsDecoder
{
public:
  SecondsDecoder();
  void updateSeconds(const bool isSyncMark, const bool isLongPulse);
  bool getSecond(uint8_t& second);
  bool getTimeData(uint64_t& data);
private:
  static const uint8_t SECONDS_PER_MINUTE = 60;
  static const int8_t LOCK_THRESHOLD = 5;
  void bounded_increment(int8_t &value, int8_t N);
  bool dataValid(uint64_t x);
  int8_t _bins[SECONDS_PER_MINUTE];
  uint64_t _bitShifter = 0;
  uint64_t _prevBitShifter = 0;
  uint8_t _activeBin = 0;
  uint8_t _minuteStartBin = 0xFF;
};