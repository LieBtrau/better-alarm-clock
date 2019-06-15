#include "actions.h"

void playSong(byte i)
{
  Serial.print("Song : ");
  Serial.println(i, DEC);
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