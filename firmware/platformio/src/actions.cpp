#include "actions.h"
#include "dcfUtcClock.h"
#include "menuNav.h"

ActionMgr actionMgr;
extern ClockFace clockface;
extern MenuMgr menuMgr;
static SongPlayer sPlayer(&Serial2, pinPlayBusy);
DcfUtcClock dcfclock(pin_DCF, true);
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
Timezone CE(CEST, CET);
void alarmAction(bool start, byte id = 0);

ActionMgr::ActionMgr() : alarms({{AlarmCalendar(0, 15), nullptr}, {AlarmCalendar(1, 15), nullptr}}), i2c(sdaPin, sclPin)
{
  alarms[0].calendar.setAlarmCallBack(alarmAction);
  alarms[1].calendar.setAlarmCallBack(alarmAction);
  _clockRefreshTimer.start(500);
  _ambientLightSenseTimer.start(500);
  pinMode(pin_PIR, INPUT);
}

//  \brief update alarm calendar with settings from the eeprom
//  \param[in]  alarmIndex   starts at index 0
void ActionMgr::updateAlarmSettings(AlarmConfig *config, ALARMNRS alarmIndex)
{
  alarms[alarmIndex].config = config;
  alarms[alarmIndex].calendar.setConfig(&config->time);
}

void ActionMgr::assignCommonConfig(CommonConfig *pConfig)
{
  pCommon = pConfig;
}

void ActionMgr::assignAlarmConfig(ALARMNRS id, AlarmConfig *config)
{
  sPlayer.setSongPtr(&config->song);
  sPlayer.setVolumePtr(&config->volume);
}

void ActionMgr::pollActions(bool buttonPressed)
{
  sPlayer.poll();
  displayOnOffControl(buttonPressed);
  if (dcfclock.update())
  {
    //MCU time is synced to DCF
    time_t utc = now();
    TimeChangeRule *tcr;
    time_t local = CE.toLocal(utc, &tcr);
    Chronos::DateTime localTime(local);
    if ((alarms[0].calendar.loop(&localTime) || alarms[1].calendar.loop(&localTime)) && buttonPressed)
    {
      alarmAction(false);
    }
    if (_clockRefreshTimer.justFinished())
    {
      _clockRefreshTimer.repeat();
      clockface.setTime(hour(local), minute(local), dcfclock.isLastSyncSuccessful());
      ALARMNRS nr;
      menuMgr.showFirstAlarm(getFirstAlarmIn(Chronos::Span::Days(1), nr) ? alarms[nr].config : nullptr);
    }
  }
  else
  {
    menuMgr.showSyncAnimation();
  }
}

bool ActionMgr::initPeripherals()
{
  i2c.setDelay_us(5);
  i2c.begin();
  i2c.setTxBuffer(malloc(50), 50);
  i2c.setRxBuffer(malloc(10), 10);
  if (!sPlayer.init() || !als.begin(&i2c))
  {
    while (true)
      ;
  }
  dcfclock.init();
  return true;
}

uint16_t ActionMgr::getTotalTrackCount()
{
  return sPlayer.getTotalTrackCount();
}

byte *ActionMgr::getSong(ALARMNRS alarmIndex)
{
  return &alarms[alarmIndex].config->song;
}
byte *ActionMgr::getVolume(ALARMNRS alarmIndex)
{
  return &alarms[alarmIndex].config->volume;
}

/**
 * \remark : Running this command when a calendar event is ongoing, will return that calendar event.
 */
bool ActionMgr::getFirstAlarmIn(Chronos::Span::Delta delta, ALARMNRS &alarmIndex)
{
  alarmIndex = MAX_ALARMS;
  time_t utc = now();
  TimeChangeRule *tcr;
  time_t local = CE.toLocal(utc, &tcr);
  Chronos::DateTime localTime(local);
  Chronos::DateTime earliestAlarm(local);
  earliestAlarm += delta;
  for (int i = 0; i < MAX_ALARMS; i++)
  {
    Chronos::DateTime alarmTime;
    if (alarms[i].calendar.getStartOfNextEvent(&localTime, &alarmTime))
    {
      if (alarmTime < earliestAlarm)
      {
        alarmIndex = (ALARMNRS)i;
        earliestAlarm = alarmTime;
      }
    }
  }
  return alarmIndex != MAX_ALARMS;
}

/**
 * \brief If display is on minimum brightness for a minute, then turn the display off.
 */
void ActionMgr::displayOnOffControl(bool buttonPressed)
{
  int brightness = 0;
  if (displayBrightnessControl(brightness))
  {
    menuMgr.setBrightness(brightness);
    if (brightness > 0)
    {
      _displayOffTimer.start(DISPLAY_DARK_TIMEOUT);
      menuMgr.showParameterMenu(true);
    }
  }
  if (movementDetected() || buttonPressed)
  {
    _displayOffTimer.start(DISPLAY_DARK_TIMEOUT);
    menuMgr.showParameterMenu(true);
  }
  if (_displayOffTimer.isRunning())
  {
    if (!menuMgr.isVisible())
    {
      menuMgr.showParameterMenu(true);
    }
  }
  if (_displayOffTimer.justFinished())
  {
    menuMgr.showParameterMenu(false);
  }
}

/**
 * \brief Set brightness level according to ambient light level
 * \note  Setting occurs fully automatically.  Should a user want to add adjustments, then this function could be changed so that it adds an offset to the selected element in the table.
 *        e.g.  If autobrightness selects index 5, but the user wants a little bit brighter, then the resulting brightness = index + useroffset
 *        Default value for useroffset = 0, but it could be -15 up to +15.
 */
bool ActionMgr::displayBrightnessControl(int &brightness)
{
  if (_ambientLightSenseTimer.justFinished())
  {
    _ambientLightSenseTimer.repeat();
    float luxlevel = 0;//als.readAlsValue(); -> interferes with DCF
    if (luxlevel == 0)
    {
      return false;
    }
    const float lightLevels[] = {0.07, 0.13, 0.25, 0.49, 0.97, 1.89, 3.7, 7.24, 14.17, 27.73, 54.26, 106.16, 207.71, 406.4, 795.16, 1555.82};
    float smallestDifference = INT32_MAX;
    for (int i = 0; i < 16; i++)
    {
      int absdiff = abs(luxlevel - lightLevels[i]);
      if (absdiff < smallestDifference)
      {
        smallestDifference = absdiff;
        brightness = i;
      }
    }
    return true;
  }
  return false;
}

bool ActionMgr::movementDetected()
{
  return digitalRead(pin_PIR);
}

//---------------------------------------------------------------------------------------------------------------------------
//  Encapsulating functions, to be used as function pointers in events
//---------------------------------------------------------------------------------------------------------------------------

void alarmAction(bool start, byte id)
{
  Serial.println(start ? "alarm start\n" : "alarm end\n");
  if (start)
  {
    sPlayer.setSongPtr(actionMgr.getSong((ALARMNRS)id));
    sPlayer.setVolumePtr(actionMgr.getVolume((ALARMNRS)id));
    sPlayer.play();
  }
  else
  {
    stopSong();
  }
}

void playSong(byte i)
{
  sPlayer.play();
}

void stopSong()
{
  sPlayer.stop();
}

void showLightness(byte i)
{
  Serial.print("Light : ");
  Serial.println(i, DEC);
}

void stopLightness()
{
  Serial.println("stop lightness");
}

void setLedArrayBrightness(byte i)
{
  Serial.print("Display : ");
  Serial.println(i, DEC);
  menuMgr.setSessionBrightness(i);
}

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
  char buf[32];
  char m[4]; // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  Serial.println(buf);
}
