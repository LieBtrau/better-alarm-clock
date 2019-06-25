#include "actions.h"
#include "menuNav.h"
#include "SongPlayer.h"

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