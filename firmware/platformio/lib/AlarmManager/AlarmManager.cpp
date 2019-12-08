#include "AlarmManager.h"

AlarmManager::AlarmManager(SongPlayer *music, uint32_t pin_Light) : _music(music),
                                                                    _pin_Light(pin_Light),
                                                                    calendar(15)
{
    state = WAITING_FOR_PRELIGHTING;
}

AlarmConfig *AlarmManager::getConfig()
{
    return _config;
}

void AlarmManager::setConfig(AlarmConfig *config)
{
    _config = config;
    calendar.setConfig(&config->time);
}

/*
 * \return true when light or sound are on due to an alarm
 */
bool AlarmManager::loop(const Chronos::DateTime *timenow)
{
    _music->poll();
    if (!timenow->asEpoch())
    {
        return false;
    }
    Chronos::DateTime alarmTime;
    switch (state)
    {
    case WAITING_FOR_PRELIGHTING:
        if (calendar.getStartOfNextEvent(timenow, &alarmTime) && alarmTime < *timenow + PRELIGHTTIME)
        {
            state = PRELIGHTING;
            return true;
        }
        return false;
    case PRELIGHTING:
    {
        Chronos::Span::Absolute diff = *timenow + PRELIGHTTIME - alarmTime;
        showLightness(_config->lightness * diff.totalSeconds() / PRELIGHTTIME.totalSeconds());
        if (calendar.isAlarmOnGoing(timenow))
        {
            state = ALARM_TIME;
        }
        return true;
    }
    case KILL_ALARM:
    {
        showLightness(0);
        stopSong();
        state = WAITING_FOR_ALARM_END;
        return false;
    }
    case WAITING_FOR_ALARM_END:
    {
        if (!calendar.isAlarmOnGoing(timenow))
        {
            state = WAITING_FOR_PRELIGHTING;
        }
        return false;
    }
    case ALARM_TIME:
        _music->playSong(_config->song, _config->volume);
        state = SOUND_PLAYING;
        return true;
    case SOUND_PLAYING:
        if (!calendar.isAlarmOnGoing(timenow))
        {
            state = KILL_ALARM;
        }
        return true;
    default:
        break;
    }
    return false;
}

void AlarmManager::turnAlarmOff()
{
    state = KILL_ALARM;
}

void AlarmManager::playSong(byte songnr)
{
    _config->song = songnr;
    _music->playSong(_config->song, _config->volume);
}

void AlarmManager::stopSong()
{
    _music->stop();
}

void AlarmManager::setVolume(byte volume)
{
    _config->volume = volume;
    _music->playSong(_config->song, _config->volume);
}

void AlarmManager::showLightness(byte lightlevel)
{
    _config->lightness = lightlevel;
    handleAlarmLight(true);
}

void AlarmManager::stopLightness()
{
}

void AlarmManager::handleAlarmLight(byte lightness)
{
}

/**
 * \remark : Running this command when a calendar event is ongoing, will return that calendar event.
 */
bool AlarmManager::getAlarmBefore(const Chronos::DateTime fromDT, Chronos::Span::Days delta, Chronos::DateTime &returnedDT)
{
    return calendar.getStartOfNextEvent(&fromDT, &returnedDT) && returnedDT < fromDT + delta;
}
