#pragma once
#include "Menu.h"
#include "parameters.h"
#include "millisDelay.h"

class MenuMgr
{
public:
    void assignCommonConfig(CommonConfig *pConfig);
    void assignAlarmConfig(AlarmConfig *config);
    void showFirstAlarm(AlarmConfig* config);
    void setBrightness(byte i);
    void setSessionBrightness(byte i);
    void brightnessSession(bool start);
    void initMenu(byte totalTrackCount);
    void showParameterMenu(bool visible);
    void showSyncAnimation();
    void showSplash();
    bool pollMenu();
    void showAlarm(byte alarmNr);
    bool isVisible();
private:
    void showLedState();
    bool _visible = false;
    byte _brightness = 0xFF;
    bool _brightnessSessionBusy = false;
    millisDelay _charliePlexingTimer;
    millisDelay _syncingTimer;
};

void showClock(bool action);