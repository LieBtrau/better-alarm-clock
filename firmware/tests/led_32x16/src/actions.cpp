#include "actions.h"
#include "menuNav.h"
#include "SongPlayer.h"
#include "Adafruit_APDS9960.h"
#include "pins.h"
#include "parameters.h"

Adafruit_APDS9960 apds;
SongPlayer sPlayer(&Serial2, pinPlayBusy);
extern AlarmConfig config2;

bool isDark()
{
  //wait for color data to be ready
  while (!apds.colorDataReady())
  {
    delay(5);
  }
  uint16_t r, g, b, c;
  apds.getColorData(&r, &g, &b, &c);
  return c < (1 << config1.dayNight);
}

void pollActions()
{
  sPlayer.poll();
  // static uint32_t ulTime = millis();
  // if (millis() > ulTime + 500)
  // {
  //   ulTime = millis();
  // }
}

bool initPeripherals()
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
  sPlayer.setSongPtr(&config2.song);
  sPlayer.setVolumePtr(&config2.volume);
  return true;
}

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
  matrix7.setBrightness(i); // 0 -> 15
  matrix.setIntensity(i);   // 0 -> 15
}