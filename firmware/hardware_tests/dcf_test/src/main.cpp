#include "AsyncDelay.h"
#include "robustDcf.h"
#include "pins.h"

RobustDcf rd(pin_DCF, true);
HardwareSerial *ser1 = &Serial;
AsyncDelay ledTimer(500, AsyncDelay::MILLIS);
void runClock();

void setup()
{
	while (!*ser1)
		;
	ser1->begin(115200);
	ser1->printf("Build %s\r\n", __TIMESTAMP__);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(pin_nEn_DCF, OUTPUT);
	digitalWrite(pin_nEn_DCF, LOW);
	rd.init();
}

void loop()
{
	if (ledTimer.isExpired())
	{
		ledTimer.restart();
		digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
	}
	runClock();
}

void runClock()
{
	Chronos::EpochTime unixEpoch;
	if (rd.update(unixEpoch))
	{
		ser1->println(unixEpoch);
		TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
		TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};	  //Last Sunday of October, at 3AM, go to UTC+60min
		Timezone myTZ(myDST, mySTD);
		Chronos::DateTime localTime = myTZ.toLocal(unixEpoch);
		localTime.printTo(*ser1);
		ser1->println();
	}
}