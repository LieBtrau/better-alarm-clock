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
#include "ButtonManager.h"
#include "fontBig.h"

extern ActionMgr actionMgr;
bool matrixFields[] = {false, false, false, false, false, false};
bool bAlarmSelected = false;
bool bMenuSelected = false;
bool clockMode = true;

MenuMgr menuMgr;
FieldParameter lightness = {0, nullptr, 100, 5, showLightness, stopLightness};
FieldParameter volume = {0, nullptr, 30, 1, setVolume, stopSong};
SelectParameter song = {nullptr, 1, 10, playSong, stopSong};
FieldParameter hours = {0, nullptr, 23, 1, nullptr, nullptr};
FieldParameter minutes = {0, nullptr, 55, 5, nullptr, nullptr};

//Some C++ boiler plate
void setHours(bool action);
void setMinutes(bool action);
void showAlarm(byte alarmNr);
void showAlarm1(bool action);
void showAlarm2(bool action);
void drawClock(ClockTime ct);
void hideClock(void);
void showClock(bool action);
void showDayBrightness(bool action);
void shownNightBrightness(bool action);
void showDayNight(bool action);
extern void getAlarmConfig(byte nr);

// 32x16 LED Matrix elements
const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 2;
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

FieldParameter dayBright = {0, nullptr, 15, 1, setLedArrayBrightness, nullptr};
FieldParameter dayNight = {0, nullptr, 10, 1, nullptr, nullptr}; //logarithmic parameter.  Actual threshold value will be 2^dayNight
FieldParameter nightBright = {0, nullptr, 15, 1, setLedArrayBrightness, nullptr};

//Objects to draw on LED matrix
ClockFace clockface = ClockFace(drawClock, hideClock);
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

//Objects for controlling alarm settings : sun light emulation intensity, song choice, song volume
LedToggle tglLightness = LedToggle(&myCharlie, &ledMatrix[LIGHTNESS], matrixFields);
LedToggle tglVolume = LedToggle(&myCharlie, &ledMatrix[VOLUME], matrixFields + 1);
LedToggle tglSongChoice = LedToggle(&myCharlie, &ledMatrix[SONGCHOICE], matrixFields + 2);
ParameterPushButton btnLightness = {LIGHTNESS, &tglLightness, &fldLightness};
ParameterPushButton btnVolume = {VOLUME, &tglVolume, &fldVolume};
ParameterPushButton btnSong = {SONGCHOICE, &tglSongChoice, &sldSong};
ParameterButtonManager mgrBtnAlarm;
//Extra alarm settings: enabling and disabling days of week.
LedToggle tglMonday = LedToggle(&myCharlie, &ledMatrix[MONDAY], nullptr);
LedToggle tglTuesday = LedToggle(&myCharlie, &ledMatrix[TUESDAY], nullptr);
LedToggle tglWednesday = LedToggle(&myCharlie, &ledMatrix[WEDNESDAY], nullptr);
LedToggle tglThursday = LedToggle(&myCharlie, &ledMatrix[THURSDAY], nullptr);
LedToggle tglFriday = LedToggle(&myCharlie, &ledMatrix[FRIDAY], nullptr);
LedToggle tglSaturday = LedToggle(&myCharlie, &ledMatrix[SATURDAY], nullptr);
LedToggle tglSunday = LedToggle(&myCharlie, &ledMatrix[SUNDAY], nullptr);
TogglePushButton btnMonday = {MONDAY, &tglMonday};
TogglePushButton btnTuesday = {TUESDAY, &tglTuesday};
TogglePushButton btnWednesday = {WEDNESDAY, &tglWednesday};
TogglePushButton btnThursday = {THURSDAY, &tglThursday};
TogglePushButton btnFriday = {FRIDAY, &tglFriday};
TogglePushButton btnSaturday = {SATURDAY, &tglSaturday};
TogglePushButton btnSunday = {SUNDAY, &tglSunday};
ToggleButtonManager mgrBtnWeekday;

