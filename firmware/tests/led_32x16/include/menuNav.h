#pragma once
#include "Menu.h"
#include "parameters.h"

extern Max72xxPanel matrix;
extern Adafruit_7segment matrix7;

void initMenu(byte totalTrackCount);
void animation();
void showParameterMenu(bool isFlashing);
void showSplash();
void showClock(bool action);
bool pollMenu();

class MenuMgr
{
public:
    void assignCommonConfig(CommonConfig *pConfig);
    void assignAlarmConfig(AlarmConfig *config);

private:
};