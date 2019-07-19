#pragma once
#include "parameters.h"
#include "SongPlayer.h"
#include "Adafruit_APDS9960.h"
#include "pins.h"

class ActionMgr
{
public:
    void assignCommonConfig(CommonConfig *pConfig);
    void assignAlarmConfig(AlarmConfig *config);
    bool isDark();
    bool initPeripherals();
    void pollActions();
    uint16_t getTotalTrackCount();

private:
    Adafruit_APDS9960 apds;
    CommonConfig *pCommon;
};

void playSong(byte i);
void setVolume(byte i);
void stopSong();
void showLightness(byte i);
void stopLightness();
void setLedArrayBrightness(byte i);