//Objects for controlling LED array brightness
LedToggle tglDayBrightness = LedToggle(&myCharlie, &ledMatrix[DAYDISPLAYBRIGHTNESS], matrixFields + 3);
LedToggle tglDayNight = LedToggle(&myCharlie, &ledMatrix[DAYNIGHTLEVEL], matrixFields + 4);
LedToggle tglNightBrightness = LedToggle(&myCharlie, &ledMatrix[NIGHTDISPLAYBRIGHTNESS], matrixFields + 5);
ParameterPushButton btnDayBright = {DAYDISPLAYBRIGHTNESS, &tglDayBrightness, &fldDayBright};
ParameterPushButton btnDayNight = {DAYNIGHTLEVEL, &tglDayNight, &fldDayNight};
ParameterPushButton btnNightBright = {NIGHTDISPLAYBRIGHTNESS, &tglNightBrightness, &fldNightBright};
ParameterButtonManager mgrBtnBrightness;

LedToggle tglAlarm = LedToggle(&myCharlie, &ledMatrix[ALARMTIME], &bAlarmSelected);
LedToggle tglMenu = LedToggle(&myCharlie, &ledMatrix[MENU], &bMenuSelected);

// Seven segment display elements
Adafruit_7segment matrix7 = Adafruit_7segment();
SevenSegmentField fldHours = SevenSegmentField(&matrix7, SevenSegmentField::LEFTPOS, &hours);
SevenSegmentField fldMinutes = SevenSegmentField(&matrix7, SevenSegmentField::RIGHTPOS, &minutes);

ActionPushButton alarmTimeButton = {ALARMTIME, &tglAlarm};
ActionPushButton menuButton = {MENU, &tglMenu};
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
  bool encoderCoupled = false;
  if (mgrBtnBrightness.keyPressed(key) | mgrBtnAlarm.keyPressed(key))
  {
    alarmTimeButton.doAction(false);
    encoderCoupled = true;
  }
  if (key == ALARMTIME && alarmTimeButton.isEnabled())
  {
    alarmTimeButton.doAction(true);
    encoderCoupled = true;
  }
  if (!encoderCoupled)
  {
    rec.setConsumer(nullptr, false);
  }
  mgrBtnWeekday.keyPressed(key);
  if (key == MENU)
  {
    alarmTimeButton.doAction(false);
    menuButton.doAction(true);
  }
}

void setMinutes(bool action)
{
  if (action)
  {
    rec.setConsumer(&fldMinutes, true);
    alarmTimeButton.setAction(setHours);
  }
}

void setHours(bool action)
{
  if (action)
  {
    rec.setConsumer(&fldHours, true);
    alarmTimeButton.setAction(setMinutes);
  }
}

void showClock(bool action)
{
  mgrBtnAlarm.disable();
  mgrBtnWeekday.disable();
  alarmTimeButton.disable();
  clockface.setVisible(true);
  menuButton.setAction(showAlarm1);
  fldHours.setVisible(false);
  fldMinutes.setVisible(false);
  clockMode = true;
}

void showAlarm2(bool action)
{
  getAlarmConfig(2);
  showAlarm(2);
  menuButton.setAction(showClock);
}

void showAlarm1(bool action)
{
  getAlarmConfig(1);
  showAlarm(1);
  menuButton.setAction(showAlarm2);
}

bool pollMenu()
{
  keyb.updateKeys(writeGpio, readGpio);
  return rec.poll();
}

void showParameterMenu(bool isFlashing)
{
  if (mgrBtnAlarm.render() | mgrBtnBrightness.render() | clockface.render())
  {
    matrix.write();
  }
  if (fldHours.render() | fldMinutes.render() | isFlashing)
  {
    matrix7.writeDisplay();
  }
  mgrBtnWeekday.render();
  alarmTimeButton.render();
  showLedState();
}

