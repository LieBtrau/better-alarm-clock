#include "Max72xxPanel.h"
#include "Fonts/Picopixel.h"
#include "Fonts/TomThumb.h"
#include <SPI.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_MCP23017.h"
#include "KeyboardScan.h"
#include "menuNav.h"
#include "pins.h"
#include "rotaryEncoder.h"
#include "parameters.h"
#include "actions.h"

bool matrixFields[] = {false, false, false, false, false, false};
bool bAlarmSelected = false;
bool bMenuSelected = false;

FieldParameter lightness = {0, nullptr, 100, 5, showLightness, stopLightness};
FieldParameter volume = {0, nullptr, 30, 1, setVolume, stopSong};
SelectParameter song = {nullptr, 1, 10, playSong, stopSong};
FieldParameter hours = {0, nullptr, 23, 1, nullptr, nullptr};
FieldParameter minutes = {0, nullptr, 55, 5, nullptr, nullptr};

void setHours(bool action);
void setMinutes(bool action);
void assignAlarmConfig(AlarmConfig *config);

// 32x16 LED Matrix elements
const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 2;
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

FieldParameter dayBright = {0, nullptr, 15, 1, setLedArrayBrightness, nullptr};
FieldParameter dayNight = {0, nullptr, 10, 1, nullptr, nullptr}; //logarithmic parameter.  Actual threshold value will be 2^dayNight
FieldParameter nightBright = {0, nullptr, 15, 1, setLedArrayBrightness, nullptr};

void assignCommonConfig(CommonConfig *config)
{
  dayBright.cur = &config->dayBright;
  dayNight.cur = &config->dayNight;
  nightBright.cur = &config->nightBright;
}

LedMatrixField fldLightness = LedMatrixField(&matrix, {0, 0}, {11, 2}, &lightness);
LedMatrixField fldVolume = LedMatrixField(&matrix, {0, 7}, {11, 9}, &volume);
LedMatrixSelect sldSong = LedMatrixSelect(&matrix, {0, 13}, {11, 15}, &song);
LedMatrixField fldDayBright = LedMatrixField(&matrix, {20, 0}, {31, 2}, &dayBright);
LedMatrixField fldDayNight = LedMatrixField(&matrix, {20, 7}, {31, 9}, &dayNight);
LedMatrixField fldNightBright = LedMatrixField(&matrix, {20, 13}, {31, 15}, &nightBright);

// Charlieplexed LEDs
Chaplex myCharlie;
CharlieLed ledMatrix[] = {
    //{anode_registerbit, cathode_registerbit},
    {2, 1}, //D4 - ALARMTIME = 0
    {1, 2}, //D3 - SONGCHOICE = 1
    {1, 0}, //D2 - VOLUME = 2
    {0, 1}, //D1 - LIGHTNESS = 3
    {0, 4}, //D14 - DAYNIGHTLEVEL = 4
    {2, 0}, //D10 - NIGHTDISPLAYBRIGHTNESS = 5
    {0, 2}, //D7 - MENU = 6
    {4, 0}, //D15 - DAYDISPLAYBRIGHTNESS = 7
    {1, 3}, //D13 - MONDAY = 8
    {2, 3}, //D5 - TUESDAY = 9
    {3, 2}, //D8 - WEDNESDAY = 10
    {3, 4}, //D6 - THURSDAY = 11
    {4, 3}, //D9 - FRIDAY = 12
    {4, 2}, //D12 - SATURDAY = 13
    {2, 4}  //D11 - SUNDAY = 14
};

