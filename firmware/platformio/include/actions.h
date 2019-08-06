#pragma once
#include "parameters.h"
#include "SongPlayer.h"
#include "Adafruit_APDS9960.h"
#include "pins.h"
#include "alarmcalendar.h"

class ActionMgr
{
    struct AlarmSettings
    {
        AlarmCalendar calendar;
        AlarmConfig *config;
    };

public:
    ActionMgr();
    void assignCommonConfig(CommonConfig *pConfig);
    void assignAlarmConfig(byte id, AlarmConfig *config);
    bool isDark();
    bool initPeripherals();
    void pollActions();
    byte* getSong(byte alarmIndex);
    byte* getVolume(byte alarmIndex);
    uint16_t getTotalTrackCount();
    void updateAlarmSettings(AlarmConfig *config, byte alarmNr);

private:
    Adafruit_APDS9960 apds;
    CommonConfig *pCommon;
    AlarmSettings alarms[2];
};

void playSong(byte i);
void setVolume(byte i);
void stopSong();
void showLightness(byte i);
void stopLightness();
void setLedArrayBrightness(byte i);
