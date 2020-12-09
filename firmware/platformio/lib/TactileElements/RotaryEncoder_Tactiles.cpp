#include "Tactiles.h"

static RotaryEncoder *pLocalRe = nullptr;

static void blink()
{
    pLocalRe->tick(); // just call tick() to check the state.
}

RotaryEncoder_Tactiles::RotaryEncoder_Tactiles(byte pinData, byte pinClock) : _re(pinData, pinClock)
{
    pLocalRe = &_re;
    attachInterrupt(digitalPinToInterrupt(pinClock), blink, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinData), blink, CHANGE);
}

bool RotaryEncoder_Tactiles::isChanged()
{
    int oldPos = _lastPosition;
    _lastPosition = _re.getPosition();
    return oldPos != _lastPosition;
}

int RotaryEncoder_Tactiles::getValue()
{
    return _lastPosition;
}
