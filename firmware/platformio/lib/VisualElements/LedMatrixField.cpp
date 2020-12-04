#include "Visuals.h"

LedMatrixField::LedMatrixField(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, byte maxValue) : _panel(panel),
                                                                                                              _topleft(topleft),
                                                                                                              _botRight(botRight),
                                                                                                              _max(maxValue)
{
    if(topleft.x >= botRight.x || topleft.y >= botRight.y)
    {
        while (true)
            ;
    }
}

void LedMatrixField::show()
{
    hide();
    _panel->drawLine(_botRight.x, _topleft.y, _botRight.x, _botRight.y, 1);
    _panel->drawLine(_topleft.x, _topleft.y, _topleft.x, _botRight.y, 1);
    byte maxLength = _botRight.x - _topleft.x - 1;
    byte length = (_cur * maxLength) / _max ;
    if (length)
    {
        _panel->fillRect(_topleft.x + 1, _topleft.y, length, _botRight.y - _topleft.y + 1, 1);
    }
}

void LedMatrixField::hide()
{
    _panel->fillRect(_topleft.x, _topleft.y, _botRight.x - _topleft.x + 1, _botRight.y - _topleft.y + 1, 0);
}

void LedMatrixField::setValue(byte cur)
{
    if(cur>_max)
    {
        cur=_max;
    }
    if (cur != _cur)
    {
        _cur = cur;
        updateNeeded = true;
    }
}
