#include "DisplayBrightness.h"

DisplayBrightness::DisplayBrightness(byte pir_pin) : _pir_pin(pir_pin)
{
}

bool DisplayBrightness::init()
{
    if (!tsl.begin())
    {
        return false;
    }
    _ambientLightSenseTimer.start(10000);
    _displayOffTimer.start(DISPLAY_DARK_TIMEOUT);
    tsl.setGain(TSL2591_GAIN_MED);                // 25x gain
    tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS); // longest integration time (dim light)
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
    if (movementDetected() || buttonPressed || _lastBrightness > 0)
    {
        _displayOffTimer.restart();
        return true;
    }
    if(!_displayOffTimer.justFinished() && _displayOffTimer.isRunning())
    {
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
    _ambientLightSenseTimer.restart();
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    float luxlevel = tsl.calculateLux(full, ir);
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
    //return digitalRead(_pir_pin);
    return false;
}
