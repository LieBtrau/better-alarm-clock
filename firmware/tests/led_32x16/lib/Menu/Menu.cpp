#include "Menu.h"

bool MenuOut::render()
{
    if(updateNeeded)
    {
        show();
    }
    bool retVal = updateNeeded;
    updateNeeded=false;
    return retVal;
}