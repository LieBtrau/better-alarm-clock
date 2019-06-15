/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"

CommonParameters compar;
AlarmParameters alarms[2];

bool matrixFields[] = {false, false, false, false, false, false};
bool bAlarmSelected = false;
bool bMenuSelected = false;
byte currentAlarm = 0;

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