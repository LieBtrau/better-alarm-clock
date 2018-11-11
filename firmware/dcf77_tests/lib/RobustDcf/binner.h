/*  The Binner gets some BCD-encoded data bytes of the SecondsDecoder, checks validity and converts these to decimal values.
 */
#pragma once
#include "Arduino.h"

class Binner
{
  public:
    Binner(uint8_t startBit, uint8_t bitWidth, bool withParity, uint8_t lowestValue, uint8_t highestValue, int8_t lockThreshold);
    void update(uint64_t newData);
    void advanceTick();
    bool getTime(uint8_t& value);
  private:
    uint8_t bcd2int(uint8_t bcd);
    uint8_t int2bcd(uint8_t hex);
    bool parityOdd(uint8_t x);
    int hammingWeight(int i);
    void bounded_increment(int8_t &value, int8_t N);
    uint8_t getValueInRange(uint8_t i);
    uint8_t _startBit;
    uint8_t _bitWidth;
    bool _withParity;
    uint8_t _lowestValue;
    uint8_t _highestValue;
    int8_t* _pData=nullptr;
    uint8_t _dataSize=0;
    uint8_t _currentTick=0;
    int8_t _lockThreshold=0;
};