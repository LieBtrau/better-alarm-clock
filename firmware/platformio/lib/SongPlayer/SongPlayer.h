#pragma once

#include "DFMiniMp3.h"
#include "Arduino.h"

class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);
  }

  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);
  }

  static void OnUsbOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("USB Disk online ");
    Serial.println(code);
  }

  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code);
  }

  static void OnUsbInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("USB Disk inserted ");
    Serial.println(code);
  }

  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);
  }

  static void OnUsbRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("USB Disk removed ");
    Serial.println(code);
  }
};

/* Volume and song choice are linked parameters.  When adjusting the volume, we want the correct song to be played and when selecting a song, we
 * want it to be played at the correct volume.  Hence we need an object that is aware of both parameters.
 */
class SongPlayer
{
public:
  SongPlayer(HardwareSerial *serial, uint32_t busyPin);
  bool init();
  void setVolumePtr(byte *volume);
  void setSongPtr(byte *song);
  void play();
  void stop();
  void poll();
  uint16_t getTotalTrackCount();

private:
  bool isPlaying();
  void printDetail(uint8_t type, int value);
  uint32_t _busyPin;
  byte *song = nullptr;
  byte *volume = nullptr;
  unsigned long delayStart = 0; // the time the delay started
  bool delayRunning = false;    // true if still waiting for delay to finish
};