#include "Tactiles.h"

Sx1509_Tactiles::Sx1509_Tactiles(byte pinIrq) :
                                                            _pin_irq(pinIrq),
                                                            _keyReleaseTimeout(100, AsyncDelay::MILLIS)
{
}

void Sx1509_Tactiles::init(SX1509 *sx, byte keyRows, byte keyCols)
{
    _sx = sx;
    // Scan time range: 1-128 ms, powers of 2
    const byte scanTime = 16; // Scan time per row, in ms
    // Debounce time range: 0.5 - 64 ms (powers of 2)
    const byte debounceTime = 8; // Debounce time
    // Sleep time range: 128 ms - 8192 ms (powers of 2) 0=OFF
    const byte sleepTime = 128;
    // Scan time must be greater than debounce time!
    _sx->keypad(keyRows, keyCols, sleepTime, scanTime, debounceTime);
    pinMode(_pin_irq, INPUT_PULLUP);
}

//\return true when new key pressed or when a key is released
bool Sx1509_Tactiles::isChanged()
{
    //I preferred using IRQ instead of polling this line, but that didn't work out well.
    if (digitalRead(_pin_irq)==LOW)
    {
        //IRQ low as long as a key is being pressed
        int keyData = _sx->readKeypad();
        _keyReleaseTimeout.restart();
        if (keyData != _previousKeyData)
        {
            _previousKeyData = keyData;
            return true;    //Key pressed
        }
    }
    if (_keyReleaseTimeout.isExpired() && _previousKeyData != 0)
    {
        _previousKeyData = 0; //so that the same key can be pressed again.
        return true; //Key released
    }
    return false;
}

//\return keycode of pressed key or 0 when no keys are pressed
int Sx1509_Tactiles::getValue()
{
    return _previousKeyData;
}