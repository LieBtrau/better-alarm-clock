#pragma once
#include "Arduino.h"

class LedDriverDimming
{
public:
    LedDriverDimming(byte pinEnable, byte pinPwmH, byte pinPwmL);
    ~LedDriverDimming();
    void init();
    void enable(bool enable);
    bool setBrightness(float brightness);
private:
    const float MAX_PWM_VALUE = 65535;
    const float MIN_PWM_VALUE = 12500;
    byte _pinEn;
    byte _pinPwmH;
    byte _pinPwmL;
};

