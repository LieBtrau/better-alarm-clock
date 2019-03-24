#include "clockdisplay.h"
#include <TM1637Display.h>

class tm1637clock : public clockdisplay
{
  public:
    tm1637clock(int pin1, int pin2);
    void init();
    void showTime(int hour, int minutes);
    void clearDisplay();
    void setBrightness(int brightness);
    void setColon(bool colonOn);
    int getMinutes();

  private:
    TM1637Display display;
    uint8_t data[4] = {SEG_G, SEG_G | SEG_DP, SEG_G, SEG_G};
    int _minutes=-1;
};