LedToggle tglLightness = LedToggle(&myCharlie, &ledMatrix[LIGHTNESS], matrixFields);
LedToggle tglVolume = LedToggle(&myCharlie, &ledMatrix[VOLUME], matrixFields + 1);
LedToggle tglSongChoice = LedToggle(&myCharlie, &ledMatrix[SONGCHOICE], matrixFields + 2);
LedToggle tglDayBrightness = LedToggle(&myCharlie, &ledMatrix[DAYDISPLAYBRIGHTNESS], matrixFields + 3);
LedToggle tglDayNight = LedToggle(&myCharlie, &ledMatrix[DAYNIGHTLEVEL], matrixFields + 4);
LedToggle tglNightBrightness = LedToggle(&myCharlie, &ledMatrix[NIGHTDISPLAYBRIGHTNESS], matrixFields + 5);
LedToggle *matrixLEDs[] = {&tglLightness, &tglVolume, &tglSongChoice, &tglDayBrightness, &tglDayNight, &tglNightBrightness};

LedToggle tglMonday = LedToggle(&myCharlie, &ledMatrix[MONDAY], nullptr);
LedToggle tglTuesday = LedToggle(&myCharlie, &ledMatrix[TUESDAY], nullptr);
LedToggle tglWednesday = LedToggle(&myCharlie, &ledMatrix[WEDNESDAY], nullptr);
LedToggle tglThursday = LedToggle(&myCharlie, &ledMatrix[THURSDAY], nullptr);
LedToggle tglFriday = LedToggle(&myCharlie, &ledMatrix[FRIDAY], nullptr);
LedToggle tglSaturday = LedToggle(&myCharlie, &ledMatrix[SATURDAY], nullptr);
LedToggle tglSunday = LedToggle(&myCharlie, &ledMatrix[SUNDAY], nullptr);
LedToggle *weekdayLEDs[] = {&tglMonday, &tglTuesday, &tglWednesday, &tglThursday, &tglFriday, &tglSaturday, &tglSunday};

LedToggle tglAlarm = LedToggle(&myCharlie, &ledMatrix[ALARMTIME], &bAlarmSelected);
LedToggle tglMenu = LedToggle(&myCharlie, &ledMatrix[MENU], &bMenuSelected);

// Seven segment display elements
Adafruit_7segment matrix7 = Adafruit_7segment();
SevenSegmentField fldHours = SevenSegmentField(&matrix7, SevenSegmentField::LEFTPOS, &hours);
SevenSegmentField fldMinutes = SevenSegmentField(&matrix7, SevenSegmentField::RIGHTPOS, &minutes);

ParameterPushButton matrixButtons[] =
    {{LIGHTNESS, &tglLightness, &fldLightness},
     {VOLUME, &tglVolume, &fldVolume},
     {SONGCHOICE, &tglSongChoice, &sldSong},
     {DAYDISPLAYBRIGHTNESS, &tglDayBrightness, &fldDayBright},
     {DAYNIGHTLEVEL, &tglDayNight, &fldDayNight},
     {NIGHTDISPLAYBRIGHTNESS, &tglNightBrightness, &fldNightBright}};
TogglePushButton weekdayButtons[] =
    {{MONDAY, &tglMonday},
     {TUESDAY, &tglTuesday},
     {WEDNESDAY, &tglWednesday},
     {THURSDAY, &tglThursday},
     {FRIDAY, &tglFriday},
     {SATURDAY, &tglSaturday},
     {SUNDAY, &tglSunday}};
ActionPushButton alarmTimeButton = {ALARMTIME, &tglAlarm};
Adafruit_MCP23017 mcp;
KeyboardScan keyb;
extern RotaryEncoderConsumer rec;

void writePinModes(byte data)
{
  mcp.writePinMode(0, data);
}

void writePullups(byte data)
{
  mcp.writePullUps(0, data);
}

void writeGpio(byte data)
{
  mcp.writeGPIO(0, data);
}

byte readGpio()
{
  return mcp.readGPIO(0);
}

void showLedState()
{
  static unsigned long ultimer = 0;
  if (millis() > ultimer + 5)
  {
    ultimer = millis();
    byte pinModes = mcp.readPinMode(1);
    byte gpioStates = mcp.readGPIO(1);
    if (myCharlie.showLedState(pinModes, gpioStates))
    {
      mcp.writePinMode(1, pinModes | 0x1F); //set all LED pins as input to avoid that unwanted LEDs light.
      mcp.writeGPIO(1, gpioStates);
      //unwanted LEDs would go on here, in between the two calls to the MCP23017
      mcp.writePinMode(1, pinModes);
    }
  }
}

