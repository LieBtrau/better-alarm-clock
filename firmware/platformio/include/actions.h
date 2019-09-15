#pragma once
#include "parameters.h"
#include "SongPlayer.h"
#include "Adafruit_APDS9960.h"
#include "pins.h"
#include "alarmcalendar.h"
#include "millisDelay.h"

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
    bool getFirstAlarmIn(Chronos::Span::Delta delta, ALARMNRS& alarmIndex);
    void displayOnOffControl(bool buttonPressed);
    bool movementDetected();
    void displayBrightnessControl(byte desiredBrightness);
    void updateDesiredDisplayBrightness();
    Adafruit_APDS9960 apds;
    CommonConfig *pCommon;
    AlarmSettings alarms[2];
    byte _displayBrightness=0xFF;
    byte _desiredDisplayBrightness=5;
    millisDelay _clockRefreshTimer;
    millisDelay _brightnessRampTimer;
    millisDelay _displayOffTimer;
    millisDelay _ambientLightSenseTimer;
};

void playSong(byte i);
void setVolume(byte i);
void stopSong();
void showLightness(byte i);
void stopLightness();
void setLedArrayBrightness(byte i);
