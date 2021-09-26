#pragma once
#include <Chronos.h>

//No sync functionality in this class, because the RTC will be updated by DCF.  DCF can't be polled.  DCF pushes its time once a minute.
class Stm32RtcWrapper
{
  public:
    Stm32RtcWrapper();
    void begin();
    bool get(Chronos::EpochTime& epoch);
    void setEpoch(Chronos::EpochTime epoch);
  private:
    Chronos::DateTime _lastSuccessfulSync;
    const Chronos::Span::Hours DEADRECKONING = 25;
};
