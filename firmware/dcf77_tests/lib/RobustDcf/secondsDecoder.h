/* The secondsDecoder finds the start of a minute inside the pulse data (long/short/none) that it gets from the phase decoder.
 * It also holds the databits for the next minute and transfers these on the start of each minute to other objects.
 */
#pragma once
#include "Arduino.h"
#include "bin.h"
class SecondsDecoder
{
public:
  typedef struct
  {
    uint64_t bitShifter;
    uint8_t validBitCtr;
  } BITDATA;
  static const uint8_t SECONDS_PER_MINUTE = 60;
  SecondsDecoder();
  void updateSeconds(const bool isSyncMark, const bool isLongPulse);
  bool getSecond(uint8_t &second);
  bool getTimeData(BITDATA *pdata);

private:
  static const int8_t LOCK_THRESHOLD = 5;
  bool dataValid(uint64_t x);
  Bin _bin;
  uint8_t _activeBin = 0;
  BITDATA _curData = {0, 0};
  BITDATA _prevData = {0, 0};
  uint8_t _minuteStartBin = 0xFF;
};