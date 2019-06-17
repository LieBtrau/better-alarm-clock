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

bool matrixFields[] = {false, false, false, false, false, false};
bool bAlarmSelected = false;
bool bMenuSelected = false;
byte currentAlarm = 0;

void setHours(bool action);
void setMinutes(bool action);

// 32x16 LED Matrix elements
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

LedMatrixField fldLightness = LedMatrixField(&matrix, {0, 0}, {11, 2}, &alarms.lightness);
LedMatrixField fldVolume = LedMatrixField(&matrix, {0, 7}, {11, 9}, &alarms.volume);
LedMatrixSelect sldSong = LedMatrixSelect(&matrix, {0, 13}, {11, 15}, &alarms.song);
LedMatrixField fldDayBright = LedMatrixField(&matrix, {20, 0}, {31, 2}, &compar.dayBright);
LedMatrixField fldDayNight = LedMatrixField(&matrix, {20, 7}, {31, 9}, &compar.dayNight);
LedMatrixField fldNightBright = LedMatrixField(&matrix, {20, 13}, {31, 15}, &compar.nightBright);

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

LedToggle tglMonday = LedToggle(&myCharlie, &ledMatrix[MONDAY], alarms.weekdays);
LedToggle tglTuesday = LedToggle(&myCharlie, &ledMatrix[TUESDAY], alarms.weekdays + 1);
LedToggle tglWednesday = LedToggle(&myCharlie, &ledMatrix[WEDNESDAY], alarms.weekdays + 2);
LedToggle tglThursday = LedToggle(&myCharlie, &ledMatrix[THURSDAY], alarms.weekdays + 3);
LedToggle tglFriday = LedToggle(&myCharlie, &ledMatrix[FRIDAY], alarms.weekdays + 4);
LedToggle tglSaturday = LedToggle(&myCharlie, &ledMatrix[SATURDAY], alarms.weekdays + 5);
LedToggle tglSunday = LedToggle(&myCharlie, &ledMatrix[SUNDAY], alarms.weekdays + 6);

LedToggle tglAlarm = LedToggle(&myCharlie, &ledMatrix[ALARMTIME], &bAlarmSelected);
LedToggle tglMenu = LedToggle(&myCharlie, &ledMatrix[MENU], &bMenuSelected);

// Seven segment display elements
Adafruit_7segment matrix7 = Adafruit_7segment();
SevenSegmentField fldHours = SevenSegmentField(&matrix7, SevenSegmentField::LEFTPOS, &alarms.hours);
SevenSegmentField fldMinutes = SevenSegmentField(&matrix7, SevenSegmentField::RIGHTPOS, &alarms.minutes);

PushButton matrixButtons[] =
    {{LIGHTNESS, &tglLightness, &fldLightness},
     {VOLUME, &tglVolume, &fldVolume},
     {SONGCHOICE, &tglSongChoice, &sldSong},
     {DAYDISPLAYBRIGHTNESS, &tglDayBrightness, &fldDayBright},
     {DAYNIGHTLEVEL, &tglDayNight, &fldDayNight},
     {NIGHTDISPLAYBRIGHTNESS, &tglNightBrightness, &fldNightBright}};
PushButton alarmTimeButton = {ALARMTIME, &tglAlarm};
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
      mcp.writePinMode(1, pinModes);
      mcp.writeGPIO(1, gpioStates);
    }
  }
}

void keyChanged(byte key)
{
  static byte lastKey = 0xFF;
  for (int i = 0; i < 6; i++)
  {
    matrixButtons[i].doAction((matrixButtons[i].key() == key) && (key != lastKey));
    if (matrixButtons[i].key() == key)
    {
      if (key == lastKey)
      {
        rec.setConsumer(nullptr, false);
      }
      else
      {
        rec.setConsumer(matrixButtons[i].getParam(), false);
        alarmTimeButton.doAction(false);
      }
    }
  }
  if (key == ALARMTIME)
  {
    alarmTimeButton.doAction(true);
  } 
  lastKey = key == lastKey ? 0xFF : key;
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
  for (int i = 0; i < 6; i++)
  {
    matrixLEDs[i]->render();
  }
  tglAlarm.render();
  showLedState();
}

void initMenu()
{
  sldSong.setLinkedParameter(&fldVolume);
  fldVolume.setLinkedParameter(&sldSong);
  mcp.begin(); // use default address 0
  matrix.init();
  matrix.fillScreen(0);
  matrix7.begin(0x70);
  rec.init();
  matrix.setFont(&Picopixel);
  matrix.setCursor(4, 10);
  matrix.setFont(&TomThumb);
  matrix.setCursor(4, 10);
  alarmTimeButton.setAction(setHours);
  matrix.print("ALARM3");
  matrix.write(); // Send bitmap to display
  keyb.init(writePinModes, writePullups);
  keyb.setCallback_keyReleased(keyChanged);
}