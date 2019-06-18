#include "actions.h"

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
  Serial.println("Display brightness");
}