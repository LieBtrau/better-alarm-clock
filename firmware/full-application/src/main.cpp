/**
 * Weekday buttons should be pressed for longer than 2 seconds before they change state.  This will prevent accidental pushes from changing the alarm settings.
 * 
 */

#include <Arduino.h>
#include "alarmcalendar.h"
#include "DisplayOnOff.h"
#include "TimeSync.h"
#include "Weekdays.h"
#include "VisualElements.h"
#include "WeekdayButtons.h"
#include "ParameterSetup.h"

typedef enum
{
  STORE_SETTINGS,
  CLOCK_ALARM,      //< Normal state : Clock and alarm are visible
  SET_SUNRISE,
  SET_VOLUME,
  SET_SONGCHOICE,
  SET_ALARM_MINUTES,
  SET_ALARM_HOURS
} OPERATING_MODE;

CFG config;
AsyncDelay ledTimer(500, AsyncDelay::MILLIS);
AsyncDelay parameterSetupTimeout(SETUP_TIMEOUT, AsyncDelay::MILLIS);
OPERATING_MODE state = CLOCK_ALARM;
const int SECONDS_IN_30_MINS = 1800;
AlarmCalendar ac1(2);

void updateAlarmCalendar()
{
  ac1.setAlarmTime(config.alarmHours, config.alarmMinutes);
  ac1.setWeekdays((AlarmCalendar::WEEKDAYS)config.wd);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(200);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
  initClockSource();
  initPeripherals(&config);
  updateAlarmCalendar();

  if (!initDisplayOnOffControl())
  {
    Serial.println("Failed to initialize.");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
  setWeekdayPointer(&config.wd);
  setBrightness(8);
  Serial.println("ready");
}

bool isAlarmBusy()
{
  time_t localTime;
  return getLocalTimeSeconds(localTime) && ac1.isUnacknowledgedAlarmOnGoing(localTime);
}

void clockloop(DISPLAY_STATE ds, bool buttonPressed)
{
  byte hours, minutes;
  time_t localTime, totalSecondsToNextEvent;
  bool alarmAcked = false;

  if (getLocalTimeSeconds(localTime))
  {
    if (buttonPressed)
    {
      ac1.acknowledgeAlarm();
      alarmAcked = true;
    }
    //Redraw LED array
    if ((isNewMinuteStarted(localTime, hours, minutes) && ds == DISPLAY_ON) || ds == DISPLAY_TURNED_ON || alarmAcked)
    {
      showClockTime(hours, minutes, isStillSynced(), ac1.isUnacknowledgedAlarmOnGoing(localTime), true);
      Chronos::DateTime::now().printTo(Serial);
      Serial.println();
    }
    //Redraw alarm LCD
    if ((ds == DISPLAY_TURNED_ON) || (ds == DISPLAY_ON))
    {
      showAlarmDisplay(ac1.isAlarmIn24Hours(localTime) ? AL_BOTH_ON : AL_BOTH_OFF);
    }
    if (ac1.isAlarmIn24Hours(localTime))
    {
      showAlarmTime(config.alarmHours, config.alarmMinutes);
    }
    //Handle music
    if (ac1.isUnacknowledgedAlarmOnGoing(localTime))
    {
      playMusic(config.songchoice, config.volume);
    }
    else
    {
      stopMusic();
    }
    //Handle sun rise emulation
    if (ac1.isUnacknowledgedAlarmOnGoing(localTime))
    {
      showSunriseSetting(config.sunRiseSetting);
    }
    else
    {
      if (ac1.getSecondsToStartOfNextEvent(localTime, totalSecondsToNextEvent) && totalSecondsToNextEvent < SECONDS_IN_30_MINS)
      {
        float brightness = config.sunRiseSetting * (SECONDS_IN_30_MINS - totalSecondsToNextEvent) / SECONDS_IN_30_MINS;
        showSunriseSetting(brightness);
      }
      else
      {
        stopSunrise();
      }
    }
  }
  else
  {
    //Time is not valid
    //Redraw LED array
    showClockTime(0, 0, false, false, false);
    //Redraw alarm LCD
    showAlarmDisplay(AL_BOTH_OFF);
    //Handle music
    stopMusic();
    //Handle sun rise emulation
    stopSunrise();
  }
}

void loop()
{
  RotaryEncoder_Tactiles::DIRECTION dir;
  KEY_CODE key;
  bool rotEncMoved = isRotaryEncoderChanged(dir);
  bool buttonPressed = isButtonChanged(key);
  DISPLAY_STATE ds = getDisplayState(buttonPressed, isAlarmBusy());

  //process buttons
  if (buttonPressed)
  {
    if (isAlarmBusy())
    {
      clockloop(ds, true);
    }
    else
    {
      switch (key)
      {
      case KEY_SUNRISE:
        state = state == SET_SUNRISE ? STORE_SETTINGS : SET_SUNRISE;
        break;
      case KEY_VOLUME:
        state = state == SET_VOLUME ? STORE_SETTINGS : SET_VOLUME;
        break;
      case KEY_SONGCHOICE:
        state = state == SET_SONGCHOICE ? STORE_SETTINGS : SET_SONGCHOICE;
        break;
      case KEY_ALARM:
        switch (state)
        {
        case SET_ALARM_HOURS:
          state = SET_ALARM_MINUTES;
          break;
        case SET_ALARM_MINUTES:
          state = STORE_SETTINGS;
          break;
        default:
          state = SET_ALARM_HOURS;
          break;
        }
        break;
      default:
        if (processWeekdayButtonEvent(key))
        {
          state = STORE_SETTINGS;
        }
        break;
      }
      parameterSetupTimeout.restart();
    }
  }
  if (rotEncMoved)
  {
    parameterSetupTimeout.restart();
  }
  if (parameterSetupTimeout.isExpired() && state != CLOCK_ALARM)
  {
    state = STORE_SETTINGS;
  }

  //Act according to state
  switch (state)
  {
  case STORE_SETTINGS:
    disableParameterSetting(&config);
    updateAlarmCalendar();
    clockloop(ds, true);
    state = CLOCK_ALARM;
    break;
  case CLOCK_ALARM:
    clockloop(ds, false);
    break;
  case SET_SUNRISE:
    if (rotEncMoved)
    {
      updateParameter(&config.sunRiseSetting, 0.0f, 100.0f, 10.0f, dir);
    }
    showSunriseSetting(config.sunRiseSetting);
    showSunlightSetting(rescaleParameter(&config.sunRiseSetting, 0.0f, 100.0f, 10.0f));
    break;
  case SET_VOLUME:
    if (rotEncMoved)
    {
      updateParameter(&config.volume, 0, MAX_VOLUME, 1, dir);
    }
    playMusic(config.songchoice, config.volume);
    showSongVolume(rescaleParameter(&config.volume, 0, MAX_VOLUME, 10));
    break;
  case SET_SONGCHOICE:
    if (rotEncMoved)
    {
      updateParameter(&config.songchoice, 1, getTrackCount(), 1, dir);
    }
    playMusic(config.songchoice, config.volume);
    showSongChoice(rescaleParameter(&config.songchoice, 1, getTrackCount(), 10));
    break;
  case SET_ALARM_HOURS:
    if (rotEncMoved)
    {
      updateParameter(&config.alarmHours, 0, 23, 1, dir);
    }
    showAlarmTime(config.alarmHours, config.alarmMinutes);
    showAlarmDisplay(AL_HOURS_ONLY, true);
    break;
  case SET_ALARM_MINUTES:
    if (rotEncMoved)
    {
      updateParameter(&config.alarmMinutes, 0, 55, 5, dir);
    }
    showAlarmTime(config.alarmHours, config.alarmMinutes);
    showAlarmDisplay(AL_MINUTES_ONLY, true);
    break;
  default:
    break;
  }

  if (ledTimer.isExpired())
  {
    ledTimer.restart();
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
  }

  //Brightness control for all elements
  switch (ds)
  {
  case DISPLAY_TURNED_ON:
    clockloop(ds, false);
    break;
  case DISPLAY_ON:
    setBrightness(getDisplayBrightness(isAlarmBusy()));
    showWeekDay(config.wd);
    break;
  case DISPLAY_TURNED_OFF:
    setVisible(false);
    break;
  case DISPLAY_OFF:
  default:
    break;
  }

  redraw();
}