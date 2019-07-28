#pragma once
#include <Chronos.h>

//No sync functionality in this class, because the RTC will be updated by DCF.  DCF can't be polled.  DCF pushes its time once a minute.
class Stm32RtcWrapper
{
  public:
    Stm32RtcWrapper();
    void begin();
    Chronos::EpochTime get();
    bool setEpoch(Chronos::EpochTime epoch);
  private:
    bool _isTimeSynced = false;
};
