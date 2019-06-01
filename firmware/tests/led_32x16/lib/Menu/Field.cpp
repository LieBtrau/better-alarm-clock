#include "Menu.h"

    LedMatrixField::LedMatrixField(Max72xxPanel* panel, Coordinate topleft, Coordinate botRight, FieldParameter* par): 
        _panel(panel), _topleft(topleft), _botRight(botRight), _val(par)
    {
        if(topleft.x >= botRight.x || topleft.y >=botRight.y)
        {
            while(true);
        }
    }

    void LedMatrixField::render()
    {
        _panel->drawRect(_topleft.x, _topleft.y, _botRight.x - _topleft.x, _botRight.y - _topleft.y, 1);
        byte maxLength = _botRight.x - _topleft.x -2;
        //_panel->drawLine(_topleft.x+1, _topleft.y+1, maxLength, _topleft.y+1, 0);
        byte length = (_val->cur * maxLength) / (_val->max - _val->min);
        if(length)
        {
            _panel->drawLine(_topleft.x+1, _topleft.y+1, length, _topleft.y+1, 1);
        }
    }

    bool LedMatrixField::increase()
    {
        if( _val->cur > _val->max - _val->step)
        {
            return false;
        }
        _val->cur += _val->step;
        return true;
    }

    bool LedMatrixField::decrease()
    {
        if(_val->cur < _val->min + _val->step)
        {
            return false;
        }
        _val->cur -= _val->step;
        return true;
    }