/**
 * Weekday buttons should be pressed for longer than 2 seconds before they change state.  This will prevent accidental pushes from changing the alarm settings.
 * 
 */

#include <Arduino.h>
#include "inputs.h"
#include "Weekdays.h"
#include "VisualElements.h"
#include "WeekdayButtons.h"
#include "ParameterSetup.h"

typedef enum
{
  CLOCK_ALARM,
  SET_SUNRISE,
  SET_VOLUME,
  SET_SONGCHOICE,
  SET_ALARM_MINUTES,
  SET_ALARM_HOURS
} OPERATING_MODE;

WEEKDAYS wd;
SX1509 io1, io2;
AsyncDelay ledTimer(500, AsyncDelay::MILLIS);
AsyncDelay parameterSetupTimeout(5000, AsyncDelay::MILLIS);
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address
OPERATING_MODE state = CLOCK_ALARM;

float sunRiseSetting = 50.0;
int volume = 10;
int songchoice = 12;
int alarmHours = 9;
int alarmMinutes=10;

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
  bool rotEncMoved = isRotaryEncoderChanged(dir);

  //process buttons
  if (isButtonChanged(key))
  {
    switch (key)
    {
    case KEY_SUNRISE:
      state = state == SET_SUNRISE ? CLOCK_ALARM : SET_SUNRISE;
      break;
    case KEY_VOLUME:
      state = state == SET_VOLUME ? CLOCK_ALARM : SET_VOLUME;
      break;
    case KEY_SONGCHOICE:
      state = state == SET_SONGCHOICE ? CLOCK_ALARM : SET_SONGCHOICE;
      break;
    case KEY_ALARM:
      switch (state)
      {
      case SET_ALARM_HOURS:
        state = SET_ALARM_MINUTES;
        break;
      case SET_ALARM_MINUTES:
        state = CLOCK_ALARM;
        break;
      default:
        state = SET_ALARM_HOURS;
        break;
      }
      break;
    default:
      processWeekdayButtonEvent(key);
      break;
    }
    parameterSetupTimeout.restart();
  }
  if (rotEncMoved)
  {
    parameterSetupTimeout.restart();
  }
  if (parameterSetupTimeout.isExpired())
  {
    state = CLOCK_ALARM;
  }

  //Act according to state
  switch (state)
  {
  case CLOCK_ALARM:
    showClockTime(0, 1, false, false);
    break;
  case SET_SUNRISE:
    if (rotEncMoved)
    {
      updateParameter(&sunRiseSetting, 0.0f, 100.0f, 10.0f, dir);
    }
    showSunlightSetting(rescaleParameter(&sunRiseSetting, 0.0f, 100.0f, 10.0f));
    break;
  case SET_VOLUME:
    if (rotEncMoved)
    {
      updateParameter(&volume, 0, 32, 1, dir);
    }
    showSongVolume(rescaleParameter(&volume, 0, 32, 10));
    break;
  case SET_SONGCHOICE:
    if (rotEncMoved)
    {
      updateParameter(&songchoice, 1, 24, 1, dir);
    }
    showSongChoice(rescaleParameter(&songchoice, 1, 24, 10));
    break;
  case SET_ALARM_HOURS:
    if(rotEncMoved)
    {
      updateParameter(&alarmHours, 0,23,1, dir);
    }
    showAlarmTime(alarmHours, alarmMinutes);
    showAlarmDisplay(AL_HOURS_ONLY, true);
    break;
  case SET_ALARM_MINUTES:
    if(rotEncMoved)
    {
      updateParameter(&alarmMinutes, 0, 55, 5, dir);
    }
    showAlarmTime(alarmHours, alarmMinutes);
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
  showWeekDay(wd);
  redraw();
}