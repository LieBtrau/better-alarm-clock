#pragma once

void initClock();
bool getLocalTimeSeconds(time_t& localTime);
bool splitTime(time_t t, byte &hours, byte &minutes);
bool isStillSynced();