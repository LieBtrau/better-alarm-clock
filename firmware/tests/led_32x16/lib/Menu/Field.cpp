#include "Menu.h"

Field::Field(FieldParameter *par) : _par(par)
{
}

bool Field::increase()
{
    if (*_par->cur > _par->max - _par->step)
    {
        return false;
    }
    *_par->cur += _par->step;
    doAction();
    updateNeeded = true;
    return true;
}

bool Field::decrease()
{
    if (*_par->cur < _par->min + _par->step)
    {
        return false;
    }
    *_par->cur -= _par->step;
    doAction();
    updateNeeded = true;
    return true;
}

void Field::doAction()
{
    if (_par->doAction != nullptr)
    {
        _par->doAction(*_par->cur);
    }
}

void Field::stopAction()
{
    if (_par->stopAction != nullptr)
    {
        _par->stopAction();
    }
}

LedMatrixField::LedMatrixField(Max72xxPanel *panel, Coordinate topleft, Coordinate botRight, FieldParameter *par) : Field(par), _panel(panel), _topleft(topleft), _botRight(botRight)
{
    if (topleft.x >= botRight.x || topleft.y >= botRight.y)
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
    byte length = (*_par->cur * maxLength) / (_par->max - _par->min);
    if (length)
    {
        _panel->fillRect(_topleft.x + 1, _topleft.y, length, _botRight.y - _topleft.y + 1, 1);
    }
}

void LedMatrixField::hide()
{
    _panel->fillRect(_topleft.x-1, _topleft.y-1, _botRight.x - _topleft.x + 3, _botRight.y - _topleft.y + 3, 0);
}

SevenSegmentField::SevenSegmentField(Adafruit_7segment *panel, byte leftPos, FieldParameter *par) : Field(par), _panel(panel), _leftPos(leftPos)
{
    if (leftPos < 0 || leftPos > 4)
    {
        while ((true))
            ;
    }
}

void SevenSegmentField::show()
{
    byte d0 = *_par->cur / 10;
    byte d1 = *_par->cur % 10;
    _panel->writeDigitNum(_leftPos, d0, false);
    _panel->writeDigitNum(_leftPos + 1, d1, false);
}

void SevenSegmentField::hide()
{
    _panel->writeDigitRaw(_leftPos, 0x00);
    _panel->writeDigitRaw(_leftPos + 1, 0x00);
}
