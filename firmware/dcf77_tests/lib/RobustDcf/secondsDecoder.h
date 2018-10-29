#pragma once
#include "Arduino.h"
class SecondsDecoder
{
  public:
    typedef enum
    {
        SYNC_MARK = 0,
        SHORT_PULSE = 1,
        UNDEFINED = 2,
        LONG_PULSE = 3
    }PULSE_TYPES;
    SecondsDecoder();
    void updateSeconds(const PULSE_TYPES tick_data);
    uint8_t getSecond();
  private:
    static const uint8_t SECONDS_PER_MINUTE = 60;
    static const uint8_t LOCK_THRESHOLD = 6;
    void bounded_increment(uint8_t &value, uint8_t N);
    void bounded_decrement(uint8_t &value, uint8_t N);
    uint8_t _data[SECONDS_PER_MINUTE];
    uint8_t _activeBin=0;
    uint8_t _max=0;
    uint8_t _max_index=0;
    uint8_t _noise_max=0;
};