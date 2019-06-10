/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"

FieldParameter lightness = {0, 50, 100, 5};
FieldParameter volume = {0, 50, 100, 5};
FieldParameter dayBright = {0, 50, 100, 5};
FieldParameter dayNight = {0, 50, 100, 5};
FieldParameter nightBright = {0, 50, 100, 5};
SelectParameter song = {3, 9, playSong};
bool matrixFields[] = {false, false, false, false, false, false};
bool weekdays[] = {false, false, false, false, false, false, false};
bool bAlarmSelected = false;
bool bMenuSelected = false;
FieldParameter hours = {0, 12, 23, 1};

void setup()
{
  Serial.begin(115200);
  initMenu();
  // delay(1000);
}

void loop()
{
  //animation();
  renderMenu();
}