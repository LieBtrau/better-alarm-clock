#include <Arduino.h>
#include "inputs.h"

AsyncDelay ledTimer(500, AsyncDelay::MILLIS);
SX1509 io1, io2;
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
	if (!setupInputs(&io1, &io2))
	{
		Serial.println("Can't setup all input devices.");
		while (true)
			;
	}
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
		Serial.println(key);
	}
	if (ledTimer.isExpired())
	{
		ledTimer.restart();
		digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
	}
}