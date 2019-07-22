#include "actions.h"
#include "menuNav.h"

ActionMgr actionMgr;
extern ClockFace clockface;
extern MenuMgr menuMgr;
static SongPlayer sPlayer(&Serial2, pinPlayBusy);

void ActionMgr::assignCommonConfig(CommonConfig *pConfig)
{
  pCommon = pConfig;
}

void ActionMgr::assignAlarmConfig(AlarmConfig *config)
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

void ActionMgr::pollActions()
{
  static byte hours = 0;
  static byte mins = 0;
  static uint32_t ulTime = millis();
  sPlayer.poll();
  if (millis() > ulTime + 500)
  {
    ulTime = millis();
    clockface.setTime(hours, mins);
    if (++mins > 59)
    {
      mins = 0;
      if (++hours > 59)
        hours = 0;
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
  return true;
}

uint16_t ActionMgr::getTotalTrackCount()
{
  return sPlayer.getTotalTrackCount();
}

//---------------------------------------------------------------------------------------------------------------------------
//  Encapsulating functions, to be used as function pointers in events
//---------------------------------------------------------------------------------------------------------------------------
void playSong(byte i)
{
  sPlayer.play();
}

void setVolume(byte i)
{
  sPlayer.changeVolume();
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
  menuMgr.setBrightness(i);
}