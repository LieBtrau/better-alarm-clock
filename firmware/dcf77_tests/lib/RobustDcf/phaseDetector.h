/* This class tries to find the start of each second in data stream of 0's & 1's coming from the DCF-module.
 * Once the phase is found, it also returns the data from the stream : long/short pulse or no pulse at all (minute sync mark)
 */
#pragma once
#include "Arduino.h"
#include "bin.h"

typedef void (*event)(const bool isSync, const bool isLongPulse);

class PhaseDetector
{
public:
  PhaseDetector(const byte inputPin, const byte monitorPin = 0);
  void init(event secondTickEvent);
  void process_one_sample();

private:
  static const int BIN_COUNT = 100;
  static const uint16_t SAMPLE_FREQ = 1000;
  static const uint16_t SAMPLES_PER_BIN = SAMPLE_FREQ / BIN_COUNT;
  static const uint16_t BINS_PER_10ms = BIN_COUNT / 100;
  static const uint16_t BINS_PER_100ms = 10 * BINS_PER_10ms;
  static const uint16_t BINS_PER_200ms = 20 * BINS_PER_10ms;
  static const uint32 LOCK_THRESHOLD = 75;

  uint8_t wrap(const uint8_t value);
  bool phaseCorrelator();
  void phase_binning(const bool input);
  void averager(const uint8_t sampled_data);
  void secondsSampler(const bool input);

  byte _inputPin = 0;
  byte _monitorPin;
  event _secondsEvent = nullptr;
  Bin _bin; //100bins, each holding for 10ms of data
  uint32_t _phaseCorrelation[BIN_COUNT];
  uint8_t _activeBin = 0;
  uint8_t _pulseStartBin = UINT8_MAX;
};