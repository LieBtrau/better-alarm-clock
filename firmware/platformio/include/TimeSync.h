#pragma once

void initClockSource();
bool getLocalTimeSeconds(time_t& localTime);
bool isNewMinuteStarted(time_t t, byte &hours, byte &minutes);
bool isStillSynced();