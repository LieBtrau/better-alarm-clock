#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
#include "PirSensor.h"
#include "DisplayBrightness.h"
#include "pins.h"

static PirSensor ps(pin_PIR);
static Adafruit_TSL2591 tsl(2591);
static DisplayBrightness db(&ps, &tsl);
static bool previousDisplayOnState = false;
static bool currentDisplayOnState = false;

bool initDisplayOnOffControl()
{
    return db.init();
}

bool displayTurnedOn(bool buttonPressed)
{
    currentDisplayOnState = db.isDisplayOn(buttonPressed);
    return !previousDisplayOnState && currentDisplayOnState;
}

bool isDisplayOn(bool buttonPressed)
{
    currentDisplayOnState = db.isDisplayOn(buttonPressed);
    return currentDisplayOnState;
}

bool getDisplayBrightness(byte &brightness)
{
    return db.getDisplayBrightness(brightness);
}
