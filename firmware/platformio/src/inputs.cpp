#include "inputs.h"

//Hardware peripherals

//Tactile elements
RotaryEncoder_Tactiles ret(pin_Data, pin_Clk);
Sx1509_Tactiles st1(pin_KeyPad_IRQ);
Sx1509_Tactiles st2(pin_KeyPad_IRQ);
Gpio_Tactiles gpiot(pin_switch, INPUT_PULLDOWN);

bool setupInputs(SX1509* io1, SX1509* io2)
{
    st1.init(io1, 4, 2);
    st2.init(io2, 4, 1);
    return true;
}

bool inputsChanged()
{
    return ret.isChanged() || st1.isChanged() || st2.isChanged() || gpiot.isChanged();
}

bool isRotaryEncoderChanged(RotaryEncoder_Tactiles::DIRECTION &dir)
{
    if (!ret.isChanged())
    {
        return false;
    }
    dir = ret.getDirection();
    return true;
}

bool isButtonChanged(KEY_CODE &key)
{
    if (st1.isChanged())
    {
        key = (KEY_CODE)st1.getValue();
        return true;
    }
    if (st2.isChanged())
    {
        key = (KEY_CODE)(st2.getValue() | 0x1000);
        return true;
    }
    return false;
}