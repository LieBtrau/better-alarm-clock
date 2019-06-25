/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"
#include "SongPlayer.h"
#include "pins.h"
void printDetail(uint8_t type, int value);

CommonParameters compar;
AlarmParameters alarms;
CommonConfig config1 = {7, 20, 7};
AlarmConfig config2;
SongPlayer sPlayer(&Serial2, pinPlayBusy);

void setup()
{
  Serial.begin(115200);
  initMenu();
  if(!sPlayer.init())
  {
    while(true);
  }
  sPlayer.setSongPtr(&config2.song);
  sPlayer.setVolumePtr(&config2.volume);
  // delay(1000);
}

void loop()
{
  renderMenu();
  sPlayer.poll();
}

