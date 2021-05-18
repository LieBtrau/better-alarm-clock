/**
 * Weekday buttons should be pressed for longer than 2 seconds before they change state.  This will prevent accidental pushes from changing the alarm settings.
 * 
 */

#include <Arduino.h>
#include "inputs.h"
#include "Weekdays.h"
#include "VisualElements.h"
#include "WeekdayButtons.h"

WEEKDAYS wd;
SX1509 io1, io2;
AsyncDelay ledTimer(500, AsyncDelay::MILLIS);
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);
	while (!Serial)
		;
	delay(200);
	Serial.printf("Build %s\r\n", __TIMESTAMP__);
	// put your setup code here, to run once:
	if (!io1.begin(IO1_SX1509_ADDRESS))
	{
		while (1)
			;
	}
	if (!io2.begin(IO2_SX1509_ADDRESS))
	{
		while (1)
			;
	}

	// Use the internal 2MHz oscillator.
	// Set LED clock to 500kHz (2MHz / (2^(3-1)):
	io1.clock(INTERNAL_CLOCK_2MHZ, 3);
	io2.clock(INTERNAL_CLOCK_2MHZ, 3);

	if (!setupInputs(&io1, &io2))
	{
		Serial.println("Can't setup all input devices.");
		while (true)
			;
	}
	if (!initVisualElements(&io1, &io2))
	{
		Serial.println("Failed to initialize.");
		while (1)
			; // If we fail to communicate, loop forever.
	}
	setWeekdayPointer(&wd);
	setBrightness(8);
	Serial.println("ready");
}

void loop()
{
	RotaryEncoder_Tactiles::DIRECTION dir;
	KEY_CODE key;
	if (isRotaryEncoderChanged(dir))
	{
		Serial.println(dir, DEC);
	}
	if (isButtonChanged(key))
	{
		processWeekdayButtonEvent(key);
		Serial.println(wd, HEX);
	}
	if (ledTimer.isExpired())
	{
		ledTimer.restart();
		digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
	}
	showWeekDay(wd);
	redraw();
}