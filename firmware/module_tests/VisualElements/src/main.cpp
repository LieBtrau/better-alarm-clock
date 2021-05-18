#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "SPI.h"
#include "VisualElements.h"

byte hours = 0;
byte minutes = 0;
// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address
SX1509 io1, io2;					  // Create an SX1509 object to be used throughout

void setup()
{
	Serial.begin(115200);
	Serial.printf("Build %s\r\n", __TIMESTAMP__);
	if (!io1.begin(IO1_SX1509_ADDRESS))
	{
		Serial.println("Failed to communicate with io1");
		while (1)
			; // If we fail to communicate, loop forever.
	}
	if (!io2.begin(IO2_SX1509_ADDRESS))
	{
		Serial.println("Failed to communicate with io2");
		while (1)
			; // If we fail to communicate, loop forever.
	}
	if (!initVisualElements(&io1, &io2))
	{
		Serial.println("Failed to initialize visual elements.");
		while (1)
			; // If we fail to communicate, loop forever.
	}
}

void loop()
{
	showAlarmDisplay((ALARM_DISPLAY)(minutes & 0x3));
	showAlarmTime(hours, minutes);
	setBrightness(minutes & 0xF);
	//sre.setBrightness(minutes * 100 / 60);
	showWeekDay((WEEKDAYS)minutes);

	if (millis() > 5000)
	{
		if (++minutes > 59)
		{
			hours = hours == 23 ? 0 : hours + 1;
			minutes = 0;
		}
		showClockTime(hours, minutes, true, false);
	}
	redraw();
	delay(100);
}