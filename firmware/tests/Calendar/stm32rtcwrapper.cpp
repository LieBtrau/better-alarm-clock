#include "stm32rtcwrapper.h"

//don't make static, because it will no longer work
STM32RTC& rtc = STM32RTC::getInstance();

void Stm32RtcWrapper::begin()
{
    rtc.setClockSource(STM32RTC::RTC_LSE_CLOCK);
    rtc.begin(STM32RTC::RTC_HOUR_24); // initialize RTC 24H format
}

time_t Stm32RtcWrapper::get()
{
    return rtc.getEpoch();
}

bool Stm32RtcWrapper::set(time_t t)
{
    rtc.setEpoch(t);
    return true;
}
