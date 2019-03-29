//We always have to include the library
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
const byte NR_OF_MATRICES = 8;
LedControl lc = LedControl(PA7, PA5, PA1, NR_OF_MATRICES);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 100;

void setup()
{
	/*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
	for (byte i = 0; i < NR_OF_MATRICES; i++)
	{
		lc.shutdown(i, false);
		/* Set the brightness to a medium values */
		lc.setIntensity(i, 1);
		/* and clear the display */
		lc.clearDisplay(i);
	}
	lc.setLed(7, 2, 1, true);
}

void loop()
{
}
