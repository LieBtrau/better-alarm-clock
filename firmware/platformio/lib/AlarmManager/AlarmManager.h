/* Class that holds one alarm
 */
#pragma once
#include "alarmcalendar.h"
#include "SongPlayer.h"

struct AlarmConfig
{
    byte lightness = 50;
    byte volume = 10;
    byte song = 3;
    ALARM_CONFIG time;
};

class AlarmManager
{
public:
    AlarmManager(SongPlayer *music, uint32_t pin_Light, AlarmConfig *config);
    void playSong(byte songnr);
    void setVolume(byte i);
    void stopSong();
    void showLightness(byte i);
    void stopLightness();
    bool loop(const Chronos::DateTime *timenow);
    void turnAlarmOff();
    bool getAlarmBefore(const Chronos::DateTime localTime, Chronos::Span::Days delta, Chronos::DateTime& alarmTime);
    AlarmConfig* getConfig();
private:
    enum
    {
        WAITING_FOR_PRELIGHTING,
        PRELIGHTING,
        KILL_ALARM,
        WAITING_FOR_ALARM_END,
        ALARM_TIME,
        SOUND_PLAYING
    } state;
    const Chronos::Span::Minutes PRELIGHTTIME = Chronos::Span::Minutes(30);
    void handleAlarmLight(byte lightness);
    SongPlayer *_music;
    uint32_t _pin_Light;
    AlarmConfig *_config;
    AlarmCalendar calendar;
};