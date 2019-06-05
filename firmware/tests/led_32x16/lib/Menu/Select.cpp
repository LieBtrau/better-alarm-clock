#include "Menu.h"

LedMatrixSelect::LedMatrixSelect(Max72xxPanel* panel, Coordinate topleft, Coordinate botRight, SelectParameter* par):
    _panel(panel), _topleft(topleft), _botRight(botRight), _par(par)
{
    if(topleft.x >= botRight.x || topleft.y >=botRight.y)
    {
        while(true);
    }
    if(botRight.x - topleft.x - 2 <= _par->max)
    {
        while(true);
    }
}

void LedMatrixSelect::show()
{
    hide();
    _panel->drawRect(_topleft.x, _topleft.y, _botRight.x - _topleft.x, _botRight.y - _topleft.y, 1);
    _panel->drawPixel(_topleft.x + 1 + _par->cur, _topleft.y+1,1);
}

void LedMatrixSelect::hide()
{
    _panel->drawRect(_topleft.x, _topleft.y, _botRight.x - _topleft.x, _botRight.y - _topleft.y, 0);
    _panel->drawLine(_topleft.x + 1, _topleft.y + 1, _botRight.x - 1, _topleft.y + 1, 0);
}

void LedMatrixSelect::next()
{
    _par->cur = _par->cur < _par->max ? _par->cur + 1 : 0;  //at maximum, loop back to minimum;
    _par->doAction(_par->cur);
    updateNeeded = true;
}

void LedMatrixSelect::prev()
{
    _par->cur = _par->cur > 0 ? _par->cur - 1 : _par->max;  //at maximum, loop back to minimum;
    _par->doAction(_par->cur);
    updateNeeded = false;
}