#pragma once
#include "Menu.h"

void initMenu();
void animation();
void renderMenu();

struct CommonParameters
{
    FieldParameter dayBright = {0, 50, 100, 5};
    FieldParameter dayNight = {0, 50, 100, 5};
    FieldParameter nightBright = {0, 50, 100, 5};
};
extern CommonParameters compar;
