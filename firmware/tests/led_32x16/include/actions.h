#pragma once
#include "Arduino.h"

bool isDark();
bool initPeripherals();
void playSong(byte i);
void setVolume(byte i);
void stopSong();
void showLightness(byte i);
void stopLightness();
void setLedArrayBrightness(byte i);
void pollActions();