#include "Menu.h"

bool MenuOut::render()
{
    if(updateNeeded)
    {
        show();
    }
    updateNeeded=false;
}