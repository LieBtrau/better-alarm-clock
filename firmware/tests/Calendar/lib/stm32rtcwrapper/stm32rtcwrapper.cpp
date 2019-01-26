#include "stm32rtcwrapper.h"
#include <utility/rtc_util.h>

Stm32RtcWrapper::Stm32RtcWrapper()
{
}

void Stm32RtcWrapper::begin()
{
    rtc_init(RTCSEL_LSE);
    rtc_set_prescaler_load(0x7fff);
}

Chronos::EpochTime Stm32RtcWrapper::get()
{
    return _isTimeSynced ? rtc_get_count() : 0;
}

bool Stm32RtcWrapper::sync()
{
    Chronos::EpochTime epoch;
    if ((!_getTimePtr) || (!_getTimePtr(epoch)))
    {
        return false;
    }
    rtc_set_count(epoch);
    _isTimeSynced = true;
    return true;
}

void Stm32RtcWrapper::setSyncProvider(getExternalTime getTimeFunction)
{
    _getTimePtr = getTimeFunction;
}
