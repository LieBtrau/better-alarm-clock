#pragma once
#include "phaseDetector.h"
#include "secondsDecoder.h"
#include "bcdDecoder.h"
#include "timezoneDecoder.h"
#include <Timezone.h>
#include <Chronos.h>

class RobustDcf
{
public:
  RobustDcf(const byte inputPin, const byte monitorPin = 0);
  void init();
  bool update(Chronos::EpochTime &unixEpoch);
  bool updateClock(SecondsDecoder::BITDATA *data, Chronos::EpochTime *pEpoch);

private:
  bool getUnixEpochTime(Chronos::EpochTime* unixEpoch);
  PhaseDetector _pd;
  SecondsDecoder _sd;
  BcdDecoder _minutes, _hours, _days, _months, _years;
  TimeZoneDecoder _tzd;
};