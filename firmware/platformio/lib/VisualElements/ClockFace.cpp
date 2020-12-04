#include "Visuals.h"
#include "fontBig.h"

ClockFace::ClockFace(Max72xxPanel *panel) : _panel(panel) {}

void ClockFace::setTime(byte hours, byte minutes, bool synced)
{
    if (hours != _hours || minutes != _minutes)
    {
        updateNeeded = true;
    }
    _hours = hours;
    _minutes = minutes;
    _synced = synced;
}

void ClockFace::show()
{
    hide();
    byte font = 4; //0 to 4
    _panel->drawBitmap(-3, 1, bigFont[(_hours / 10 != 0 ? _hours / 10 : 10) + 11 * font], 8, 12, 1);
    _panel->drawBitmap(5, 1, bigFont[(_hours % 10) + 11 * font], 8, 12, 1);
    _panel->drawBitmap(16, 1, bigFont[(_minutes / 10 != 0 ? _minutes / 10 : 10) + 11 * font], 8, 12, 1);
    _panel->drawBitmap(24, 1, bigFont[(_minutes % 10) + 11 * font], 8, 12, 1);
    if (_synced)
    {
        _panel->fillRect(15, 4, 2, 2, 1);
        _panel->fillRect(15, 10, 2, 2, 1);
    }
    else
    {
        _panel->fillRect(15, 7, 2, 2, 1);
    }
}

void ClockFace::hide()
{
    _panel->fillScreen(0);
}
