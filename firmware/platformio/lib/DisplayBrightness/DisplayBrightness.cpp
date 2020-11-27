#include "DisplayBrightness.h"
#include "AsyncDelay.h"

DisplayBrightness::DisplayBrightness(PirSensor *ps, Adafruit_TSL2591 *tsl) : _ps(ps),
                                                                             _tsl(tsl),
                                                                             _ambientLightSenseTimer(LIGHT_SENSOR_READ_INTERVAL, AsyncDelay::MILLIS),
                                                                             _displayOnTimer(DISPLAY_DARK_TIMEOUT, AsyncDelay::MILLIS)
{
}

bool DisplayBrightness::init()
{
    if (!_tsl->begin())
    {
        return false;
    }
    _tsl->setGain(TSL2591_GAIN_MED);                // 25x gain
    _tsl->setTiming(TSL2591_INTEGRATIONTIME_600MS); // longest integration time (dim light)
    _ps->init();
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
    if (_ps->movementDetected() || buttonPressed || _lastBrightness > 0)
    {
        _displayOnTimer.restart();
        return true;
    }
    return !_displayOnTimer.isExpired();
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
    if (!_ambientLightSenseTimer.isExpired())
    {
        return false;
    }
    _ambientLightSenseTimer.restart();
    uint32_t lum = _tsl->getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    float luxlevel = _tsl->calculateLux(full, ir);
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