#pragma once
#include "Menu.h"

extern Max72xxPanel matrix;
extern Adafruit_7segment matrix7;

void initMenu();
void animation();
void showParameterMenu(bool isFlashing);
void showSplash();
void showClock(bool action);
bool pollMenu();