#pragma once
#include "Menu.h"
#include "RotaryEncoder_MCP23017.h"

class RotaryEncoderConsumer
{
public:
  RotaryEncoderConsumer(Adafruit_MCP23017 *mcp, uint32_t irqPin);
  void init();
  void setConsumer(ParameterUpdate *p, bool flash);
  ParameterUpdate *getConsumer();
  void increase();
  void decrease();
  void poll();
  bool render();
private:
  RotaryEncoder_MCP23017 rotenc;
  ParameterUpdate *_p = nullptr;
  bool flashing=false;
  bool deviceUpdateNeeded=false;
};

