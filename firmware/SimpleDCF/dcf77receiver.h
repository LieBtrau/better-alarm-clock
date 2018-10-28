/*
 * http://firefly-power.de/ARM/DCF77.c.HTML.htm
 */

#pragma once

#include "Chronos.h"

class Dcf77Receiver
{
public:
    Dcf77Receiver(byte dcfPin);
    void init();
    bool getLocalTime(Chronos::DateTime& localTime);
    void sampleDcf();
private:
    const unsigned long MIN_MISSING_PULSE_PERIOD = 1500;
    const unsigned long MIN_PULSE_PERIOD = 700;
    const unsigned long MIN_PULSE_LENGTH = 50;
    const unsigned long MAX_SHORT_PULSE_LENGTH = 180;
    const unsigned long ALFA=50;
    static const byte RXBUFSIZE=20;
    byte _dcfPin;
    long _rxBuf[RXBUFSIZE];
    bool _bufReady = false;
    long _smoothCorrelation=0;
    long _prevCorrelation=0;
    long _deltaCorrelation;
    long _minSmoothEdge=0;
    byte _subSampling=0;
    byte _backupData=0;
    int _cycleCtr=0;
    int _pllReg=320;
    unsigned long _prevTimeStamp=0;
};
