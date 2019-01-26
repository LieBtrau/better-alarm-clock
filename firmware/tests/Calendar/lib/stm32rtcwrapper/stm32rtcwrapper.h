#pragma once
#include <Chronos.h>

class Stm32RtcWrapper
{
  public:
    Stm32RtcWrapper();
    typedef bool (*getExternalTime)(Chronos::EpochTime &epoch);
    void begin();
    Chronos::EpochTime get();
    bool sync();
    void setSyncProvider(getExternalTime getTimeFunction);

  private:
    getExternalTime _getTimePtr = nullptr;
    bool _isTimeSynced = false;
};
