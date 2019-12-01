#include "DisplayBrightness.h"

DisplayBrightness::DisplayBrightness(byte sda_pin, byte scl_pin, byte pir_pin)
    : i2c(sda_pin, scl_pin), _pir_pin(pir_pin)
{
    _ambientLightSenseTimer.start(1000);
}

bool DisplayBrightness::init()
{
    i2c.setDelay_us(5);
    i2c.begin();
    i2c.setTxBuffer(malloc(50), 50);
    i2c.setRxBuffer(malloc(10), 10);
    if (!als.begin(&i2c))
    {
        return false;
    }
    pinMode(_pir_pin, INPUT);
    return true;
}

/**
 * \brief Determines whether display should be on, based on four inputs:
 *          - user waving at the clock
 *          - a button being pressed
 *          - ambient light level
 *          - timeout
 */
bool DisplayBrightness::isDisplayOn(bool buttonPressed)
{
    if (movementDetected() || buttonPressed || _lastBrightness > 0 || _displayOffTimer.isRunning())
    {
        _displayOffTimer.start(DISPLAY_DARK_TIMEOUT);
        return true;
    }
    return false;
}

/**
 * \brief Set brightness level according to ambient light level
 * \note  Setting occurs fully automatically.  Should a user want to add adjustments, then this function could be changed so that it adds an offset to the selected element in the table.
 *        e.g.  If autobrightness selects index 5, but the user wants a little bit brighter, then the resulting brightness = index + useroffset
 *        Default value for useroffset = 0, but it could be -15 up to +15.
 * \param brightness    will contain a value [0,15] when returning
 * \return true when result valid, else when light sensor is still busy
 */
bool DisplayBrightness::getDisplayBrightness(byte &brightness)
{
    if (!_ambientLightSenseTimer.justFinished())
    {
        return false;
    }
    _ambientLightSenseTimer.repeat();
    float luxlevel = 0; //als.readAlsValue(); -> software I²C interferes with DCF
    if (luxlevel == 0)
    {
        return false;
    }
    const float lightLevels[] = {0.07, 0.13, 0.25, 0.49, 0.97, 1.89, 3.7, 7.24, 14.17, 27.73, 54.26, 106.16, 207.71, 406.4, 795.16, 1555.82};
    float smallestDifference = INT32_MAX;
    for (int i = 0; i < 16; i++)
    {
        int absdiff = abs(luxlevel - lightLevels[i]);
        if (absdiff < smallestDifference)
        {
            smallestDifference = absdiff;
            brightness = i;
        }
    }
    _lastBrightness = brightness;
    return true;
}

bool DisplayBrightness::movementDetected()
{
    return digitalRead(_pir_pin);
}
