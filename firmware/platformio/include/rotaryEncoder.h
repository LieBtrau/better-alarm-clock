#pragma once
#include "Menu.h"
#include "RotaryEncoder_MCP23017.h"

class RotaryEncoderConsumer
{
public:
  RotaryEncoderConsumer(RotaryEncoder_MCP23017 *renc);
  void init();
  void setConsumer(ParameterUpdate *p, bool flash);
  ParameterUpdate *getConsumer();
  void increase();
  void decrease();
  void poll();
  bool render();

private:
  RotaryEncoder_MCP23017 *rotenc = nullptr;
  ParameterUpdate *_p = nullptr;
  bool flashing = false;
  bool deviceUpdateNeeded = false;
};
