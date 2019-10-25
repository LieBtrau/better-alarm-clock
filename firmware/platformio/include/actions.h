#pragma once
#include "parameters.h"
#include "SongPlayer.h"
#include "pins.h"
#include "alarmcalendar.h"
#include "millisDelay.h"
#include "MovingAverage.h"

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
    bool initPeripherals();
    void pollActions(bool buttonPressed);
    byte* getSong(ALARMNRS alarmIndex);
    byte* getVolume(ALARMNRS alarmIndex);
    uint16_t getTotalTrackCount();
    void updateAlarmSettings(AlarmConfig *config, ALARMNRS alarmNr);
private:
    const int DISPLAY_DARK_TIMEOUT = 15000;
    bool getFirstAlarmIn(Chronos::Span::Delta delta, ALARMNRS& alarmIndex);
    void displayOnOffControl(bool buttonPressed);
    bool movementDetected();
    bool displayBrightnessControl(int& brightness);
    CommonConfig *pCommon;
    AlarmSettings alarms[2];
    byte _displayBrightness=0xFF;
    byte _desiredDisplayBrightness=5;
    millisDelay _clockRefreshTimer;
    millisDelay _displayOffTimer;
    millisDelay _ambientLightSenseTimer;
    MovingAverage<uint16_t, 16> _brightnessFilter;
};

void playSong(byte i);
void setVolume(byte i);
void stopSong();
void showLightness(byte i);
void stopLightness();
void setLedArrayBrightness(byte i);