void keyChanged(byte key)
{
  static byte lastKey = 0xFF;
  byte matrixElements = sizeof(matrixButtons) / sizeof(matrixButtons[0]);
  for (int i = 0; i < matrixElements; i++)
  {
    matrixButtons[i].doAction((matrixButtons[i].key() == key) && (key != lastKey));
    if (matrixButtons[i].key() == key)
    {
      if (key == lastKey)
      {
        rec.setConsumer(nullptr, false); //pushing same button will toggle it.
      }
      else
      {
        rec.setConsumer(matrixButtons[i].getParam(), false);
        alarmTimeButton.doAction(false);
      }
    }
  }
  byte weekdayElements = sizeof(weekdayButtons) / sizeof(weekdayButtons[0]);
  for (int i = 0; i < weekdayElements; i++)
  {
    if (weekdayButtons[i].key() == key)
    {
      weekdayButtons[i].toggle();
      //rec.setConsumer(nullptr, false);  //Enable this line to disable rotary encoder on other elements
    }
  }
  if (key == ALARMTIME)
  {
    alarmTimeButton.doAction(true);
  }
  lastKey = key == lastKey ? 0xFF : key; // if the same button is pressed for a third time, control must be on again : toggle effect.
}

void setMinutes(bool action)
{
  rec.setConsumer(&fldMinutes, true);
  alarmTimeButton.setAction(setHours);
}

void setHours(bool action)
{
  rec.setConsumer(&fldHours, true);
  alarmTimeButton.setAction(setMinutes);
}

void renderMenu()
{
  bool flashing = rec.poll();
  keyb.updateKeys(writeGpio, readGpio);
  if (fldLightness.render() || sldSong.render() || fldVolume.render() || fldDayBright.render() || fldDayNight.render() || fldNightBright.render())
  {
    matrix.write();
  }
  if (fldHours.render() || fldMinutes.render() || flashing)
  {
    matrix7.writeDisplay();
  }
  byte matrixElements = sizeof(matrixLEDs) / sizeof(matrixLEDs[0]);
  for (int i = 0; i < matrixElements; i++)
  {
    matrixLEDs[i]->render();
  }
  byte weekdayElements = sizeof(weekdayLEDs) / sizeof(weekdayLEDs[0]);
  for (int i = 0; i < weekdayElements; i++)
  {
    weekdayLEDs[i]->render();
  }
  tglAlarm.render();
  showLedState();
}

void initMenu()
{
  sldSong.setLinkedParameter(&fldVolume);
  fldVolume.setLinkedParameter(&sldSong);
  alarmTimeButton.setAction(setHours);
  mcp.begin(); // use default address 0
  matrix.init();
  matrix.fillScreen(0);
  matrix7.begin(0x70);
  rec.init();
  matrix.setFont(&Picopixel);
  matrix.setCursor(4, 10);
  matrix.setFont(&TomThumb);
  matrix.setCursor(4, 10);
  matrix.print("ALARM3");
  matrix.write(); // Send bitmap to display
  keyb.init(writePinModes, writePullups);
  keyb.setCallback_keyReleased(keyChanged);
  tglSaturday.isLedOn();
}

void assignAlarmConfig(AlarmConfig *config)
{
  lightness.cur = &config->lightness;
  volume.cur = &config->volume;
  song.cur = &config->song;
  hours.cur = &config->hours;
  minutes.cur = &config->minutes;
  byte weekdayElements = sizeof(weekdayLEDs) / sizeof(weekdayLEDs[0]);
  for (int i = 0; i < weekdayElements; i++)
  {
    weekdayLEDs[i]->setSource(&config->weekdays[i]);
  }
}
