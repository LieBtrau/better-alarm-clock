#pragma once
#include "Menu.h"
#include "parameters.h"

class MenuMgr
{
public:
    void assignCommonConfig(CommonConfig *pConfig);
    void assignAlarmConfig(AlarmConfig *config);
    void setFirstAlarm(AlarmConfig* config);
    void setBrightness(byte i);
    void setSessionBrightness(byte i);
    void brightnessSession(bool start);
    void initMenu(byte totalTrackCount);
    void showParameterMenu(bool visible);
    void showSplash();
    bool pollMenu();
    void showAlarm(byte alarmNr);

private:
    void showLedState();
    bool _visible = false;
    byte _brightness;
    bool _brightnessSessionBusy = false;
};

void showClock(bool action);