#pragma once
#include "Menu.h"
#include "parameters.h"

class MenuMgr
{
public:
    void assignCommonConfig(CommonConfig *pConfig);
    void assignAlarmConfig(AlarmConfig *config);
    void setBrightness(byte i);
    void initMenu(byte totalTrackCount);
    void showParameterMenu();
    void showSplash();
    void pollMenu();
    void showAlarm(byte alarmNr);
    void setDisplayOn(bool on);
private:
    void showLedState();
};

void showClock(bool action);