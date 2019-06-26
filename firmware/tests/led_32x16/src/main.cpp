/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"
#include "SongPlayer.h"

CommonParameters compar;
AlarmParameters alarms;
CommonConfig config1 = {7, 20, 7};
AlarmConfig config2;

void setup()
{
  Serial.begin(115200);
  initMenu();
  if(!initPeripherals())
  {
    Serial.println("Can't init peripherals");
    while(true);
  }
}

void loop()
{
  renderMenu();
  pollActions();
}

