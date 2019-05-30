#pragma once
#include "Adafruit_MCP23017.h"

typedef void (*voidFuncPtrVoid)(void);

class RotaryEncoder_MCP23017
{
public:
    RotaryEncoder_MCP23017(Adafruit_MCP23017 *mcp, uint32_t irqPin);
    void init();
    void poll();
    void setClockwiseCallback(voidFuncPtrVoid callback);
    void setCounterClockwiseCallback(voidFuncPtrVoid callback);

private:
    Adafruit_MCP23017 *_mcp;
    uint32_t _irqPin;
    const byte PIN1 = 13;
    const byte PIN2 = 14;
    const byte PIN1_MASK = 0x20;
    const byte PIN2_MASK = 0x40;
    const byte GPIO_PORT = 1; // 1 = PORTB (because pin 13 & pin 14 belong to port B, not port A)
    voidFuncPtrVoid _cwCallback = nullptr;
    voidFuncPtrVoid _ccwCallback = nullptr;
};
