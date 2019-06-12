#include "menuNav.h"
#include "actions.h"
#include "RotaryEncoder_MCP23017.h"
#include "pins.h"

extern FieldParameter lightness;
extern FieldParameter volume;
extern FieldParameter dayBright;
extern FieldParameter dayNight;
extern FieldParameter nightBright;
extern SelectParameter song;
extern bool weekdays[7];
extern bool bAlarmSelected;
extern bool bMenuSelected;
extern bool matrixFields[6];

extern FieldParameter hours;

// 32x16 LED Matrix elements
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

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

LedToggle tglMonday = LedToggle(&myCharlie, &ledMatrix[MONDAY], weekdays);
LedToggle tglTuesday = LedToggle(&myCharlie, &ledMatrix[TUESDAY], weekdays + 1);
LedToggle tglWednesday = LedToggle(&myCharlie, &ledMatrix[WEDNESDAY], weekdays + 2);
LedToggle tglThursday = LedToggle(&myCharlie, &ledMatrix[THURSDAY], weekdays + 3);
LedToggle tglFriday = LedToggle(&myCharlie, &ledMatrix[FRIDAY], weekdays + 4);
LedToggle tglSaturday = LedToggle(&myCharlie, &ledMatrix[SATURDAY], weekdays + 5);
LedToggle tglSunday = LedToggle(&myCharlie, &ledMatrix[SUNDAY], weekdays + 6);

LedToggle tglAlarm = LedToggle(&myCharlie, &ledMatrix[ALARMTIME], &bAlarmSelected);
LedToggle tglMenu = LedToggle(&myCharlie, &ledMatrix[MENU], &bMenuSelected);

// Seven segment display elements
Adafruit_7segment matrix7 = Adafruit_7segment();
SevenSegmentField fldHours = SevenSegmentField(&matrix7, SevenSegmentField::RIGHTPOS, &hours);

PushButton matrixButtons[] =
    {{LIGHTNESS, &tglLightness, &fldLightness},
     {VOLUME, &tglVolume, &fldVolume},
     {SONGCHOICE, &tglSongChoice, &sldSong},
     {DAYDISPLAYBRIGHTNESS, &tglDayBrightness, &fldDayBright},
     {DAYNIGHTLEVEL, &tglDayNight, &fldDayNight},
     {NIGHTDISPLAYBRIGHTNESS, &tglNightBrightness, &fldNightBright}};
Adafruit_MCP23017 mcp;
KeyboardScan keyb;
RotaryEncoder_MCP23017 rotenc(&mcp, pinIRQ);
ParameterUpdate *rotencProcessor = nullptr;

bool dirUp = true;
bool dirUp1 = true;
unsigned long ulTimer = millis();
unsigned long ulTimer1 = millis();

void animation()
{
  if (millis() > ulTimer1 + 500)
  {
    ulTimer1 = millis();
    sldSong.increase();
  }

  if (dirUp)
  {
    if (!fldLightness.increase())
    {
      dirUp = false;
    }
  }
  else
  {
    if (!fldLightness.decrease())
    {
      dirUp = true;
    }
  }
}

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
  for (int i = 0; i < 6; i++)
  {
    matrixButtons[i].doAction(matrixButtons[i].key() == key);
    if (matrixButtons[i].key() == key)
    {
      rotencProcessor = matrixButtons[i].getParam();
    }
  }
}

void increaseRotEnc()
{
  if (rotencProcessor)
  {
    rotencProcessor->increase();
  }
}

void decreaseRotEnc()
{
  if (rotencProcessor)
  {
    rotencProcessor->decrease();
  }
}

void renderMenu()
{
  keyb.updateKeys(writeGpio, readGpio);
  rotenc.poll();
  if (fldLightness.render() || sldSong.render() || fldVolume.render() || fldDayBright.render() || fldDayNight.render() || fldNightBright.render())
  {
    matrix.write();
  }
  if (fldHours.render())
  {
    matrix7.writeDisplay();
  }
  for (int i = 0; i < 6; i++)
  {
    matrixLEDs[i]->render();
  }
  showLedState();
}

void initMenu()
{
  mcp.begin(); // use default address 0
  matrix.init();
  matrix.fillScreen(0);
  matrix7.begin(0x70);
  rotenc.init();
  rotenc.setClockwiseCallback(increaseRotEnc);
  rotenc.setCounterClockwiseCallback(decreaseRotEnc);
  fldLightness.render();
  fldVolume.render();
  fldHours.render();
  sldSong.render();
  fldDayBright.render();
  fldDayNight.render();
  fldNightBright.render();
  matrix.setFont(&Picopixel);
  matrix.setCursor(4, 10);
  matrix.setFont(&TomThumb);
  matrix.setCursor(4, 10);

  matrix.print("ALARM3");
  matrix.write(); // Send bitmap to display
  keyb.init(writePinModes, writePullups);
  keyb.setCallback_keyReleased(keyChanged);
}
