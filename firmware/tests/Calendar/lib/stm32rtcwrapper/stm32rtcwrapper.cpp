#include "stm32rtcwrapper.h"
#include <utility/rtc_util.h>

void Stm32RtcWrapper::begin()
{
    rtc_init(RTCSEL_LSE);
    rtc_set_prescaler_load(0x7fff);
}

time_t Stm32RtcWrapper::get()
{
    return rtc_get_count();
}

bool Stm32RtcWrapper::set(time_t t)
{
    rtc_set_count(t);
    return true;
}
