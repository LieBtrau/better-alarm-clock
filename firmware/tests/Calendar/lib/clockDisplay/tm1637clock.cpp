#include "tm1637clock.h"
#include "Arduino.h"

tm1637clock::tm1637clock(int pin1, int pin2) : display(pin1, pin2)
{
}

void tm1637clock::init()
{
    display.setBrightness(1);
    display.setSegments(data);
}

void tm1637clock::showTime(int hour, int minutes)
{
    _minutes = minutes;
    data[0] = display.encodeDigit(hour / 10);
    data[1] = (data[1] & SEG_DP) | display.encodeDigit(hour % 10);
    data[2] = display.encodeDigit(minutes / 10);
    data[3] = display.encodeDigit(minutes % 10);
    display.setSegments(data);
}

void tm1637clock::clearDisplay()
{
    display.clear();
}

void tm1637clock::setBrightness(int brightness)
{
    display.setBrightness(brightness);
}

void tm1637clock::setColon(bool colonOn)
{
    data[1] &= ~SEG_DP;
    data[1] |= colonOn ? SEG_DP : 0;
    display.setSegments(data);
}

int tm1637clock::getMinutes()
{
    return _minutes;
}