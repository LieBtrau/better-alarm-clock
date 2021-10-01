#include "stm32rtcwrapper.h"
#include <STM32RTC.h>

static STM32RTC& rtc = STM32RTC::getInstance();

Stm32RtcWrapper::Stm32RtcWrapper()
{
}

void Stm32RtcWrapper::begin()
{
    rtc.setClockSource(STM32RTC::LSE_CLOCK);
    rtc.begin();
}

bool Stm32RtcWrapper::get(Chronos::EpochTime& epoch)
{
    epoch = rtc.getEpoch();
    return Chronos::DateTime(epoch) - _lastSuccessfulSync < DEADRECKONING;
}

void Stm32RtcWrapper::setEpoch(Chronos::EpochTime epoch)
{
    rtc.setEpoch(epoch);
    _lastSuccessfulSync = Chronos::DateTime(epoch);
}