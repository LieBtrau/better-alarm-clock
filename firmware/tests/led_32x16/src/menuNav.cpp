#include "menuNav.h"
#include "actions.h"

extern FieldParameter lightness;
extern SelectParameter song;
extern bool bLightnessSelected;
extern FieldParameter hours;


Adafruit_7segment matrix7 = Adafruit_7segment();
Chaplex myCharlie;

int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA1;
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
Coordinate topleft = {0, 0};
Coordinate botright = {11, 2};
LedMatrixField fldLightness = LedMatrixField(&matrix, topleft, botright, &lightness);

Coordinate topleft1 = {0, 7};
Coordinate botRight1 = {11, 9};
LedMatrixSelect sldSong = LedMatrixSelect(&matrix, topleft1, botRight1, &song);

CharlieLed ledDD1 = {0, 1};
LedToggle tglLightness = LedToggle(&myCharlie, &ledDD1, &bLightnessSelected);

SevenSegmentField fldHours = SevenSegmentField(&matrix7, SevenSegmentField::RIGHTPOS, &hours);

void disableButtonLightness(LedToggle *led);
void enableButtonLightness(LedToggle *led);

PushButton buttons[1] = {{&tglLightness, enableButtonLightness}};
Adafruit_MCP23017 mcp;
KeyboardScan keyb;
bool dirUp = true;
bool dirUp1 = true;
unsigned long ulTimer = millis();
unsigned long ulTimer1 = millis();
MenuOut *output = &fldLightness;

void animation()
{
  if (millis() > ulTimer1 + 500)
  {
    ulTimer1 = millis();
    sldSong.next();
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

void disableButtonLightness(LedToggle *led)
{
    led->clear();
    buttons[0].doAction = enableButtonLightness;
}

void enableButtonLightness(LedToggle *led)
{
    led->set();
    buttons[0].doAction = disableButtonLightness;
}

void keyChanged(byte key)
{
    buttons[0].doAction(buttons[0].led);
}

void renderMenu()
{
  keyb.updateKeys(writeGpio, readGpio);
  if (output->render() || sldSong.render())
  {
    matrix.write();
  }
  if (fldHours.render())
  {
    matrix7.writeDisplay();
  }
  tglLightness.render();
  showLedState();
}

void initMenu()
{
    mcp.begin(); // use default address 0
    matrix.init();
    matrix.fillScreen(0);
    matrix7.begin(0x70);
    fldLightness.render();
    fldHours.render();
    sldSong.render();
    matrix.setFont(&Picopixel);
    matrix.setCursor(4, 10);
    matrix.setFont(&TomThumb);
    matrix.setCursor(4, 10);

    matrix.print("ALARM2");
    matrix.write(); // Send bitmap to display
    keyb.init(writePinModes, writePullups);
    keyb.setCallback_keyReleased(keyChanged);
}