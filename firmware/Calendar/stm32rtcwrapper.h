#pragma once
#include <Time.h>
#include <STM32RTC.h>


class Stm32RtcWrapper
{
public:
    void begin();
    static time_t get();
    static bool set(time_t t);
};
