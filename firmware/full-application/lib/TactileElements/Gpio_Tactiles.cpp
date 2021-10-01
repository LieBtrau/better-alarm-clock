#include "Tactiles.h"

static AsyncDelay _keyReleaseTimeout(100, AsyncDelay::MILLIS);

static void event_gpio_changed()
{
    _keyReleaseTimeout.restart();
}

Gpio_Tactiles::Gpio_Tactiles(byte pinGpio, int pin_mode) : _pinGpio(pinGpio)
{
    pinMode(_pinGpio, pin_mode);
    attachInterrupt(digitalPinToInterrupt(_pinGpio), event_gpio_changed, CHANGE);
}

Gpio_Tactiles::~Gpio_Tactiles()
{
}

bool Gpio_Tactiles::isChanged()
{
    if(_keyReleaseTimeout.isExpired())
    {
        int currentGpioState = digitalRead(_pinGpio);
        if(currentGpioState != _lastGpioState)
        {
            _lastGpioState = currentGpioState;
            return true;
        }
    }
    return false;
}

int Gpio_Tactiles::getValue()
{
    return _lastGpioState;
}