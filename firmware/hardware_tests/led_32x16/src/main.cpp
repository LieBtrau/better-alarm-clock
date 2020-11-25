#include "Wire.h"
#include "Max72xxPanel.h"

int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA4;

Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

void setup()
{
  matrix.init();
  matrix.setCursor(2, 5);
  matrix.print("99:99");
  matrix.write(); // Send bitmap to display
}

void loop()
{
}