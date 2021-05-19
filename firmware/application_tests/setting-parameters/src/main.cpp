/**
 * Weekday buttons should be pressed for longer than 2 seconds before they change state.  This will prevent accidental pushes from changing the alarm settings.
 * 
 */

#include <Arduino.h>
#include "Weekdays.h"
#include "VisualElements.h"
#include "WeekdayButtons.h"
#include "ParameterSetup.h"

typedef enum
{
	STORE_SETTINGS,
	CLOCK_ALARM,
	SET_SUNRISE,
	SET_VOLUME,
	SET_SONGCHOICE,
	SET_ALARM_MINUTES,
	SET_ALARM_HOURS
} OPERATING_MODE;

CFG config;
AsyncDelay ledTimer(500, AsyncDelay::MILLIS);
AsyncDelay parameterSetupTimeout(SETUP_TIMEOUT, AsyncDelay::MILLIS);
OPERATING_MODE state = CLOCK_ALARM;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);
	while (!Serial)
		;
	delay(200);
	Serial.printf("Build %s\r\n", __TIMESTAMP__);
	initPeripherals(&config);
	setWeekdayPointer(&config.wd);
	setBrightness(8);
	Serial.println("ready");
}

void loop()
{
	RotaryEncoder_Tactiles::DIRECTION dir;
	KEY_CODE key;
	bool rotEncMoved = isRotaryEncoderChanged(dir);

	//process buttons
	if (isButtonChanged(key))
	{
		switch (key)
		{
		case KEY_SUNRISE:
			state = state == SET_SUNRISE ? STORE_SETTINGS : SET_SUNRISE;
			break;
		case KEY_VOLUME:
			state = state == SET_VOLUME ? STORE_SETTINGS : SET_VOLUME;
			break;
		case KEY_SONGCHOICE:
			state = state == SET_SONGCHOICE ? STORE_SETTINGS : SET_SONGCHOICE;
			break;
		case KEY_ALARM:
			switch (state)
			{
			case SET_ALARM_HOURS:
				state = SET_ALARM_MINUTES;
				break;
			case SET_ALARM_MINUTES:
				state = STORE_SETTINGS;
				break;
			default:
				state = SET_ALARM_HOURS;
				break;
			}
			break;
		default:
			if (processWeekdayButtonEvent(key))
			{
				state = STORE_SETTINGS;
			}
			break;
		}
		parameterSetupTimeout.restart();
	}
	if (rotEncMoved)
	{
		parameterSetupTimeout.restart();
	}
	if (parameterSetupTimeout.isExpired() && state != CLOCK_ALARM)
	{
		state = STORE_SETTINGS;
	}

	//Act according to state
	switch (state)
	{
	case STORE_SETTINGS:
		disableParameterSetting(&config);
		state = CLOCK_ALARM;
		break;
	case CLOCK_ALARM:
		showClockTime(0, 1, false, false);
		showAlarmDisplay(AL_BOTH_OFF, false);
		break;
	case SET_SUNRISE:
		if (rotEncMoved)
		{
			updateParameter(&config.sunRiseSetting, 0.0f, 100.0f, 10.0f, dir);
		}
		showSunriseSetting(config.sunRiseSetting);
		showSunlightSetting(rescaleParameter(&config.sunRiseSetting, 0.0f, 100.0f, 10.0f));
		break;
	case SET_VOLUME:
		if (rotEncMoved)
		{
			updateParameter(&config.volume, 0, MAX_VOLUME, 1, dir);
		}
		playMusic(config.songchoice, config.volume);
		showSongVolume(rescaleParameter(&config.volume, 0, MAX_VOLUME, 10));
		break;
	case SET_SONGCHOICE:
		if (rotEncMoved)
		{
			updateParameter(&config.songchoice, 1, getTrackCount(), 1, dir);
		}
		playMusic(config.songchoice, config.volume);
		showSongChoice(rescaleParameter(&config.songchoice, 1, getTrackCount(), 10));
		break;
	case SET_ALARM_HOURS:
		if (rotEncMoved)
		{
			updateParameter(&config.alarmHours, 0, 23, 1, dir);
		}
		showAlarmTime(config.alarmHours, config.alarmMinutes);
		showAlarmDisplay(AL_HOURS_ONLY, true);
		break;
	case SET_ALARM_MINUTES:
		if (rotEncMoved)
		{
			updateParameter(&config.alarmMinutes, 0, 55, 5, dir);
		}
		showAlarmTime(config.alarmHours, config.alarmMinutes);
		showAlarmDisplay(AL_MINUTES_ONLY, true);
		break;
	default:
		break;
	}

	if (ledTimer.isExpired())
	{
		ledTimer.restart();
		digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
	}
	showWeekDay(config.wd);
	redraw();
}