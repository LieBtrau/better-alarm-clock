#pragma once

#include "millisDelay.h"
#include "SoftWire.h"
#include "DFRobot_VEML7700.h"

class DisplayBrightness
{
    public:
    DisplayBrightness(byte sda_pin, byte scl_pin, byte pir_pin);
    bool init();
    bool getDisplayBrightness(byte& brightness);
    bool isDisplayOn(bool buttonPressed);
private:
    const int DISPLAY_DARK_TIMEOUT = 15000;
    bool movementDetected();
    byte _displayBrightness=0xFF;
    byte _desiredDisplayBrightness=5;
    millisDelay _clockRefreshTimer;
    millisDelay _displayOffTimer;
    millisDelay _ambientLightSenseTimer;
    SoftWire i2c;
    DFRobot_VEML7700 als;
    byte _pir_pin;
    int _lastBrightness=0;
};