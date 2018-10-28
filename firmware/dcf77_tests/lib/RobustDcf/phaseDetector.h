#pragma once
#include "Arduino.h"

typedef void(*event)(const byte value);

class PhaseDetector
{
  public:
    PhaseDetector(const byte inputPin, const byte monitorPin=0);
    void init();
    void process_one_sample();
    void attachSecondEventHandler(event secondTickEvent);

  private:
    static const int BIN_COUNT = 100;
    static const uint16_t SAMPLE_FREQ = 1000;
    static const uint16_t SAMPLES_PER_BIN = SAMPLE_FREQ / BIN_COUNT;
    static const uint16_t BINS_PER_10ms = BIN_COUNT / 100;
    static const uint16_t BINS_PER_100ms = 10 * BINS_PER_10ms;
    static const uint16_t BINS_PER_200ms = 20 * BINS_PER_10ms;

    typedef struct
    {
        uint16_t data[BIN_COUNT];
        uint32_t phaseCorrelation[BIN_COUNT];
        uint8_t activeBin;
        uint8_t pulseStart;
    } phase_bins;

    uint16_t wrap(const uint16_t value);
    void phaseCorrelator();
    void phase_binning(const bool input);
    void averager(const uint8_t sampled_data);
    void secondsSampler(const bool input);

    phase_bins _bins;
    byte _inputPin = 0;
    byte _monitorPin;
    event _secondsEvent = nullptr;
};