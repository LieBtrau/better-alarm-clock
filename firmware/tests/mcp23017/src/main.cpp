#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Chaplex.h"

byte ctrlpins[] = {8,9,10,11,12}; //MCP23017 pins controlling charlieplexed leds
Chaplex myCharlie(ctrlpins, sizeof(ctrlpins));
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

void mcp23017digitalwrite(uint32_t pin, uint32_t state)
{
  mcp.digitalWrite(pin, state);
}

void mcp23017pinmode(uint32_t pin, uint32_t state)
{
  mcp.pinMode(pin, state);
}

void setup()
{
  mcp.begin(); // use default address 0
  myCharlie.setPinmode(mcp23017pinmode);
  myCharlie.setDigitalWrite(mcp23017digitalwrite);
  myCharlie.setLedState(ledmatrix[1], ON);
  myCharlie.setLedState(ledmatrix[12], ON);
}

// flip the pin #0 up and down
void loop()
{
  myCharlie.showLedState();
  delay(1);
}