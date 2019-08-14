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
    void assignAlarmConfig(ALARMNRS id, AlarmConfig *config);
    bool isDark();
    bool initPeripherals();
    void pollActions(bool buttonPressed);
    byte* getSong(ALARMNRS alarmIndex);
    byte* getVolume(ALARMNRS alarmIndex);
    uint16_t getTotalTrackCount();
    void updateAlarmSettings(AlarmConfig *config, ALARMNRS alarmNr);
    bool getAlarmIn24h(ALARMNRS& alarmIndex);

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
