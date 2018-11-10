#include "phaseDetector.h"

static PhaseDetector *psd;
void getSample();

PhaseDetector::PhaseDetector(const byte inputPin, const byte monitorPin) : _inputPin(inputPin),
                                                                           _monitorPin(monitorPin)
{
    memset(_bins, 0, sizeof(_bins));
    memset(_phaseCorrelation, 0, sizeof(_phaseCorrelation));
    _activeBin = 0;
    _pulseStartBin = 255;
    psd = this;
}

void PhaseDetector::init(event secondTickEvent)
{
    pinMode(_inputPin, INPUT);
    digitalWrite(_monitorPin, HIGH);
    pinMode(_monitorPin, OUTPUT);
    _secondsEvent = secondTickEvent;
    systick_attach_callback(getSample);
}

void PhaseDetector::secondsSampler(const bool input)
{
    static byte state = 0;
    static byte pulseCtr = 0;
    static bool syncMark = false;
    static byte currentSecondPulseStart = 0;
    switch (state)
    {
    case 0:
        //Start sampling <10ms before pulse start or <100ms after pulse start
        if (wrap(BIN_COUNT + _pulseStartBin - _activeBin) <= BINS_PER_10ms || wrap((BIN_COUNT + _activeBin - _pulseStartBin)) <= BINS_PER_100ms)
        {
            state = 1;
            pulseCtr = input;
            currentSecondPulseStart = _pulseStartBin;
        }
        break;
    case 1:
        //Check what the most occurring inputpin value was from 10ms before the start of the pulse up to 100ms later.
        //This where the sync mark is located in case it's present
        pulseCtr += input;
        if (wrap(BIN_COUNT + currentSecondPulseStart + BINS_PER_100ms) == _activeBin)
        {
            state = 2;
            syncMark = (pulseCtr > (BINS_PER_100ms >> 1) ? false : true);
            pulseCtr = 0;
        }
        break;
    case 2:
        //Check what the most occurring inputpin value was from 100ms after the start of the pulse up to 110ms later.
        //This is where the the difference between a short and a long pulse can be detected.
        pulseCtr += input;
        if (wrap(BIN_COUNT + currentSecondPulseStart + BINS_PER_200ms + BINS_PER_10ms) == _activeBin)
        {
            state = 0;
            if (_secondsEvent)
            {
                _secondsEvent(syncMark, pulseCtr > (BINS_PER_100ms >> 1) ? true : false);
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
    while (result >= BIN_COUNT)
    {
        result -= BIN_COUNT;
    }
    return result;
}

// The correlation is used to find the window of maximum signal strength.
void PhaseDetector::phaseCorrelator()
{
    //Reset bin
    _phaseCorrelation[_activeBin] = 0;

    //Correlate with the template
    for (uint16_t bin = 0; bin < BINS_PER_100ms; ++bin)
    {
        _phaseCorrelation[_activeBin] += ((uint32_t)_bins[wrap(_activeBin + bin)]);
    }
    _phaseCorrelation[_activeBin] <<= 1;
    for (uint16_t bin = BINS_PER_100ms; bin < BINS_PER_200ms; ++bin)
    {
        _phaseCorrelation[_activeBin] += (uint32_t)_bins[wrap(_activeBin + bin)];
    }

    //Find bin where correlation is maximum
    uint32_t maxCorrelation = 0;
    byte highestCorrelationBin = 0;
    for (uint16_t bin = 0; bin < BIN_COUNT; ++bin)
    {
        if (_phaseCorrelation[bin] > maxCorrelation)
        {
            maxCorrelation = _phaseCorrelation[bin];
            highestCorrelationBin = bin;
        }
    }

    //Move the bin where the pulse starts closer to the bin with currently the highest match
    if (highestCorrelationBin < _pulseStartBin)
    {
        _pulseStartBin--;
    }
    if (highestCorrelationBin > _pulseStartBin)
    {
        _pulseStartBin++;
    }
}

//Add the averaged sample to the correct bin.
//This function gets called every 10ms
void PhaseDetector::phase_binning(const bool input)
{
    // how many seconds may be accumulated
    // this controls how slow the filter may be to follow a phase drift
    // N times the clock precision shall be smaller 1
    // clock 30 ppm => N < 300
    const uint16_t N = 300;

    _activeBin = ((_activeBin < BIN_COUNT - 1) ? _activeBin + 1 : 0);

    if (input)
    {
        if (_bins[_activeBin] < N)
        {
            ++_bins[_activeBin];
        }
    }
    else
    {
        if (_bins[_activeBin] > 0)
        {
            --_bins[_activeBin];
        }
    }
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
    //sample inverter for HKW module
    averager(sampled_data ? 0 : 1);
}

void getSample()
{
    psd->process_one_sample();
}