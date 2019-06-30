/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"
#include "SongPlayer.h"

static CommonConfig config1;
static AlarmConfig config2;

void setup()
{
  Serial.begin(115200);
  assignCommonConfig(&config1);
  assignActionsConfig(&config1, &config2);
  assignAlarmConfig(&config2);
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

