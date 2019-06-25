#include "Menu.h"

LedMatrixSelect::LedMatrixSelect(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, SelectParameter *par) : _panel(panel), _topleft(topleft), _botRight(botRight), _par(par)
{
    if (topleft.x >= botRight.x || topleft.y >= botRight.y)
    {
        while (true)
            ;
    }
    if(par->min >= par->max)
    {
        while(true);
    }
}

void LedMatrixSelect::show()
{
    hide();
    _panel->drawLine(_botRight.x, _topleft.y, _botRight.x, _botRight.y, 1);
    _panel->drawLine(_topleft.x, _topleft.y, _topleft.x, _botRight.y, 1);
    _panel->drawLine(_topleft.x + *_par->cur, _topleft.y, _topleft.x + *_par->cur, _botRight.y, 1);
}

void LedMatrixSelect::hide()
{
    _panel->fillRect(_topleft.x, _topleft.y, _botRight.x - _topleft.x, _botRight.y - _topleft.y + 1, 0);
}

bool LedMatrixSelect::increase()
{
    *_par->cur = *_par->cur < _par->max ? *_par->cur + 1 : _par->min; //at maximum, loop back to minimum;
    doAction();
    updateNeeded = true;
    return true;
}

bool LedMatrixSelect::decrease()
{
    *_par->cur = *_par->cur > _par->min ? *_par->cur - 1 : _par->max; //at maximum, loop back to minimum;
    doAction();
    updateNeeded = true;
    return true;
}

void LedMatrixSelect::doAction()
{
    if(_par->doAction != nullptr)
    {
        _par->doAction(*_par->cur);
    }
}

void LedMatrixSelect::stopAction()
{
    if(_par->stopAction != nullptr)
    {
        _par->stopAction();
    }
}
