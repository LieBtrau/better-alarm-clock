#include "Max72xxPanel.h"
#include <SPI.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Menu.h"
#include "Adafruit_MCP23017.h"

Adafruit_7segment matrix7 = Adafruit_7segment();
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 2;
const byte pinMOSI = PA7;
const byte pinSCLK = PA5;
const byte pinCS = PA1;
Adafruit_MCP23017 mcp;
Chaplex myCharlie;

void playSong(byte i)
{
  matrix7.print(i, DEC);
  matrix7.writeDisplay();
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

Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
Coordinate topleft = {0, 0};
Coordinate botright = {12, 3};
FieldParameter lightness = {0, 50, 100, 5};
LedMatrixField fldLightness = LedMatrixField(&matrix, topleft, botright, &lightness);

Coordinate topleft1 = {0, 7};
Coordinate botRight1 = {12, 10};
SelectParameter song = {3, 9, playSong};
LedMatrixSelect sldSong = LedMatrixSelect(&matrix, topleft1, botRight1, &song);

CharlieLed ledDD1 = {0, 1};
bool bLightnessSelected=false;
LedToggle tglLightness = LedToggle(&myCharlie, &ledDD1, &bLightnessSelected);

void setup()
{
  mcp.begin(); // use default address 0
  matrix.init();
  fldLightness.render();
  sldSong.render();
  //   matrix.setCursor(2, 5);
  //   matrix.print("99:99");
  //   matrix.write(); // Send bitmap to display

  // #ifndef __AVR_ATtiny85__
  //   Serial.begin(9600);
  //   Serial.println("7 Segment Backpack Test");
  // #endif
  matrix7.begin(0x70);
}

bool dirUp = true;
unsigned long ulTimer = millis();
MenuOut* output=&fldLightness;

void loop()
{
  // if (millis() > ulTimer + 500)
  // {
  //   ulTimer = millis();
  //   if (dirUp)
  //   {
  //     tglLightness.set();
  //     dirUp = false;
  //   }
  //   else
  //   {
  //     tglLightness.clear();
  //     dirUp = true;
  //   }
  //   tglLightness.render();
  // }

  if(dirUp)
  {
  if(!fldLightness.increase())
  {
    dirUp=false;
  }
  }else{
    if(!fldLightness.decrease())
    {
      dirUp=true;
    }
  }
  matrix.fillScreen(0);
  // sldSong.next();
  output->render();
  // sldSong.render();
  matrix.write();
   delay(100);
  //showLedState();
  // // try to print a number thats too long
  // matrix7.print(10000, DEC);
  // matrix7.writeDisplay();
  // delay(500);

  // // print a hex number
  // matrix7.print(0xBEEF, HEX);
  // matrix7.writeDisplay();
  // delay(500);

  // // print a floating point
  // matrix7.print(12.34);
  // matrix7.writeDisplay();
  // delay(500);

  // // print with print/println
  // for (uint16_t counter = 0; counter < 9999; counter++) {
  //   matrix7.println(counter);
  //   matrix7.writeDisplay();
  //   matrix.fillScreen(0);
  //   matrix.setCursor(2, 5);
  //   matrix.print(counter);
  //   matrix.write();
  //   delay(10);
  // }

  // // method #2 - draw each digit
  // uint16_t blinkcounter = 0;
  // boolean drawDots = false;
  // for (uint16_t counter = 0; counter < 9999; counter ++) {
  //   matrix7.writeDigitNum(0, (counter / 1000), drawDots);
  //   matrix7.writeDigitNum(1, (counter / 100) % 10, drawDots);
  //   matrix7.drawColon(drawDots);
  //   matrix7.writeDigitNum(3, (counter / 10) % 10, drawDots);
  //   matrix7.writeDigitNum(4, counter % 10, drawDots);

  //   blinkcounter+=50;
  //   if (blinkcounter < 500) {
  //     drawDots = false;
  //   } else if (blinkcounter < 1000) {
  //     drawDots = true;
  //   } else {
  //     blinkcounter = 0;
  //   }
  //   matrix7.writeDisplay();
  //   delay(10);
  // }
}