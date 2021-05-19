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
	matrix.write(); // Send bitmap to display
}

void loop()
{
	matrix.setCursor(2, 5);
	matrix.fillScreen(0);
	matrix.print("43:21");
	matrix.write();
	delay(1000);
	matrix.setCursor(2, 5);
	matrix.fillScreen(0);
	matrix.print("12:34");
	matrix.write();
	delay(1000);
}