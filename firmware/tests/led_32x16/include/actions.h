#pragma once
#include "parameters.h"

void assignActionsConfig(CommonConfig *pConfig, AlarmConfig *config);
bool isDark();
bool initPeripherals();
void playSong(byte i);
void setVolume(byte i);
void stopSong();
void showLightness(byte i);
void stopLightness();
void setLedArrayBrightness(byte i);
void pollActions();