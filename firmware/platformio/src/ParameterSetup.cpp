#include "Tactiles.h"
#include "LedDriverDimming.h"
#include "pins.h"
#include "Mp3Notify.h"
#include <DFMiniMp3.h>
#include "inputs.h"
#include "VisualElements.h"

const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address

LedDriverDimming ldd(pin_en_sun, pin_pwmh, pin_pwml);
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial2);
SX1509 io1, io2;
int trackCount = 0;
bool isMusicPlaying = false;

void disableParameterSetting()
{
    ldd.enable(false);
    mp3.stop();
    isMusicPlaying = false;
}

void initPeripherals()
{
  if (!io1.begin(IO1_SX1509_ADDRESS))
  {
    while (1)
      ;
  }
  if (!io2.begin(IO2_SX1509_ADDRESS))
  {
    while (1)
      ;
  }

  // Use the internal 2MHz oscillator.
  // Set LED clock to 500kHz (2MHz / (2^(3-1)):
  io1.clock(INTERNAL_CLOCK_2MHZ, 3);
  io2.clock(INTERNAL_CLOCK_2MHZ, 3);

  if (!setupInputs(&io1, &io2))
  {
    Serial.println("Can't setup all input devices.");
    while (true)
      ;
  }
  if (!initVisualElements(&io1, &io2))
  {
    Serial.println("Failed to initialize.");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
    ldd.init();
    mp3.begin();
    trackCount = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
}

void showSunriseSetting(float sunRiseSetting)
{
    ldd.enable(true);
    ldd.setBrightness(sunRiseSetting);
}

int getTrackCount()
{
    return trackCount;
}

void playMusic(int track, int volume)
{
    static int lastTrack = 0;
    static int lastVolume = -1;
    if (!isMusicPlaying || track != lastTrack || volume != lastVolume)
    {
        mp3.setVolume(volume);
        mp3.loopGlobalTrack(track);
        lastTrack = track;
        lastVolume = volume;
        isMusicPlaying = true;
    }
}
