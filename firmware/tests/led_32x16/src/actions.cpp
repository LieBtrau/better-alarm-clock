#include "actions.h"
#include "menuNav.h"

void playSong(byte i)
{
  Serial.print("Song : ");
  Serial.println(i, DEC);
}

void stopSong()
{
  Serial.println("stop song");
}

void showLightness(byte i)
{
  Serial.print("Light : ");
  Serial.println(i, DEC);
}

void setVolume(byte i)
{
  Serial.print("Volume : ");
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