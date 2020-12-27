#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "PirSensor.h"
#include "DisplayBrightness.h"
#include "DisplayOnOff.h"
#include "pins.h"

static PirSensor ps(pin_PIR);
static Adafruit_TSL2591 tsl(2591);
static DisplayBrightness db(&ps, &tsl);
static DISPLAY_STATE displayState = DISPLAY_OFF;
static AsyncDelay delayedOffTimer(10000, AsyncDelay::MILLIS);

bool initDisplayOnOffControl()
{
    return db.init();
}

DISPLAY_STATE getDisplayState(bool buttonPressed, bool newAlarmOngoing)
{
    bool displayOn = db.isDisplayOn() || buttonPressed || newAlarmOngoing;
    switch (displayState)
    {
    case DISPLAY_OFF:
        if (displayOn)
        {
            displayState = DISPLAY_TURNED_ON;
            delayedOffTimer.restart();
        }
        break;
    case DISPLAY_TURNED_ON:
        displayState = displayOn || !delayedOffTimer.isExpired() ? DISPLAY_ON : DISPLAY_TURNED_OFF;
        break;
    case DISPLAY_ON:
        if (!displayOn && delayedOffTimer.isExpired())
        {
            displayState = DISPLAY_TURNED_OFF;
        }
        break;
    case DISPLAY_TURNED_OFF:
        displayState = displayOn ? DISPLAY_TURNED_ON : DISPLAY_OFF;
        break;
    default:
        displayState = DISPLAY_OFF;
        break;
    }

    return displayState;
}

byte getDisplayBrightness(bool newAlarmOngoing)
{
    if (newAlarmOngoing)
    {
        return 15;
    }
    return db.getDisplayBrightness();
}