void initMenu(byte totalTrackCount)
{
  mgrBtnAlarm.addButton(&btnLightness);
  mgrBtnAlarm.addButton(&btnVolume);
  mgrBtnAlarm.addButton(&btnSong);
  mgrBtnAlarm.attachRotaryEncoder(&rec);

  mgrBtnWeekday.addButton(&btnMonday);
  mgrBtnWeekday.addButton(&btnTuesday);
  mgrBtnWeekday.addButton(&btnWednesday);
  mgrBtnWeekday.addButton(&btnThursday);
  mgrBtnWeekday.addButton(&btnFriday);
  mgrBtnWeekday.addButton(&btnSaturday);
  mgrBtnWeekday.addButton(&btnSunday);

  mgrBtnBrightness.addButton(&btnNightBright);
  mgrBtnBrightness.addButton(&btnDayNight);
  mgrBtnBrightness.addButton(&btnDayBright);
  mgrBtnBrightness.attachRotaryEncoder(&rec);
  btnDayBright.setAction(showDayBrightness);
  btnNightBright.setAction(shownNightBrightness);
  btnDayNight.setAction(showDayNight);

  song.max = totalTrackCount;
  sldSong.setLinkedParameter(&fldVolume);
  fldVolume.setLinkedParameter(&sldSong);
  alarmTimeButton.setAction(setHours);
  menuButton.setAction(showAlarm1);
  mcp.begin(); // use default address 0
  matrix.init();
  matrix.fillScreen(0);
  matrix7.begin(0x70);
  rec.init();
  keyb.init(writePinModes, writePullups);
  keyb.setCallback_keyReleased(keyChanged);
}

void showSplash()
{
  matrix.fillScreen(0);
  matrix.setFont(&Picopixel);
  matrix.setCursor(4, 10);
  matrix.setFont(&TomThumb);
  matrix.setCursor(4, 10);
  matrix.print("SPLASH");
  matrix.write(); // Send bitmap to display
  delay(1000);
  matrix.fillScreen(0);
}

void drawClock(ClockTime ct)
{
  byte font = 4; //0 to 4
  matrix.drawBitmap(-3, 1, bigFont[(ct.hours / 10 != 0 ? ct.hours / 10 : 10) + 11 * font], 8, 12, 1);
  matrix.drawBitmap(5, 1, bigFont[(ct.hours % 10) + 11 * font], 8, 12, 1);
  matrix.drawBitmap(16, 1, bigFont[(ct.mins / 10 != 0 ? ct.mins / 10 : 10) + 11 * font], 8, 12, 1);
  matrix.drawBitmap(24, 1, bigFont[(ct.mins % 10) + 11 * font], 8, 12, 1);
  matrix.fillRect(15, 4, 2, 2, 1);
  matrix.fillRect(15, 10, 2, 2, 1);
  setLedArrayBrightness(actionMgr.isDark() ? *nightBright.cur : *dayBright.cur);
}

void hideClock(void)
{
  matrix.fillScreen(0);
}

void showAlarm(byte alarmNr)
{
  clockface.setVisible(false);
  matrix.fillScreen(0);
  matrix.setFont(&TomThumb);
  matrix.setCursor(4, 10);
  matrix.print("ALARM");
  matrix.print(alarmNr);
  matrix.write(); // Send bitmap to display
  delay(1000);
  matrix.fillScreen(0);
  matrix.setCursor(15, 10);
  matrix.print(alarmNr);
  mgrBtnAlarm.enable();
  mgrBtnWeekday.enable();
  fldHours.setVisible(true);
  fldMinutes.setVisible(true);
  matrix.write(); // Send bitmap to display
  alarmTimeButton.enable();
  clockMode = false;
}

void showDayBrightness(bool action)
{
  if (clockMode)
  {
    clockface.setVisible(!action);
  }
  fldDayBright.setVisible(action);
}

void shownNightBrightness(bool action)
{
  if (clockMode)
  {
    clockface.setVisible(!action);
  }
  fldNightBright.setVisible(action);
}

void showDayNight(bool action)
{
  if (clockMode)
  {
    clockface.setVisible(!action);
  }
  fldDayNight.setVisible(action);
}

void MenuMgr::assignCommonConfig(CommonConfig *config)
{
  dayBright.cur = &config->dayBright;
  dayNight.cur = &config->dayNight;
  nightBright.cur = &config->nightBright;
}

void MenuMgr::assignAlarmConfig(AlarmConfig *config)
{
  lightness.cur = &config->lightness;
  volume.cur = &config->volume;
  song.cur = &config->song;
  hours.cur = &config->hours;
  minutes.cur = &config->minutes;
  tglMonday.setSource(&config->weekdays[0]);
  tglTuesday.setSource(&config->weekdays[1]);
  tglWednesday.setSource(&config->weekdays[2]);
  tglThursday.setSource(&config->weekdays[3]);
  tglFriday.setSource(&config->weekdays[4]);
  tglSaturday.setSource(&config->weekdays[5]);
  tglSunday.setSource(&config->weekdays[6]);
}
