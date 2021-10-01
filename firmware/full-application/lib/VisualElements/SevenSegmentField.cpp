#include "Visuals.h"

SevenSegmentField::SevenSegmentField(Adafruit_7segment *panel, byte leftPos) : _panel(panel)
{
    if (leftPos < 0 || leftPos > 4)
    {
        return;
    }
    _leftPos = leftPos;
}

void SevenSegmentField::show()
{
    byte tens = (_data / 10) % 10;
    if (!tens)
    {
        _panel->writeDigitRaw(_leftPos, 0x00);
    }
    else
    {
        _panel->writeDigitNum(_leftPos, tens);
    }
    _panel->writeDigitNum(_leftPos + 1, _data % 10);
    _panel->drawColon(true);
}

void SevenSegmentField::hide()
{
    _panel->writeDigitRaw(_leftPos, 0x00);
    _panel->writeDigitRaw(_leftPos + 1, 0x00);
    _panel->drawColon(false);
}

void SevenSegmentField::setValue(byte data)
{
    if (_data != data)
    {
        updateNeeded = true;
    }
    _data = data;
}
