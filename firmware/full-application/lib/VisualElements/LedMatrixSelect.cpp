#include "Visuals.h"

LedMatrixSelect::LedMatrixSelect(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, byte max) : 
    _panel(panel), 
    _topleft(topleft), 
    _botRight(botRight), 
    _max(max)
{
    if (topleft.x >= botRight.x || topleft.y >= botRight.y)
    {
        while (true)
            ;
    }
}

void LedMatrixSelect::show()
{
    hide();
    _panel->drawLine(_botRight.x, _topleft.y, _botRight.x, _botRight.y, 1);
    _panel->drawLine(_topleft.x, _topleft.y, _topleft.x, _botRight.y, 1);
    byte offset = _cur * (_botRight.x - _topleft.x - 1) / _max;
    _panel->drawLine(_topleft.x + 1 + offset, _topleft.y, _topleft.x + 1 + offset, _botRight.y, 1);
}

void LedMatrixSelect::hide()
{
    _panel->fillRect(_topleft.x, _topleft.y, _botRight.x - _topleft.x + 1, _botRight.y - _topleft.y + 1, 0);
}

//\param cur 0 to max-1
void LedMatrixSelect::setValue(byte cur)
{
    if(cur>=_max)
    {
        cur=_max-1;
    }
    if (cur != _cur)
    {
        _cur = cur;
        updateNeeded = true;
    }
}
