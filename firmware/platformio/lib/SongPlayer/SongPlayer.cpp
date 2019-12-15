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
    mp3.setRepeatPlay(true);
    mp3.reset();
    totalTracks = mp3.getTotalTrackCount() >> 1; //for some reason 24 is returned while there are only 12 tracks on the sd-card.);;
    return totalTracks != 0;
}

uint16_t SongPlayer::getTotalTrackCount()
{
    return totalTracks;
}

void SongPlayer::playSong(byte song, byte volume)
{
    if (volume > 30)
    {
        return;
    }
    myvolume = volume;
    if (song < 1 || song > totalTracks)
    {
        return;
    }
    if (song != currentSong)
    {
        songChanged = true;
    }
    currentSong = song;
    _singleShotTimer.start(200);
}

void SongPlayer::stop()
{
    currentSong = 0;
    mp3.stop();
}

bool SongPlayer::isPlaying()
{
    return digitalRead(_busyPin) == LOW ? true : false;
}

void SongPlayer::poll()
{
    mp3.loop();
    if (_singleShotTimer.justFinished())
    {
        mp3.setVolume(myvolume);
        if (songChanged)
        {
            mp3.loopGlobalTrack(currentSong);
            songChanged = false;
        }
    }
}
