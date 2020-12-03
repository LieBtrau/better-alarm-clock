#include "Visuals.h"

SevenSegmentField::SevenSegmentField(Adafruit_7segment *panel) : _panel(panel)
{
}

bool SevenSegmentField::init(byte leftPos)
{
    if (leftPos < 0 || leftPos > 4)
    {
        return false;
    }
    _leftPos = leftPos;
    _panel->begin(0x70); // pass in the address
    return true;
}

void SevenSegmentField::show()
{
    _panel->writeDigitNum(_leftPos, (_data / 10) % 10);
    _panel->writeDigitNum(_leftPos + 1, _data % 10);
}

void SevenSegmentField::hide()
{
    _panel->writeDigitRaw(_leftPos, 0x00);
    _panel->writeDigitRaw(_leftPos + 1, 0x00);
}

void SevenSegmentField::setValue(byte data)
{
    _data = data;
    updateNeeded=true;
}

void SevenSegmentField::setBrightness(byte brightness)
{
    _panel->setBrightness(brightness & 0xF);
    updateNeeded=true;
}
