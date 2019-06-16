/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"

CommonParameters compar;
AlarmParameters alarms;
CommonConfig config1 = {50, 50, 50};
AlarmConfig config2;

void setup()
{
  Serial.begin(115200);
  initMenu();
  // delay(1000);
}

void loop()
{
  renderMenu();
}