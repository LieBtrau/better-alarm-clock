#include "phaseDetector.h"

static PhaseDetector* psd;
void getSample();

PhaseDetector::PhaseDetector(const byte inputPin, const byte monitorPin):
    _inputPin(inputPin),
    _monitorPin(monitorPin)
{
    memset(_bins.data, 0, BIN_COUNT * sizeof(uint16_t));
    memset(_bins.phaseCorrelation, 0, BIN_COUNT * sizeof(uint32_t));
    _bins.activeBin = 0;
    _bins.pulseStart = 255;
    psd = this;
}

void PhaseDetector::init()
{
    pinMode(_inputPin, INPUT);
    digitalWrite(_monitorPin, HIGH);
    pinMode(_monitorPin, OUTPUT);
    systick_attach_callback(getSample);
}

void PhaseDetector::attachSecondEventHandler(event secondTickEvent)
{
    _secondsEvent = secondTickEvent;
}

void PhaseDetector::secondsSampler(const bool input)
{
    static byte state=0;
    static byte pulseData=0;
    static byte decodedData=0;
    static byte currentSecondPulseStart=0;
    // Serial1.print(_bins.pulseStart);
    // Serial1.print(",");
    // Serial1.println(_bins.activeBin);
    //Serial1.println(_bins.pulseStart);
    switch(state)
    {
        case 0:
            //Start sampling <10ms before pulse start or <100ms after pulse start
            if(wrap(BIN_COUNT + _bins.pulseStart - _bins.activeBin) <= BINS_PER_10ms || wrap((BIN_COUNT + _bins.activeBin - _bins.pulseStart)) <= BINS_PER_100ms)
            {
                state=1;
                pulseData = input;
                currentSecondPulseStart = _bins.pulseStart;
            }
            else
            {
            }
            break;
        case 1:
            //Check what the most occurring inputpin value was from 10ms before the start of the pulse up to 100ms later.
            pulseData+=input;
            if(wrap(BIN_COUNT + currentSecondPulseStart + BINS_PER_100ms) == _bins.activeBin)
            {
                state=2;
                decodedData = (pulseData > (BINS_PER_100ms>>1) ? 1 : 0);
                pulseData=0;
            }
            break;
        case 2:
            //Check what the most occurring inputpin value was from 100ms after the start of the pulse up to 110ms later.
            pulseData+=input;
            if(wrap(BIN_COUNT + currentSecondPulseStart + BINS_PER_200ms + BINS_PER_10ms) == _bins.activeBin)
            {
                state=0;
                decodedData |= (pulseData > (BINS_PER_100ms>>1) ? 2 : 0);
                /* Decoded data:
                0 : no pulse            -> minute marker
                1 : short regular pulse -> 0-bit
                3 : long regular pulse  -> 1-bit
                2 : short pulse, but 100ms shifted -> invalid
                */
               if(_secondsEvent)
               {
                   _secondsEvent(decodedData);
               }
            }
            break;
    }
}

// faster modulo function which avoids division
// returns value % bin_count
uint16_t PhaseDetector::wrap(const uint16_t value)
{
    uint16_t result = value;
    // while (result >= BIN_COUNT)
    // {
    //     result -= BIN_COUNT;
    // }
    // return result;
    return value % BIN_COUNT;
}

// The correlation is used to find the window of maximum signal strength.
void PhaseDetector::phaseCorrelator()
{
    uint8_t current_bin = _bins.activeBin;
    _bins.phaseCorrelation[current_bin] = 0;
    for (uint16_t bin = 0; bin < BINS_PER_100ms; ++bin)
    {
        _bins.phaseCorrelation[current_bin] += ((uint32_t)_bins.data[wrap(current_bin + bin)]);
    }
    _bins.phaseCorrelation[current_bin] <<= 1;
    for (uint16_t bin = BINS_PER_100ms; bin < BINS_PER_200ms; ++bin)
    {
        _bins.phaseCorrelation[current_bin] += (uint32_t)_bins.data[wrap(current_bin + bin)];
    }

    uint32_t maxCorrelation = 0;
    byte highestCorrelationBin=0;
    for (uint16_t bin = 0; bin < BIN_COUNT; ++bin) {
        //Find bin where correlation is maximum
        if (_bins.phaseCorrelation[bin] > maxCorrelation) {
            maxCorrelation = _bins.phaseCorrelation[bin];
            highestCorrelationBin = bin;
        }
    }

    if(highestCorrelationBin<_bins.pulseStart)
    {
        _bins.pulseStart--;
    }
    if(highestCorrelationBin>_bins.pulseStart)
    {
        _bins.pulseStart++;
    }

    
}

//Add the averaged sample to the correct bin.
//This function gets called every 10ms
void PhaseDetector::phase_binning(const bool input)
{
    // how many seconds may be cummulated
    // this controls how slow the filter may be to follow a phase drift
    // N times the clock precision shall be smaller 1
    // clock 30 ppm => N < 300
    const uint16_t N = 300;

    //Select the next bin to add samples to.
    // if(_bins.activeBin < BIN_COUNT - 1)
    // {
    //     _bins.activeBin++;
    // }else{
    //     _bins.activeBin=0;
    // }

    _bins.activeBin = ((_bins.activeBin < BIN_COUNT - 1) ? _bins.activeBin + 1 : 0);
 
    if (input)
    {
        if (_bins.data[_bins.activeBin] < N)
        {
            ++_bins.data[_bins.activeBin];
        }
    }
    else
    {
        if (_bins.data[_bins.activeBin] > 0)
        {
            --_bins.data[_bins.activeBin];
        }
    }
    // Serial1.println(bins.data[bins.tick]);
}

//Find the symbol that occurs most (0 or 1) every 10 samples.
//This function gets called every ms.
void PhaseDetector::averager(const uint8_t sampled_data)
{
    static uint8_t sampleCtr = 0;
    static uint8_t average = 0;

    // detector stage 0: average 10 samples (per bin)
    average += sampled_data;

    if (++sampleCtr >= SAMPLES_PER_BIN)
    {
        // once all samples for the current bin are captured the bin gets updated
        // that is each 10ms control is passed to stage 1
        const bool input = (average > (SAMPLES_PER_BIN >> 1));
        phase_binning(input);
        phaseCorrelator();
        secondsSampler(input);
        average = 0;
        sampleCtr = 0;
    }
}

void PhaseDetector::process_one_sample()
{
    const uint8_t sampled_data = digitalRead(_inputPin);

    digitalWrite(_monitorPin, sampled_data);

    averager(sampled_data ? 0 : 1);
}

void getSample()
{
    psd->process_one_sample();
}