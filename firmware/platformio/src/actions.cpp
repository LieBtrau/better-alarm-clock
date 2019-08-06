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
void alarmAction(byte id, bool start);

ActionMgr::ActionMgr() : alarms({{AlarmCalendar(0, 15), nullptr}, {AlarmCalendar(1, 15), nullptr}})
{
  alarms[0].calendar.setAlarmCallBack(alarmAction);
  alarms[1].calendar.setAlarmCallBack(alarmAction);
}

//  \brief update alarm calendar
//  \param[in]  alarmIndex   starts at index 0
void ActionMgr::updateAlarmSettings(AlarmConfig *config, byte alarmIndex)
{
  alarms[alarmIndex].config = config;
  alarms[alarmIndex].calendar.setConfig(&config->time);
}

void ActionMgr::assignCommonConfig(CommonConfig *pConfig)
{
  pCommon = pConfig;
}

void ActionMgr::assignAlarmConfig(byte id, AlarmConfig *config)
{
  sPlayer.setSongPtr(&config->song);
  sPlayer.setVolumePtr(&config->volume);
}

bool ActionMgr::isDark()
{
  //wait for color data to be ready
  while (!apds.colorDataReady())
  {
    delay(5);
  }
  uint16_t r, g, b, c;
  apds.getColorData(&r, &g, &b, &c);
  return c < (1 << pCommon->dayNight);
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

void ActionMgr::pollActions()
{
  static uint32_t ulTime = millis();
  static bool alarmsPrinted = false;

  sPlayer.poll();
  if (millis() > ulTime + 500)
  {
    if (dcfclock.update())
    {
      //MCU time is synced to DCF
      ulTime = millis();
      time_t utc = now();
      TimeChangeRule *tcr;
      time_t local = CE.toLocal(utc, &tcr);
      //printDateTime(local, tcr->abbrev);                //debug
      clockface.setTime(hour(local), minute(local));
      Chronos::DateTime localTime(local);
      //localTime.printTo(Serial);                        //debug
      alarms[0].calendar.loop(&localTime);
      alarms[1].calendar.loop(&localTime);
      if (!alarmsPrinted)
      {
        alarmsPrinted = true;
        alarms[0].calendar.listEvents(localTime);
        alarms[1].calendar.listEvents(localTime);
      }
    }
    else
    {
      //should show some animation here, indicating clock is syncing time
      clockface.setTime(0, 0);
    }
  }
}

bool ActionMgr::initPeripherals()
{
  if (!apds.begin())
  {
    return false;
  }
  apds.enableColor(true);
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

byte* ActionMgr::getSong(byte alarmIndex)
{
  return &alarms[alarmIndex].config->song;
}
byte* ActionMgr::getVolume(byte alarmIndex)
{
  return &alarms[alarmIndex].config->volume;
}

//---------------------------------------------------------------------------------------------------------------------------
//  Encapsulating functions, to be used as function pointers in events
//---------------------------------------------------------------------------------------------------------------------------

void alarmAction(byte id, bool start)
{
  Serial.println(start ? "alarm start\n" : "alarm end\n");
  if (start)
  {
    sPlayer.setSongPtr(actionMgr.getSong(id));
    sPlayer.setVolumePtr(actionMgr.getVolume(id));
    sPlayer.play();
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