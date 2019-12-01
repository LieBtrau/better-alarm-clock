#include "LedArray.h"
//#include "Fonts/Picopixel.h"
#include "Fonts/TomThumb.h"

LedArray::LedArray(byte pinMOSI, byte pinSCLK, byte pinCS) : matrix
{
}

void LedArray::init()
{
}

void LedArray::render()
{
  matrix.write();
}


void LedArray::setBrightness(byte i)
{
  if (i != brightness)
  {
    brightness = i;
    if(!i)
    {
      matrix.fillScreen(0);
      matrix.write();
    }
    else
    {
      matrix.setIntensity(i);
    }
  }
}
