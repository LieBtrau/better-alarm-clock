/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"

CommonParameters compar;
FieldParameter lightness = {0, 50, 100, 5};
FieldParameter volume = {0, 50, 100, 5};
SelectParameter song = {3, 9, playSong};
bool weekdays[] = {false, false, false, false, false, false, false};

bool matrixFields[] = {false, false, false, false, false, false};
bool bAlarmSelected = false;
bool bMenuSelected = false;
FieldParameter hours = {0, 12, 23, 1};
FieldParameter minutes = {0, 15, 55, 5};

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