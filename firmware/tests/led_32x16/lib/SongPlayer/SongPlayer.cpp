#include "SongPlayer.h"
#define DEBUG

static DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial2);

SongPlayer::SongPlayer(HardwareSerial *serial, uint32_t busyPin) : _busyPin(busyPin)
{
}

bool SongPlayer::init()
{
    pinMode(_busyPin, INPUT);
    mp3.begin();
    uint16_t count = mp3.getTotalTrackCount();
    return count != 0;
}

void SongPlayer::setVolumePtr(byte *volume)
{
    this->volume = volume;
}

void SongPlayer::setSongPtr(byte *song)
{
    this->song = song;
}

void SongPlayer::changeVolume()
{
    if (volume == nullptr || song == nullptr)
    {
        return;
    }
    mp3.setVolume(*volume);
    if (!isPlaying())
    {
        mp3.loopGlobalTrack(*song);
    }
}

void SongPlayer::play()
{
    if (volume == nullptr || song == nullptr)
    {
        return;
    }
    mp3.setVolume(*volume);
    mp3.loopGlobalTrack(*song);
}

void SongPlayer::stop()
{
    mp3.stop();
}

bool SongPlayer::isPlaying()
{
    return digitalRead(_busyPin) == LOW ? true : false;
}

void SongPlayer::poll()
{
    mp3.loop();
}
