#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Chaplex.h"
#include "KeyboardScan.h"

byte ctrlpins[] = {8, 9, 10, 11, 12}; //MCP23017 pins controlling charlieplexed leds
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
volatile bool updated = false;
byte pinData = 0;

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

void rotaryEncUpdate()
{
  updated = true;
}

void setup()
{
  Serial.begin(115200);
  mcp.begin(); // use default address 0
  mcp.pinMode(13, INPUT);
  mcp.pinMode(14, INPUT);
  mcp.setupInterrupts(true, false, LOW);
  mcp.setupInterruptPin(13, CHANGE);
  pinMode(PB1, INPUT_PULLUP);
  attachInterrupt(PB1, rotaryEncUpdate, FALLING);
  keyb.init(writePinModes, writePullups);
}

byte lastPos = 1;
void loop()
{
  showLedState();
  updateLEDs();
  if (keyb.updateKeys(writeGpio, readGpio))
  {
    Serial.println(keyb.getKeys());
  }
  if (updated || !digitalRead(PB1))
  {
    //rotate CCW : 0,96,0,96, ...
    //rotate CW : 64,32,64,32, ...
    byte curPos = mcp.readInterruptCapture(1) & 0x60;

    if (!lastPos && curPos == 96)
    {
      Serial.print("-");
    }
    if (lastPos == 64 && curPos == 32)
    {
      Serial.print("+");
    }
    lastPos = curPos;
    updated = false;
  }
}