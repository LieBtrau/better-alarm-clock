#include "inputs.h"

//Hardware peripherals
SX1509 io1, io2;

//Tactile elements
RotaryEncoder_Tactiles ret(pin_Data, pin_Clk);
Sx1509_Tactiles st1(&io1, pin_KeyPad_IRQ);
Sx1509_Tactiles st2(&io2, pin_KeyPad_IRQ);
Gpio_Tactiles gpiot(pin_switch, INPUT_PULLDOWN);

bool setupInputs()
{
    const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
    const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address

    // put your setup code here, to run once:
    if (!io1.begin(IO1_SX1509_ADDRESS))
    {
        return false;
    }
    if (!io2.begin(IO2_SX1509_ADDRESS))
    {
        return false;
    }

    // Use the internal 2MHz oscillator.
    // Set LED clock to 500kHz (2MHz / (2^(3-1)):
    io1.clock(INTERNAL_CLOCK_2MHZ, 3);
    io2.clock(INTERNAL_CLOCK_2MHZ, 3);

    st1.init(4, 2);
    st2.init(4, 1);

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