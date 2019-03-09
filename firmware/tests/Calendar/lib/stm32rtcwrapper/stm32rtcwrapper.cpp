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

Chronos::EpochTime Stm32RtcWrapper::get()
{
    return _isTimeSynced ? rtc.getEpoch() : 0;
}

bool Stm32RtcWrapper::setEpoch(Chronos::EpochTime epoch)
{
    rtc.setEpoch(epoch);
    _isTimeSynced = true;
    return true;
}