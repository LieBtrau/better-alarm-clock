#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Chaplex.h"
#include "KeyboardScan.h"
#include "RotaryEncoder_MCP23017.h"

Chaplex myCharlie;
CharlieLed ledmatrix[15] = {
    //{anode_registerbit, cathode_registerbit},
    {0, 1}, //D1
    {1, 0}, //D2
    {1, 2}, //D3
    {2, 1}, //D4
    {2, 3}, //D5
    {3, 4}, //D6
    {0, 2}, //D7
    {3, 2}, //D8
    {4, 3}, //D9
    {2, 0}, //D10
    {2, 4}, //D11
    {4, 2}, //D12
    {1, 3}, //D13
    {0, 4}, //D14
    {4, 0}  //D15
};
Adafruit_MCP23017 mcp;
KeyboardScan keyb;
byte pinData = 0;
RotaryEncoder_MCP23017 rotenc(&mcp, PB1);

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
  byte pinModes = mcp.readPinMode(1);
  byte gpioStates = mcp.readGPIO(1);
  if (myCharlie.showLedState(pinModes, gpioStates))
  {
    mcp.writePinMode(1, pinModes);
    mcp.writeGPIO(1, gpioStates);
  }
}

void updateLEDs()
{
  static unsigned long ulTimer = millis();
  static byte i = 0;
  if (millis() > ulTimer + 100)
  {
    ulTimer = millis();
    myCharlie.setLedState(ledmatrix[i], OFF);
    i = i + 1 > 14 ? 0 : i + 1;
    myCharlie.setLedState(ledmatrix[i], ON);
  }
}

void increaseRotEnc()
{
  Serial.println("+");
}

void decreaseRotEnc()
{
  Serial.println("-");
}

void keyChanged(byte key)
{
  Serial.println(key);
}

void setup()
{
  Serial.begin(115200);
  mcp.begin(); // use default address 0
  rotenc.init();
  rotenc.setClockwiseCallback(increaseRotEnc);
  rotenc.setCounterClockwiseCallback(decreaseRotEnc);
  keyb.init(writePinModes, writePullups);
  keyb.setCallback_keyReleased(keyChanged);
}

void loop()
{
  showLedState();
  updateLEDs();
  keyb.updateKeys(writeGpio, readGpio);
  rotenc.poll();
}