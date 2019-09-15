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

ActionMgr::ActionMgr() : alarms({{AlarmCalendar(0, 15), nullptr}, {AlarmCalendar(1, 15), nullptr}})
{
  alarms[0].calendar.setAlarmCallBack(alarmAction);
  alarms[1].calendar.setAlarmCallBack(alarmAction);
  _clockRefreshTimer.start(500);
  _brightnessRampTimer.start(2000);
  _ambientLightSenseTimer.start(1000);
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
  updateDesiredDisplayBrightness();
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
  if (!apds.begin())
  {
    return false;
  }
  apds.enableColor();
  if (!sPlayer.init())
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
  if (movementDetected() || buttonPressed || _desiredDisplayBrightness > 0)
  {
    _displayOffTimer.start(60000);
    menuMgr.showParameterMenu(true);
  }
  if (_displayOffTimer.isRunning())
  {
    if (!menuMgr.isVisible())
    {
      menuMgr.showParameterMenu(true);
    }
    displayBrightnessControl(_desiredDisplayBrightness);
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
void ActionMgr::displayBrightnessControl(byte desiredBrightness)
{
  if (_displayBrightness == 0xFF)
  {
    //not properly initialized yet
    _displayBrightness = desiredBrightness;
  }
  if (_brightnessRampTimer.justFinished())
  {
    _brightnessRampTimer.repeat();
    if (_displayBrightness < desiredBrightness)
    {
      _displayBrightness++;
    }
    if (_displayBrightness > desiredBrightness)
    {
      _displayBrightness--;
    }
  }
  menuMgr.setBrightness(_displayBrightness);
}

/**
 * Returns display brightness according to ambient light level
 */
void ActionMgr::updateDesiredDisplayBrightness()
{
  if (_ambientLightSenseTimer.justFinished())
  {
    _ambientLightSenseTimer.repeat();
    //wait for color data to be ready
    while (!apds.colorDataReady())
    {
      delay(5);
    }
    uint16_t r, g, b, c;
    apds.getColorData(&r, &g, &b, &c);
    const int lightLevels[] = {1, 2, 3, 4, 7, 12, 19, 31, 50, 82, 134, 219, 358, 584, 953, 1556};
    int smallestDifference = INT32_MAX;
    int indexSmallest = 0;
    for (int i = 0; i < 16; i++)
    {
      int absdiff = abs(c - lightLevels[i]);
      if (absdiff < smallestDifference)
      {
        smallestDifference = absdiff;
        indexSmallest = i;
      }
    }
    _desiredDisplayBrightness = indexSmallest;
  }
}

bool ActionMgr::movementDetected()
{
  return false;
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
