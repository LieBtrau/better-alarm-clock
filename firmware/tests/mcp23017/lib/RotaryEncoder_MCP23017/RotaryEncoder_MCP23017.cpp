#include "RotaryEncoder_MCP23017.h"

static byte lastPos = 1;
static volatile bool updated = false;

static void rotaryEncUpdate()
{
    updated = true;
}

RotaryEncoder_MCP23017::RotaryEncoder_MCP23017(Adafruit_MCP23017 *mcp, uint32_t irqPin) : _mcp(mcp), _irqPin(irqPin)
{
}

void RotaryEncoder_MCP23017::init()
{
    _mcp->pinMode(PIN1, INPUT);
    _mcp->pinMode(PIN2, INPUT);
    _mcp->setupInterrupts(true, false, LOW);
    _mcp->setupInterruptPin(PIN1, CHANGE);
    pinMode(_irqPin, INPUT_PULLUP);
    attachInterrupt(_irqPin, rotaryEncUpdate, FALLING);
}

void RotaryEncoder_MCP23017::poll()
{
    if (updated || !digitalRead(_irqPin)) //digitalRead is only here to make sure IRQ always gets cleared on MCP23017.
    {
        //rotate CCW : 0,0x60,0,0x60, ...
        //rotate CW : 0x40,0x20,0x40,0x20, ...
        byte curPos = _mcp->readInterruptCapture(GPIO_PORT) & (PIN1_MASK | PIN2_MASK);

        if (!lastPos && curPos == (PIN1_MASK | PIN2_MASK))
        {
            if (_ccwCallback)
            {
                _ccwCallback();
            }
        }
        if (lastPos == PIN2_MASK && curPos == PIN1_MASK)
        {
            if (_cwCallback)
            {
                _cwCallback();
            }
        }
        lastPos = curPos;
        updated = false;
    }
}

void RotaryEncoder_MCP23017::setClockwiseCallback(voidFuncPtrVoid callback)
{
    _cwCallback = callback;
}

void RotaryEncoder_MCP23017::setCounterClockwiseCallback(voidFuncPtrVoid callback)
{
    _ccwCallback = callback;
}
