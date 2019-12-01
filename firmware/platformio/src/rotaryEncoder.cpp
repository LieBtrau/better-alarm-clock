#include "rotaryEncoder.h"

static RotaryEncoderConsumer* reconsm;

RotaryEncoderConsumer::RotaryEncoderConsumer(RotaryEncoder_MCP23017* renc) : rotenc(renc)
{
  reconsm = this;
}

void increaseRotEnc()
{
  reconsm->increase();
}

void decreaseRotEnc()
{
  reconsm->decrease();
}

void RotaryEncoderConsumer::setConsumer(ParameterUpdate *p, bool flash)
{
  if (_p != nullptr)
  {
    if (flashing)
    {
      _p->render(true); //so that it doesn't stay hidden if it was flashing
      deviceUpdateNeeded = true;
    }
    if ((p != _p->getLinkedParameter()) || p==nullptr)
    {
      _p->stopAction();
    }
  }
  if (p != nullptr)
  {
    p->doAction();
  }
  _p = p;
  flashing = flash;
}

ParameterUpdate *RotaryEncoderConsumer::getConsumer()
{
  return _p;
}

void RotaryEncoderConsumer::increase()
{
  if (_p != nullptr)
  {
    _p->increase();
  }
}

void RotaryEncoderConsumer::decrease()
{
  if (_p != nullptr)
  {
    _p->decrease();
  }
}

void RotaryEncoderConsumer::poll()
{
  rotenc->poll();
}

bool RotaryEncoderConsumer::render()
{
  bool ret = deviceUpdateNeeded;
  deviceUpdateNeeded = false;
  if (flashing && _p != nullptr)
  {
    return ret || _p->flash();
  }
  return ret;
}

void RotaryEncoderConsumer::init()
{
  rotenc->setClockwiseCallback(increaseRotEnc);
  rotenc->setCounterClockwiseCallback(decreaseRotEnc);
}