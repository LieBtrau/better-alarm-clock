#include "LedDriverDimming.h"

LedDriverDimming::LedDriverDimming(byte pinEnable, byte pinPwmH, byte pinPwmL) : _pinEn(pinEnable),
                                                                                 _pinPwmH(pinPwmH),
                                                                                 _pinPwmL(pinPwmL)
{
}

LedDriverDimming::~LedDriverDimming()
{
}

void LedDriverDimming::init()
{
    pinMode(_pinPwmH, OUTPUT);
    pinMode(_pinPwmL, OUTPUT);
    pinMode(_pinEn, OUTPUT);
    analogWrite(_pinPwmH, 255);
    analogWrite(_pinPwmL, 255);
    analogWrite(_pinEn, 0); //disabled by default
}

void LedDriverDimming::enable(bool enable)
{
    analogWrite(_pinEn, enable ? 255 : 0);
}

/* Set perceived brightness.
 * \param perceivedBrightness : 0% to 100%
 * \return true when success, false when brightness was out of range.
 */
bool LedDriverDimming::setBrightness(float perceivedBrightness)
{
    if (perceivedBrightness < 0 || perceivedBrightness > 100)
    {
        return false;
    }
    float multiplicator = (MAX_PWM_VALUE - MIN_PWM_VALUE) / 10000;
    int lightValue = MAX_PWM_VALUE - (multiplicator * perceivedBrightness * perceivedBrightness);
    analogWrite(_pinEn, lightValue > 64000 ? 1 : 255);
    analogWrite(_pinPwmH, highByte(lightValue));
    analogWrite(_pinPwmL, lowByte(lightValue));
    return true;
}
