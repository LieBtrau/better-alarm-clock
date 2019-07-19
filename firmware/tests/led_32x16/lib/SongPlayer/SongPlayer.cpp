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

    mp3.reset();
    return getTotalTrackCount() != 0;
}

uint16_t SongPlayer::getTotalTrackCount()
{
    return mp3.getTotalTrackCount()>>1; //for some reason 24 is returned while there are only 12 tracks on the sd-card.);;
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
